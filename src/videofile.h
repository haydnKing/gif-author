#ifndef GTKMM_GIFAUTHOR_VIDEOFILE_H
#define GTKMM_GIFAUTHOR_VIDEOFILE_H

#include "video.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

/**
 * A video that's being read from disk
 */
class VideoFile: public Video
{
    public:
        /**
         * default constructor
         */
        VideoFile();
        virtual ~VideoFile();

    //member functions

        /**
         * Open a video file
         * @returns true on success
         */
        bool open(const char* filename);

        /**
         * is a file open?
         * @returns True if a file is open
         */
        bool is_open();

        /**
         * Close an open file and deallocate all memory
         */
        void close();

        /**
         * get the timestamp of the current frame
         */
        int64_t timestamp();

    //inherited functions

        virtual bool is_ok();

        virtual int width();

        virtual int height();

        virtual int64_t position();

        virtual int64_t length();

        virtual int64_t frame_duration_ms();

        virtual bool seek_to(int64_t index, bool wrap=true);

        virtual Glib::RefPtr<VideoFrame> get_frame();

    protected:
        void init();

        AVFrame *new_avframe();
        bool decode_frame();
        bool convert_frame(AVFrame **out);
        bool decode_convert_frame(AVFrame **out);
        static bool done_init;

        AVFormatContext *formatCtx;
        AVCodecContext *codecCtx;
        AVCodec *codec;
        SwsContext *swsCtx;
        int videoStream;
        int64_t frameLength, numFrames;

        AVFrame *orig_frame;
        uint8_t *buffer;

};

#endif 
