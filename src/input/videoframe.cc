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

Affine2D Affine2D::Scale(double x_ratio, double y_ratio)
{
    Affine2D ret;
    ret.A[0] = x_ratio;
    ret.A[3] = x_ratio;
    return ret;
};

Affine2D Affine2D::Transform(double x, double y)
{
    Affine2D ret;
    ret.b[0] = x;
    ret.b[1] = y;
    return ret;
};

Affine2D Affine2D::Rotation(double a, double x, double y)
{
    Affine2D ret;
    ret.A[0] = std::cos(a);
    ret.A[1] = -std::sin(a);
    ret.A[2] = std::sin(a);
    ret.A[3] = std::cos(a);
    ret.b[0] = x * (std::cos(a)-1) - y*std::sin(a);
    ret.b[1] = x * std::sin(a) + y * (std::cos(a)-1);
    return ret;
};


Affine2D Affine2D::RotationDegrees(double a, double x, double y)
{
    return Affine2D::Rotaion(2*std::PI*a/360.);
};


Affine2D Affine2D::Product(const Affine2D& left, const Affine2D& right)
{
    Affine2D ret;
    ret.A[0] = left.A[0] * right.A[0] + left.A[1] * right.A[2];
    ret.A[1] = left.A[0] * right.A[1] + left.A[1] * right.A[3];
    ret.A[0] = left.A[2] * right.A[0] + left.A[3] * right.A[2];
    ret.A[1] = left.A[2] * right.A[1] + left.A[3] * right.A[3];
    ret.b[0] = left.A[0] * right.b[0] + left.A[1] * right.b[1] + left.b[0];
    ret.b[1] = left.A[2] * right.b[0] + left.A[3] * right.b[1] + left.b[1];
    return ret;
};
        
Affine2D Affine2D::Invert(const Affine2D& rhs)
{
    Affine2D ret;
    double det = rhs.A[0] * rhs.A[3] - rhs.A[1] * rhs.A[2];
    ret.A[0] =   rhs.A[3] / det;
    ret.A[1] = - rhs.A[1] / det;
    ret.A[2] =   rhs.A[2] / det;
    ret.A[3] = - rhs.A[0] / det;
    ret.b[0] = ret.A[0] * rhs.b[0] + ret.b[1] * ret.b[1];
    ret.b[1] = ret.A[2] * rhs.b[0] + ret.b[3] * ret.b[1];
    return ret;
}


Affine2D Affine2D::operator*(const Affine2D& rhs) const
{
    return Affine2D::Product(*this, rhs);
};

Affine2D Affine2D::invert() const
{
    return Affine2D::Invert(*this);    
};

double Affine2D::get_x(const double& x, const double& y) const
{
    return A[0] * x + A[1] * y + b[0];
};

double Affine2D::get_y(const double& x, const double& y) const
{
    return A[2] * x + A[3] * y + b[0];
};

void Affine2D::get(const double& u, const double& v, 
                         double& x,       double& y) const
{
    x = A[0] * u + A[1] * v + b[0];
    y = A[2] * u + A[3] * v + b[1];
};


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
                int64_t position,
                pVideoFrame data_parent)
{
    uint8_t* cdata = data;
    if(copy){
        cdata = new uint8_t[3*rowstride*height];
        std::memcpy(cdata, data, 3*rowstride*height);
    }
    VideoFrame* f = new VideoFrame();
    f->init(cdata, width, height, rowstride, timestamp, position, data_parent);
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

pVideoFrame VideoFrame::crop(int left, int top, int width, int height)
{
    return VideoFrame::create_from_data(
            data+3*(left+width*height),
            width,
            height,
            rowstride,
            false,
            timestamp,
            position,
            pVideoFrame(this));
};

pVideoFrame VideoFrame::transform(const Affine2D& tr, InterpolationMode mode){
    int i;
    //get inverse transform
    Affine2D inv = tr.invert();
    //find out where the image corners transform to
    double cx[4];
    double cy[4];
    tr.get(    0.,    0., cx[0], cy[0]);
    tr.get(    0., width, cx[1], cy[1]);
    tr.get(height,    0., cx[2], cy[2]);
    tr.get(height, width, cx[3], cy[3]);
    //find the limits of the image
    double limits[4] = {std::numeric_limits<double>::max,
                        std::numeric_limits<double>::max,
                        std::numeric_limits<double>::min,
                        std::numeric_limits<double>::min};
    for(i = 0; i < 4; i++){
        if(limits[0] > x[i]) limits[0] = x[i];
        if(limits[1] > y[i]) limits[1] = y[i];
        if(limits[2] < x[i]) limits[2] = x[i];
        if(limits[3] < y[i]) limits[3] = y[i];
    }
    int _width = int(limits[2] - limits[0]);
    int _height = int(limits[3] - limits[1]); 

    //allocate data
    uint8_t *_data = new uint8_t[3*_width*_height];
    double x,y;
    for(int u=0; u < _width; u++)
    {
        for(int v=0; v < _width; v++)
        {
            //find location of (x,y) in the original image
            inv.get(limits[0]+u, limits[1]+v, x, y);
            interpolate(x,y,_data+3*(x+_width*y), mode);
        }
    }

    return VideoFrame::create_from_data(_data, 
                                        _width, 
                                        _height, 
                                        _width,
                                        false,
                                        timestamp,
                                        position);
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
               const uint8_t* a_1, 
               const uint8_t* a0, 
               const uint8_t* a1, 
               const uint8_t* a2,
               double* out){
    for(int i = 0; i < 2; i++){
        out[i] = 0.5 * (
                    (2.* a0[i]) + 
                    (1.*a_1[i] - 1.*a_1[i]) * t + 
                    (2.*a_1[i] - 5.*a0[i] + 4.*a1[i] - 1.*a2[i]) * t*t +
                    (-1.*a_1[i] + 3.*a0[i] - 3.*a1[i] +1.*a2[i]) * t*t*t);
    }
};


void VideoFrame::interpolate_bicubic(double x, double y, uint8_t* out) const
{
    int ix = int(x),
        iy = int(y);
    uint8_t* f = new uint8_t[48];
    value_at(ix-1, iy-1, f+ 0); //0
    value_at(ix  , iy-1, f+ 3); //1
    value_at(ix+1, iy-1, f+ 6); //2
    value_at(ix+2, iy-1, f+ 9); //3

    value_at(ix-1, iy  , f+12); //4
    value_at(ix  , iy  , f+15); //5
    value_at(ix+1, iy  , f+18); //6
    value_at(ix+2, iy  , f+21); //7

    value_at(ix-1, iy+1, f+24); //8
    value_at(ix  , iy+1, f+27); //9
    value_at(ix+1, iy+1, f+30); //10
    value_at(ix+2, iy+1, f+33); //11

    value_at(ix-1, iy+2, f+36); //12
    value_at(ix  , iy+2, f+39); //13
    value_at(ix+1, iy+2, f+42); //14
    value_at(ix+2, iy+2, f+45); //15

    double b[4*3];
    
    //apply horizontally
    bicubic_p(x-ix, f + 0, f+ 3, f+ 6, f+ 9, b  );
    bicubic_p(x-ix, f +12, f+15, f+18, f+21, b+3);
    bicubic_p(x-ix, f +24, f+27, f+30, f+33, b+6);
    bicubic_p(x-ix, f +36, f+39, f+42, f+45, b+9);

    //apply vertically
    double r[3];
    bicubic_p(y-iy, b, b+3, b+6, b+9, r);

    out[0] = uint8_t(std::min(std::max(r[0]+0.5, 0), UINT8_MAX));
    out[1] = uint8_t(std::min(std::max(r[1]+0.5, 0), UINT8_MAX));
    out[2] = uint8_t(std::min(std::max(r[2]+0.5, 0), UINT8_MAX));

    delete [] f;
};

//uint8_t VideoFrame::interpolate_lanczos(double x, double y, uint8_t* out) const;

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

void VideoFrame::init(uint8_t* _data, int w, int h, int r, int64_t t, int64_t p, pVideoFrame dp){
    if(data != NULL){
        delete data;
    }
    data = _data;
    height = h;
    width = w;
    rowstride = r;
    timestamp = t;
    position = p;
    data_parent = dp;
};
