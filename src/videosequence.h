#ifndef GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H
#define GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H

#include "video.h"

/**
 * A sequence of in memory video frames
 */
class VideoSequence: public Video
{
    public:
       // Inherited members 
        virtual bool is_ok();
        virtual int width();
        virtual int height();
        virtual int64_t position();
        virtual int64_t length();
        virtual int64_t frame_duration_ms();
        virtual bool seek_to(int64_t index, bool wrap=true);
        virtual Glib::RefPtr<VideoFrame> get_frame();
};
         

#endif // GTKMM_GIFAUTHOR_VIDEOSEQUENCE_H
