#ifndef GTKMM_GIFAUTHOR_VIDEOFILE_H
#define GTKMM_GIFAUTHOR_VIDEOFILE_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
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

        /**
         * Read the next frame
         * @param frame The frame to decode into, NULL to create a new frame
         * @returns the decoded frame
         */
        bool get_next_frame(AVFrame **out);

        /**
         * Read the previous frame
         * @param frame The frame to decode into, NULL to create a new frame
         * @returns the decoded frame
         */
        bool get_prev_frame(AVFrame **out);


        /**
         * Get the current timestamp
         * @returns current timestamp
         */
        int16_t get_timestamp();

        /**
         * Get current frame number
         * @returns current frame index
         */
        int16_t get_frame_index();

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
        int64_t frameLength;

        AVFrame *orig_frame;
        uint8_t *buffer;
        

};

#endif 
