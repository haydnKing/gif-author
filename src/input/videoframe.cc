#include "videoframe.h"
#include <cstring> //for memcpy
        
Affine2D::Affine2D(){
    std::memset(&A, 0, 4 * sizeof(double));
    std::memset(&b, 0, 2 * sizeof(double));
};

Affine2D::Affine2D(const Affine2D& rhs){
    std::memcpy(&A, rhs.get_A(), 4 * sizeof(double));
    std::memcpy(&b, rhs.get_B(), 2 * sizeof(double));
};

Affine2D Affine2D::I(){
    Affine2D ret;
    ret.A[0] = 1.;
    ret.A[3] = 1.;
    return ret;
};

Affine2D Affine2D::Scale(double x_ratio, double y_ratio);

    

VideoFrame::VideoFrame():
    width(-1),
    height(-1),
    rowstride(-1),
    timestamp(-1)
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

pVideoFrame VideoFrame::create(){
    return pVideoFrame(new VideoFrame());
};

pVideoFrame VideoFrame::create_from_data(
                uint8_t* data,
                int width,
                int height,
                int rowstride,
                bool copy,
                int64_t timestamp,
                int64_t position)
{
    uint8_t* cdata = data;
    if(copy){
        cdata = new uint8_t[3*rowstride*height];
        std::memcpy(cdata, data, 3*rowstride*height);
    }
    VideoFrame* f = new VideoFrame();
    f->init(cdata, width, height, rowstride, timestamp, position);
    return pVideoFrame(f);
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

int64_t VideoFrame::get_position() const {
    return position;
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

uint8_t* value_at(double x, double y, InterpolationMode mode){
    //check bounds
    if(x<0. || y<0. || x>=width || y>=height){
        return NULL;
    }
    //check if both x and y are whole numbers
    if( x-int(x) ==0 && y - int(y) == 0 ){
        int p = 3*(int(x)+rowstride*int(y));
        return Color(data[p], data[p+1], data[p+2]);
    }
    //otherwise, interpolate
    switch(mode){
        case INTERPOLATE_NEAREST:
            return interpolate_nearest(x,y);
        case INTERPOLATE_BILINEAR:
            return interpolate_bilinear(x,y);
        case INTERPOLATE_CUBIC:
            return interpolate_cubic(x,y);
        case INTERPOLATE_LANCZOS:
            return interpolate_lanczos(x,y);
    }
};
    
Color VideoFrame::interpolate_nearest(double x, double y) const;
Color VideoFrame::interpolate_bilinear(double x, double y) const;
Color VideoFrame::interpolate_cubic(double x, double y) const;
Color VideoFrame::interpolate_lanczos(double x, double y) const;

void VideoFrame::init(uint8_t* _data, int w, int h, int r, int64_t t, int64_t p){
    if(data != NULL){
        delete data;
    }
    data = _data;
    height = h;
    width = w;
    rowstride = r;
    timestamp = t;
    position = p;
};
