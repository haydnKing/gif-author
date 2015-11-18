#include "videoframe.h"
#include <cstring> //for memcpy

VideoFrame::VideoFrame():
    width(-1),
    height(-1),
    rowstride(-1),
    timestamp(-1),
{
    data = NULL;
};

VideoFrame::~VideoFrame(){
    if(data != NULL){
        delete data;
    }
};

/*
 * CREATE FUNCTIONS
 */

Glib::RefPtr<VideoFrame> VideoFrame::create(){
    return Glib::RefPtr<VideoFrame>(new VideoFrame());
};

Glib::RefPtr<VideoFrame> VideoFrame::create_from_data(
                uint8_t* data,
                int width,
                int height,
                int rowstride,
                bool copy,
                int64_t timestamp){
    uint8_t* cdata = data;
    if(copy){
        cdata = new uint8_t[3*rowstride*height];
        std::memcpy(cdata, data, 3*rowstride*height);
    }
    VideoFrame* f = new VideoFrame();
    f->init(cdata, width, height, rowstride, timestamp);
    return Glib::RefPtr<VideoFrame>(*f);
};

/*
 * Accessors
 */

int VideoFrame::get_width() const {
    return width;
};

int VideoFrame::get_height() const {
    return height;
};

int VideoFrame::get_rowstride() const {
    return rowstride;
};

int VideoFrame::get_datasize() const {
    return 3*rowstride*height;
};

int64_t VideoFrame::get_timestamp() const {
    return timestamp;
};

const uint8_t* VideoFrame::get_data() const {
    return data;
};

uint8_t* VideoFrame::get_data(){
    return data;
};

bool VideoFrame::is_ok() const{
    return height>0 && width>0 && rowstride>0 && data!=NULL;
};

void VideoFrame::init(uint8_t* _data, int w, int h, int r, int64_t t){
    if(data != NULL){
        delete data;
    }
    data = _data;
    height = h;
    width = w;
    rowstride = r;
    timestamp = t;
};
