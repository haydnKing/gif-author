#ifndef GTKMM_GIFAUTHOR_VIDEOFRAME_H
#define GTKMM_GIFAUTHOR_VIDEOFRAME_H

#include <glibmm/main.h>

/**
 * Hold a frame of video
 */
class VideoFrame : public Glib::Object
{
    public:
        virtual ~VideoFrame();
        /*
         * Create an empty Frame
         */
        static Glib::RefPtr<VideoFrame> create();

        /*
         * Create a frame from raw data
         * @param data The raw data to use, 24bpp RGB format
         * @param width image width
         * @param height image height
         * @param rowstride image width plus padding
         * @param copy if true, inisialise a copy of the data leaving the
         * caller responsible for the original. If false, responsibility to
         * delete the data is passed to the VideoFrame
         * @param timestamp the timestamp of the frame, defaults to none
         */
        static Glib::RefPtr<VideoFrame> create_from_data(
                uint8_t* data,
                int width,
                int height,
                int rowstride,
                bool copy=true,
                int64_t timestamp=-1);

        /*
         * frame height
         */
        int get_height() const;
        /*
         * frame width
         */
        int get_width() const;
        /*
         * size of each padded row
         */
        int get_rowstride() const;
        /*
         * data size
         */
        int get_datasize() const;
        /*
         * when to display the frame in ms
         */
        int64_t get_timestamp() const; 

        /*
         * get a const pointer to the data
         */
        const uint8_t* get_data() const;

        /*
         * get a pointer to the data
         */
        uint8_t* get_data();

        /*
         * Check if the VideoFrame is initiated
         */
        bool is_ok() const;

    protected:
        VideoFrame();

    private:
        void init(uint8_t* _data, int w, int h, int r, int64_t t); 

        int height, width, rowstride;
        int64_t timestamp;
        uint8_t* data;
        
};

#endif // GTKMM_GIFAUTHOR_VIDEOFRAME_H
