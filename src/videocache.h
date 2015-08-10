#ifndef GTKMM_GIFAUTHOR_VIDEOCACHE_H
#define GTKMM_GIFAUTHOR_VIDEOCACHE_H

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class VideoCache
{
    public:
        /**
         * default constructor
         */
        VideoCache();
        virtual ~VideoCache();

        /**
         * Open a video file
         * \returns true on success
         */
        bool open(const char* filename);
        /**
         * is a file open?
         * \returns True if a file is open
         */
        bool isOpen();

        /**
         * Close an open file and deallocate all memory
         */
        void close();

    protected:
        void init();
        static bool done_init;

        AVFormatContext *formatCtx;
        AVCodecContext *codecCtx;
        AVCodec *codec;
        int videoStream;

};

#endif 
