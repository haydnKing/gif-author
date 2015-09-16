#include "videosequence.h"

VideoSequence::VideoSequence(int w, int h):
    width(w),
    height(h)
{};

VideoSequence::~VideoSequence(){};

Glib::RefPtr<VideoSequence> VideoSequence::create(int width, int height){
    return Glib::RefPtr<VideoSequence>(new VideoSequence(width, height));
};

Glib::RefPtr<VideoSequence> VideoSequence::create_from_video(
        Video& rhs,
        int64_t start_frame,
        int64_t end_frame){
    //check that the video is OK
    if(not rhs.is_ok()){
        return Glib::RefPtr<VideoSequence>(NULL);
    }
    //record initial position
    int64_t pos = rhs.position();

    //sanitise start and end frames
    if(start_frame < 0){
        start_frame = 0;
    }
    if(end_frame < 0){
        end_frame = rhs.length();
    }

    //Create the return sequence
    Glib::RefPtr<VideoSequence> ret = VideoSequence::create(rhs.width(), rhs.height());
    Glib::RefPtr<VideoFrame> frame;

    //seek to start frame
    rhs.seek_to(start_frame);
    //fetch frames until the end or we run out of frames
    while(rhs.position() < end_frame){
       frame = rhs.get_frame();
       if(!frame){
           break;
       }
       ret->append(frame);
    }

    //seek back to the position we were at
    rhs.seek_to(pos);

    return ret;
};

