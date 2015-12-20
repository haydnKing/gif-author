#include "videoframe.h"
#include <cstring> //for memcpy
       
// ------------------- Useful Functions

Color<uint8_t> extrapolate_linear(const uint8_t* a, 
                                  const uint8_t* b,
                                  int distance){
    return extrapolate_linear(Color<uint8_t>(a), Color<uint8_t>(b), distance);
};
    
Color<uint8_t> extrapolate_linear(const Color<uint8_t>& a, 
                                  const Color<uint8_t>& b,
                                  int distance){
    double R = b.r() + distance * (double(b.r()) - double(a.r())),
           G = b.g() + distance * (double(b.g()) - double(a.g())),
           B = b.b() + distance * (double(b.b()) - double(a.b()));
    return Color<uint8_t>(uint8_t(std::min(std::max(0,R), UINT8_MAX)),
                          uint8_t(std::min(std::max(0,G), UINT8_MAX)),
                          uint8_t(std::min(std::max(0,B), UINT8_MAX)));
};

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

Color<uint8_t> VideoFrame::value_at(int x, int y, ExtrapolationMode mode){
    //if we're within the image
    if(x>=0 && x < width && y>=0 && y < height){
        return Color<uint8_t>(data+3*(x+rowstride*y));
    };
    //otherwise extrapolate
    switch mode{
        case EXTRAPOLATION_NONE:
            return Color<uint8_t>(0,0,0);
        case EXTRAPOLATION_CONST:
            x = std::min(std::max(x, 0), width-1);
            y = std::min(std::max(y, 0), height-1);
            return Color<uint8_t>(data+3*(x+rowstride*y));
        case EXTRAPOLATION_LINEAR:
            return extrapolate_linear(x,y);
    }
};

Color<uint8_t> VideoFrame::value_at(double x, double y, InterpolationMode mode)
{
    //Check (x,y) is within the image
    if(x<0 && y < 0 && x >= width && y>= height){
        return Color();
    }
    //interpolate
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
    
Color<uint8_t> VideoFrame::interpolate_nearest(double x, double y) const 
{
    int ix = int(x+0.5),
        iy = int(y+0.5);
    //this will round up eveything in range (width-0.5,width) to width
    //which could cause a seg-fault
    if(ix == width) ix--;
    if(iy == height) iy--;
    return Color(data+3*(ix+rowstride*iy));
};

Color<uint8_t> VideoFrame::interpolate_bilinear(double x, double y) const
{
    int ix = int(x),
        iy = int(y);
    Color<uint8_t> a = value_at(ix  , iy  ),
                   b = value_at(ix+1, iy  ),
                   c = value_at(ix  , iy+1),
                   d = value_at(ix+1, iy+1);
    double fx = x - ix,
           fy = y - iy;
    //x-axis
    a.r(fx*a.r() + (1.-fx)*b.r());
    a.g(fx*a.g() + (1.-fx)*b.g());
    a.b(fx*a.b() + (1.-fx)*b.b());
    c.r(fx*c.r() + (1.-fx)*d.r());
    c.g(fx*c.g() + (1.-fx)*d.g());
    c.b(fx*c.b() + (1.-fx)*d.b());
    //y-axis
    a.r(fy*a.r() + (1.-fy)*c.r());
    a.g(fy*a.g() + (1.-fy)*c.g());
    a.b(fy*a.b() + (1.-fy)*c.b());
    
    return a;
};

Color<uint8_t> VideoFrame::interpolate_cubic(double x, double y) const
{
    int ix = int(x),
        iy = int(y);
    Color<uint8_t> f[16] = {
        value_at(ix-1, iy-1),
        value_at(ix  , iy-1),
        value_at(ix+1, iy-1),
        value_at(ix+2, iy-1),
        value_at(ix-1, iy  ),
        value_at(ix  , iy  ),
        value_at(ix+1, iy  ),
        value_at(ix+2, iy  ),
        value_at(ix-1, iy+1),
        value_at(ix  , iy+1),
        value_at(ix+1, iy+1),
        value_at(ix+2, iy+1),
        value_at(ix-1, iy+2),
        value_at(ix  , iy+2),
        value_at(ix+1, iy+2),
        value_at(ix+2, iy+2),
    };
};

Color<uint8_t> VideoFrame::interpolate_lanczos(double x, double y) const;

Color<uint8_t> VideoFrame::extrapolate_linear(int x, int y) const
{
    //if y in image
    if(y>=0 && y < height)
    {
        //extrapolate left
        if(x < 0)
            return extrapolate_linear(data+3*(1+rowstride*y),
                    data+3*(0+rowstride*y),
                    -x);
        //extrapolate right
        if(x >= width)
            return extrapolate_linear(data+3*(width-2+rowstride*y),
                                      data+3*(width-1+rowstride*y),
                                      1+x-width);
    }
    //if x in image
    if(x>=0 && x<width)
    {
        //extrapolate up
        if(y < 0)
            return extrapolate_linear(data+3*(x+rowstride),
                                      data+3*x,
                                      -y);
        //extrapolate down
        if(y >= height)
            return extrapolate_linear(data+3*(x+(height-2)*rowstride),
                                      data+3*(x+(height-1)*rowstride),
                                      1+y-height);
    }
    //if we're still here, niether are in the image, corner.
    //left
    if(x<0)
    {
        //top
        if(y<0)
            return extrapolate_linear(
                    extrapolate_linear(offset(1,1), offset(0,1), -x), 
                    extrapolate_linear(offset(1,0), offset(0,0), -x), 
                    -y);
        //bottom
        if(y>=height)
            return extrapolate_linear(
                    extrapolate_linear(offset(1,height-2), offset(0,height-2), -x), 
                    extrapolate_linear(offset(1,height-1), offset(0,height-1), -x), 
                    1+y-height);
    }
    //right
    if(x>=width)
    {
        //top
        if(y<0)
            return extrapolate_linear(
                    extrapolate_linear(offset(width-2,1), offset(width-1,1), -x), 
                    extrapolate_linear(offset(width-2,0), offset(width-1,0), -x), 
                    -y);
        //bottom
        if(y>=height)
            return extrapolate_linear(
                    extrapolate_linear(offset(width-2,height-2), offset(width-1,height-2), -x), 
                    extrapolate_linear(offset(width-2,height-1), offset(width-1,height-1), -x), 
                    1+y-height);
    }
};

uint8_t* VideoFrame::offset(int x, int y) const
{
    return data+3*(x+rowstride*y);
};

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
