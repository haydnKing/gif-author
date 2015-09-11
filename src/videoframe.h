#ifndef GTKMM_GIFAUTHOR_VIDEOFRAME_H
#define GTKMM_GIFAUTHOR_VIDEOFRAME_H

#include <glibmm/main.h>

extern "C" {
#include <libavformat/avformat.h>
}

/**
 * Hold a frame of video
 */
class VideoFrame
{
    public:
        virtual ~VideoFrame();
        /*
         * Create an empty Frame
         */
        static Glib::RefPtr<VideoFrame> create();
        /*
         * Create a frame from an AVFrame
         */
        static Glib::RefPtr<VideoFrame> create_from_AVFrame(AVFrame& rhs);

        /*
         * frame height
         */
        int height() const;
        /*
         * frame width
         */
        int width() const;
        /*
         * size of each padded row
         */
        int rowstride() const;
        /*
         * data size
         */
        int datasize() const;

        /*
         * get a const pointer to the data
         */
        const uint8_t* data() const;

        /*
         * get a pointer to the data
         */
        uint8_t* data();

    protected:
        VideoFrame();
        
};

#endif // GTKMM_GIFAUTHOR_VIDEOFRAME_H
