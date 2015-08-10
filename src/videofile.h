#ifndef GTKMM_GIFAUTHOR_VIDEOFILE_H
#define GTKMM_GIFAUTHOR_VIDEOFILE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class VideoFile
{
    public:
        /**
         * default constructor
         */
        VideoFile();
        virtual ~VideoFile();

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

        /**
         * Frame width, -1 if no file open
         */
        int width();

        /**
         * Frame height, -1 if no file open
         */
        int height();

    protected:
        void init();
        static bool done_init;

        AVFormatContext *formatCtx;
        AVCodecContext *codecCtx;
        AVCodec *codec;
        int videoStream;

};

#endif 
