#include "videosequence.h"

VideoSequence::VideoSequence(int w, int h):
    frame_width(w),
    frame_height(h),
    p(0)
{
    it = frames.begin();
};

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

void VideoSequence::append(Glib::RefPtr<VideoFrame>& frame){
    if(frame->width() == width() && frame->height() == height())
        frame.push_back(frame);
};

bool VideoSequence::is_ok(){
    return frame_width > 0 && frame_height > 0;
};

int VideoSequence::width(){
    return frame_width;
};

int VideoSequence::height(){
    return frame_height;
};

int64_t VideoSequence::position(){
    return p;
};

int64_t VideoSequence::length(){
    return frames.size();
};

int64_t VideoSequence::frame_duration_ms(){
    return 40;
};

bool VideoSequence::seek_to(int64_t index, bool wrap){};

Glib::RefPtr<VideoFrame> VideoSequence::get_frame(){
    if(frames.empty()){
        return Glib::RefPtr<VideoFrame>(NULL);
    }

};

