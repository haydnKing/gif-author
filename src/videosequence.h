#ifndef GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H
#define GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H

#include <list>
#include <glibmm/main.h>
#include "video.h"

/**
 * A sequence of in memory video frames
 */
class VideoSequence: public Video
{
    public:
        virtual ~VideoSequence();

        /*
         * Create an empty VideoSequence
         */
        static Glib::RefPtr<VideoSequence> create(int width, int height);

        /*
         * Create a new VideoSequence from a video, clipping as necessary
         */
        static Glib::RefPtr<VideoSequence> create_from_video(
                Video& rhs, int64_t start_frame=-1, int64_t end_frame=-1);

        /*
         * Add a frame to the end of the sequence
         */
        void append(Glib::RefPtr<VideoFrame>& frame);

       // Inherited members 
        virtual bool is_ok();
        virtual int width();
        virtual int height();
        virtual int64_t position();
        virtual int64_t length();
        virtual int64_t frame_duration_ms();
        virtual bool seek_to(int64_t index, bool wrap=true);
        virtual Glib::RefPtr<VideoFrame> get_frame();

    protected:
        VideoSequence(int width=-1, int height=-1);
        
    private:
        std::list<Glib::RefPtr<VideoFrame>> frames;
        int width, height;
};
         

#endif // GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H
