#include "videocache.h"

bool VideoCache::done_init = false;

VideoCache::VideoCache(){
   init();
}

VideoCache::~VideoCache(){
    if(isOpen()){
        close();
    }
}

bool VideoCache::open(const char* filename){
    AVCodecContext *tempCodecContext;

    //close anything that's already open
    close();

    if(avformat_open_input(formatCtx, filename, NULL, 0, NULL)!=0){
        return false; //failed to open file
    }

    if(avformat_find_stream_info(formatCtx, NULL)<0){
        return false; // Couldn't find stream information
    }

    av_dump_format(formatCtx, 0, filename, 0);

    // Find the first video stream
    videoStream=-1;
    for(int i=0; i<pFormatCtx->nb_streams; i++)
      if(formatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
        videoStream=i;
        break;
      }
    if(videoStream==-1){
      return false; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    tempCodecCtx = formatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    codec = avcodec_find_decoder(tempCodecCtx->codec_id);
    if(codec==NULL) {
        return false;
    }
    // Copy context
    codecCtx = avcodec_alloc_context3(codec);
    if(avcodec_copy_context(codecCtx, tempCodecContext) != 0) {
      return false; // Error copying codec context
    }
    // Open codec
    if(avcodec_open2(codecCtx, codec)<0){
      return false; // Could not open codec
    }
}

bool VideoCache::isOpen(){

}


void VideoCache::init(){
    if(!done_init){
        av_register_all();
        done_init = true;
    }
}
