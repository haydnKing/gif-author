#include "videofile.h"
#include <iostream>

bool VideoFile::done_init = false;

VideoFile::VideoFile(){
   init();
   formatCtx = NULL;
   codecCtx = NULL;
   codec = NULL;
   swsCtx = NULL;
   videoStream = -1;

   orig_frame=NULL;
   buffer=NULL;
}

VideoFile::~VideoFile(){
    if(isOpen()){
        close();
    }
}

bool VideoFile::open(const char* filename){
    AVCodecContext *tempCodecCtx;
    lastKey = 0;

    //close anything that's already open
    close();

    if(avformat_open_input(&formatCtx, filename, NULL, NULL)!=0){
        close();
        return false; //failed to open file
    }

    if(avformat_find_stream_info(formatCtx, NULL)<0){
        close();
        return false; // Couldn't find stream information
    }

    av_dump_format(formatCtx, 0, filename, 0);

    // Find the first video stream
    videoStream=-1;
    for(int i=0; i<formatCtx->nb_streams; i++)
        if(formatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    if(videoStream==-1){
        close();
        return false; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    tempCodecCtx = formatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    codec = avcodec_find_decoder(tempCodecCtx->codec_id);
    if(codec==NULL) {
        close();
        return false;
    }
    // Copy context
    codecCtx = avcodec_alloc_context3(codec);
    if(avcodec_copy_context(codecCtx, tempCodecCtx) != 0) {
        close();
        return false; // Error copying codec context
    }
    // Open codec
    if(avcodec_open2(codecCtx, codec, NULL)<0){
        close();
        return false; // Could not open codec
    }

    // initialize SWS context for software scaling
    swsCtx = sws_getCachedContext(swsCtx,
                                  codecCtx->width,
                                  codecCtx->height,
                                  codecCtx->pix_fmt,
                                  codecCtx->width,
                                  codecCtx->height,
                                  PIX_FMT_RGB24,
                                  SWS_BILINEAR,
                                  NULL,
                                  NULL,
                                  NULL
                                  );

    orig_frame = av_frame_alloc();

    frameLength = (formatCtx->streams[videoStream]->time_base.den * 
                   formatCtx->streams[videoStream]->avg_frame_rate.den) / 
                  (formatCtx->streams[videoStream]->time_base.num *
                   formatCtx->streams[videoStream]->avg_frame_rate.num);

    return true;
}

bool VideoFile::isOpen(){
    return (codecCtx != NULL) && (formatCtx != NULL);
}

void VideoFile::close(){

    if(codecCtx != NULL){
        avcodec_close(codecCtx);
        codecCtx = NULL;
    }
    codec = NULL;
    if(formatCtx != NULL){
        avformat_close_input(&formatCtx);
        formatCtx = NULL;
    }

    if(swsCtx != NULL){
        sws_freeContext(swsCtx);
        swsCtx = NULL;
    }

    if(orig_frame != NULL){
        av_free(orig_frame);
        orig_frame = NULL;
    }
    if(buffer != NULL){
        av_free(buffer);
        buffer = NULL;
    }
}

int VideoFile::width(){
    if(isOpen()){
        return codecCtx->width;
    }
    return -1;
}

int VideoFile::height(){
    if(isOpen()){
        return codecCtx->height;
    }
    return -1;
}


void VideoFile::init(){
    if(!done_init){
        av_register_all();
        done_init = true;
    }
}

AVFrame *VideoFile::new_avframe(){
    
    AVFrame *ret_frame = av_frame_alloc();

    ret_frame->format = AV_PIX_FMT_RGB24;
    ret_frame->width = codecCtx->width;
    ret_frame->height = codecCtx->height;

    if(av_frame_get_buffer(ret_frame, 4)<0){
        return NULL;
    };
    
    return ret_frame;
}

bool VideoFile::get_next_frame(AVFrame **out){
    return decode_frame(out);
}

bool VideoFile::decode_frame(AVFrame **out){
    if(*out == NULL){
        *out = new_avframe();
        if(*out == NULL){
            return false;
        }
    }

    AVPacket packet;
    int frameFinished;
    while(av_read_frame(formatCtx, &packet)>=0) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) {
            // remember key frames
            if(orig_frame->key_frame){
                lastKey = av_frame_get_best_effort_timestamp(orig_frame);
            }
            // Decode video frame
            avcodec_decode_video2(codecCtx, orig_frame, &frameFinished, &packet);

            // Did we get a video frame?
            if(frameFinished) {
                // Convert the image from its native format to RGB
                sws_scale(swsCtx, 
                          (uint8_t const * const *)orig_frame->data,
                          orig_frame->linesize, 
                          0, 
                          codecCtx->height,
                          (*out)->data, 
                          (*out)->linesize);

                //copy properties
                av_frame_copy_props(*out, orig_frame);

                //Return the completed frame
                av_free_packet(&packet);
                return true;
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    return false;
}

bool VideoFile::get_prev_frame(AVFrame **out){
    uint16_t pts;
    if(orig_frame){
        //return false if we're at the start
        if(get_timestamp() == 0){
            return false;
        }
        //hop back to the last key frame
        if(av_seek_frame(formatCtx, 
                         videoStream, 
                         lastKey,
                         AVSEEK_FLAG_ANY && AVSEEK_FLAG_BACKWARD) < 0){
            return false;
        }
        avcodec_flush_buffers(codecCtx);

        //decode forwards until we get to the frame just before where we were
        pts = av_frame_get_best_effort_timestamp(orig_frame) - frameLength;
        if(!get_next_frame(out)) return false;
        while(av_frame_get_best_effort_timestamp(orig_frame) < pts){
            if(!get_next_frame(out)) return false;
        }

        return out;
    }
    return false;
}

int16_t VideoFile::get_timestamp(){
    return av_frame_get_best_effort_timestamp(orig_frame);
}

int16_t VideoFile::get_frame_index(){
    return av_frame_get_best_effort_timestamp(orig_frame) / frameLength;
}



