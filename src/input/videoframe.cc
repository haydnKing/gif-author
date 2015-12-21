#include "videoframe.h"
#include <cstring> //for memcpy
       
// ------------------- Useful Functions

void ext_lin(const uint8_t* a, 
             const uint8_t* b,
             int distance,
             uint8_t* out)
{
    double R = b[0] + distance * (double(b[0]) - double(a[0])),
           G = b[1] + distance * (double(b[1]) - double(a[1])),
           B = b[2] + distance * (double(b[2]) - double(a[2]));
    out[0] = uint8_t(std::min(std::max(0,R), UINT8_MAX));
    out[1] = uint8_t(std::min(std::max(0,G), UINT8_MAX));
    out[2] = uint8_t(std::min(std::max(0,B), UINT8_MAX));
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

bool VideoFrame::extrapolate(int x, int y, uint8_t*& out, ExtrapolationMode mode){
    //if we're within the image
    if(x>=0 && x < width && y>=0 && y < height)
    {
        out = data+3*(x+rowstride*y);
        return false;
    }
    //otherwise extrapolate
    switch mode
    {
        case EXTRAPOLATION_NONE:
            out = new uint8_t[3];
            std::set(out, 0, 3*sizeof(uint8_t));
            return true;
        case EXTRAPOLATION_CONST:
            x = std::min(std::max(x, 0), width-1);
            y = std::min(std::max(y, 0), height-1);
            out = data+3*(x+rowstride*y);
            return false;
        case EXTRAPOLATION_LINEAR:
            out = new uint8_t[3];
            extrapolate_linear(x,y,out);
            return true;
    }
};

uint8_t* VideoFrame::value_at(double x, double y, uint8_t out, InterpolationMode mode)
{
    if(out == NULL)
    {
        out = new uint8_t[3];
    }
    //Check (x,y) is within the image
    if(x<0 && y < 0 && x >= width && y>= height)
    {
        std::memset(out, 0, 3*sizeof(uint8_t));
        return out;
    }
    //interpolate
    switch(mode){
        case INTERPOLATE_NEAREST:
            interpolate_nearest(x, y, out);
            return;
        case INTERPOLATE_BILINEAR:
            interpolate_bilinear(x, y, out);
            return;
        case INTERPOLATE_CUBIC:
            interpolate_bicubic(x, y, out);
            return;
        case INTERPOLATE_LANCZOS:
            interpolate_lanczos(x, y, out);
            return;
    }
};
    
void VideoFrame::interpolate_nearest(double x, double y, uint8_t* out) const 
{
    int ix = int(x+0.5),
        iy = int(y+0.5);
    //this will round up eveything in range (width-0.5,width) to width
    //which could cause a seg-fault
    if(ix == width) ix--;
    if(iy == height) iy--;
    std::memcpy(out, data+3*(ix+rowstride*iy), 3*sizeof(uint8_t));
};

void VideoFrame::interpolate_bilinear(double x, double y, uint8_t* out) const
{
    int ix = int(x),
        iy = int(y);
    uint8_t *a, *b, *c, *d;
    bool da = extrapolate(ix  , iy  , a),
         db = extrapolate(ix+1, iy  , b),
         dc = extrapolate(ix  , iy+1, c),
         dd = extrapolate(ix+1, iy+1, d);
    double fx = x - ix,
           fy = y - iy;
    //x-axis
    a[0] = fx*a[0] + (1.-fx)*b[0];
    a[1] = fx*a[1] + (1.-fx)*b[1];
    a[2] = fx*a[2] + (1.-fx)*b[2];
    c[0] = fx*c[0] + (1.-fx)*d[0];
    c[1] = fx*c[1] + (1.-fx)*d[1];
    c[2] = fx*c[2] + (1.-fx)*d[2];
    //y-axis
    out[0] = fy*a[0] + (1.-fy)*c[0];
    out[1] = fy*a[1] + (1.-fy)*c[1];
    out[2] = fy*a[2] + (1.-fy)*c[2];
    
    if(da) delete[] a;
    if(db) delete[] b;
    if(dc) delete[] c;
    if(dd) delete[] d;
};

void bicubic_p(double t, 
               const Color& a0, 
               const Color& a1, 
               const Color& a2, 
               const Color& a3,
               double* out);

Color<uint8_t> VideoFrame::interpolate_bicubic(double x, double y) const
{
    int ix = int(x),
        iy = int(y);
    Color<uint8_t> f[16] = {
        value_at(ix-1, iy-1), //0
        value_at(ix  , iy-1), //1
        value_at(ix+1, iy-1), //2
        value_at(ix+2, iy-1), //3

        value_at(ix-1, iy  ), //4
        value_at(ix  , iy  ), //5
        value_at(ix+1, iy  ), //6
        value_at(ix+2, iy  ), //7

        value_at(ix-1, iy+1), //8
        value_at(ix  , iy+1), //9
        value_at(ix+1, iy+1), //10
        value_at(ix+2, iy+1), //11

        value_at(ix-1, iy+2), //12
        value_at(ix  , iy+2), //13
        value_at(ix+1, iy+2), //14
        value_at(ix+2, iy+2), //15
    };

    double b[4*3];
    
    bicubic_p(x-ix, f + 0, f+ 1, f+ 2, f+ 3, b  );
    bicubic_p(x-ix, f + 4, f+ 5, f+ 6, f+ 7, b+3);
    bicubic_p(x-ix, f + 8, f+ 9, f+10, f+11, b+6);
    bicubic_p(x-ix, f +12, f+13, f+14, f+15, b+9);

    double r[3];
    bicubic_p(y-iy, b, b+3, b+6, b+9, r);

    return Color<uint8_t>(uint8_t(std::min(std::max(r[0]+0.5, 0), UINT8_MAX)),
                          uint8_t(std::min(std::max(r[1]+0.5, 0), UINT8_MAX)),
                          uint8_t(std::min(std::max(r[2]+0.5, 0), UINT8_MAX)));
};

Color<uint8_t> VideoFrame::interpolate_lanczos(double x, double y) const;

void VideoFrame::extrapolate_linear(int x, int y, uint8_t* out) const
{
    //if y in image
    if(y>=0 && y < height)
    {
        //extrapolate left
        if(x < 0)
            ext_lin(data+3*(1+rowstride*y),
                    data+3*(0+rowstride*y),
                    -x,
                    out);
        //extrapolate right
        else if(x >= width)
            ext_lin(data+3*(width-2+rowstride*y),
                    data+3*(width-1+rowstride*y),
                    1+x-width,
                    out);
        return;
    }
    //if x in image
    else if(x>=0 && x<width)
    {
        //extrapolate up
        if(y < 0)
            ext_lin(data+3*(x+rowstride),
                    data+3*x,
                    -y,
                    out);
        //extrapolate down
        else if(y >= height)
            ext_lin(data+3*(x+(height-2)*rowstride),
                    data+3*(x+(height-1)*rowstride),
                    1+y-height,
                    out);
        return;
    }
    //if we're still here, niether are in the image, corner.
    double a[3], b[3];
    //left
    else if(x<0)
    {
        //top
        if(y<0)
        {
            ext_lin(offset(1,1), offset(0,1), -x, a); 
            ext_lin(offset(1,0), offset(0,0), -x, b); 
            ext_lin(a, b, -y, out);
        }
        //bottom
        else if(y>=height)
        {
            ext_lin(offset(1,height-2), offset(0,height-2), -x, a); 
            ext_lin(offset(1,height-1), offset(0,height-1), -x, b); 
            ext_lin(a, b, 1+y-height, out);
        }
        return;
    }
    //right
    else if(x>=width)
    {
        //top
        if(y<0)
        {
            ext_lin(offset(width-2,1), offset(width-1,1), -x, a); 
            ext_lin(offset(width-2,0), offset(width-1,0), -x, b);
            ext_lin(a, b, -y, out);
        }
        //bottom
        else if(y>=height)
        {
            ext_lin(offset(width-2,height-2), offset(width-1,height-2), -x, a); 
            ext_lin(offset(width-2,height-1), offset(width-1,height-1), -x, b);
            ext_lin(a, b, 1+y-height, out);
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
