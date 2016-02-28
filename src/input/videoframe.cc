#include "videoframe.h"
#include <cstring> //for memcpy
#include <cmath>
#include <limits>
       
// ------------------- Useful Functions

void ext_lin(const uint8_t* a, 
             const uint8_t* b,
             int distance,
             uint8_t* out)
{
    double R = b[0] + distance * (double(b[0]) - double(a[0])),
           G = b[1] + distance * (double(b[1]) - double(a[1])),
           B = b[2] + distance * (double(b[2]) - double(a[2]));
    out[0] = uint8_t(std::min(std::max(0.,R), double(UINT8_MAX)));
    out[1] = uint8_t(std::min(std::max(0.,G), double(UINT8_MAX)));
    out[2] = uint8_t(std::min(std::max(0.,B), double(UINT8_MAX)));
};

Affine2D::Affine2D(){
    A[0] = 0.;
    A[1] = 0.;
    A[2] = 0.;
    A[3] = 0.;
    b[0] = 0.;
    b[1] = 0.;
};

Affine2D::Affine2D(const Affine2D& rhs){
    std::memcpy(&A, rhs.get_A(), 4 * sizeof(double));
    std::memcpy(&b, rhs.get_b(), 2 * sizeof(double));
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
    return Affine2D::Rotation(2*M_PI*a/360., x, y);
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
    ret.A[2] = - rhs.A[2] / det;
    ret.A[3] =   rhs.A[0] / det;
    ret.b[0] = ret.A[0] * rhs.b[0] + ret.A[1] * rhs.b[1];
    ret.b[1] = ret.A[2] * rhs.b[0] + ret.A[3] * rhs.b[1];
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
    return A[2] * x + A[3] * y + b[1];
};

void Affine2D::get(const double& u, const double& v, 
                         double& x,       double& y) const
{
    x = A[0] * u + A[1] * v + b[0];
    y = A[2] * u + A[3] * v + b[1];
};
        
std::string Affine2D::as_string() const
{
    std::stringstream ss;
    ss << "[" << A[0] << ", " << A[1] << "] [x]   [" << b[0] << "]\n" 
       << "[" << A[2] << ", " << A[3] << "] [y] + [" << b[1] << "]";     
    return ss.str();
};


VideoFrame::VideoFrame():
    width(-1),
    height(-1),
    rowstride(-1),
    timestamp(-1),
    frame_parent(NULL)
{
    data = NULL;
};

VideoFrame::~VideoFrame(){
    if(frame_parent != NULL)
        av_frame_free(&frame_parent);
    else if(data != NULL)
        delete [] data;
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
        cdata = new uint8_t[rowstride*height];
        std::memcpy(cdata, data, rowstride*height);
    }
    VideoFrame* f = new VideoFrame();
    f->init(cdata, width, height, rowstride, timestamp, position, data_parent);
    return pVideoFrame(f);
};
        
pVideoFrame VideoFrame::create_from_avframe(AVFrame *fr, int64_t timestamp, int64_t position)
{
    pVideoFrame vf = create_from_data(fr->data[0],
                                      fr->width,
                                      fr->height,
                                      fr->linesize[0],
                                      false,
                                      timestamp,
                                      position);
    vf->frame_parent = fr;
    return vf;
};
        
Glib::RefPtr<Gdk::Pixbuf> VideoFrame::get_pixbuf(){
    return Gdk::Pixbuf::create_from_data(get_data(),
                                         Gdk::COLORSPACE_RGB,
                                         false,
                                         8,
                                         get_width(),
                                         get_height(),
                                         get_rowstride());
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
    return rowstride*height;
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

pVideoFrame VideoFrame::scale_to(int w, int h, InterpolationMethod method) const
{
    std::cout << "scale_to(" << w << ", " << h << ", ...);" << std::endl;
    Affine2D tr = Affine2D::Scale(float(w)/float(get_width()),
                                  float(h)/float(get_height()));
    return transform(tr, method);
};

pVideoFrame VideoFrame::transform(const Affine2D& tr, InterpolationMethod mode) const
{
    int i;
    //get inverse transform
    Affine2D inv = tr.invert();
    //find out where the image corners transform to
    double cx[4];
    double cy[4];
    tr.get(   0.,    0., cx[0], cy[0]);
    tr.get(width,    0., cx[1], cy[1]);
    tr.get(   0., height, cx[2], cy[2]);
    tr.get(width, height, cx[3], cy[3]);
    //find the limits of the image
    double limits[4] = {std::numeric_limits<double>::max(),
                        std::numeric_limits<double>::max(),
                        std::numeric_limits<double>::min(),
                        std::numeric_limits<double>::min()};
    for(i = 0; i < 4; i++){
        if(limits[0] > cx[i]) limits[0] = cx[i];
        if(limits[1] > cy[i]) limits[1] = cy[i];
        if(limits[2] < cx[i]) limits[2] = cx[i];
        if(limits[3] < cy[i]) limits[3] = cy[i];
    }
    int t_width = int(limits[2] - limits[0]+0.5);
    int t_height = int(limits[3] - limits[1]+0.5); 

    //allocate data
    uint8_t *t_data = new uint8_t[3*t_width*t_height];
    std::memset(t_data, 0, 3*t_width*t_height);
    double x,y;
    for(int v=0; v < t_height; v++)
    {
        for(int u=0; u < t_width; u++)
        {
            //find location of (x,y) in the original image
            inv.get(limits[0]+u, limits[1]+v, x, y);
            interpolate(x,y,t_data+3*(u+t_width*v), mode);
        }
    }

    return VideoFrame::create_from_data(t_data, 
                                        t_width, 
                                        t_height, 
                                        3*t_width,
                                        false,
                                        timestamp,
                                        position);
};

void VideoFrame::extrapolate(int x, int y, uint8_t* out, ExtrapolationMethod mode) const
{
    //if we're within the image
    if(x>=0 && x < width && y>=0 && y < height)
    {
        std::memcpy(out, get_pixel(x,y), 3*sizeof(uint8_t));
        return;
    }
    //otherwise extrapolate
    switch(mode)
    {
        case EXTRAPOLATION_NONE:
            std::memset(out, 0, 3*sizeof(uint8_t));
            return;
        case EXTRAPOLATION_CONST:
            x = std::min(std::max(x, 0), width-1);
            y = std::min(std::max(y, 0), height-1);
            std::memcpy(out, get_pixel(x,y), 3*sizeof(uint8_t));
            return;
        case EXTRAPOLATION_LINEAR:
            extrapolate_linear(x,y,out);
            return;
    }
};

void VideoFrame::interpolate(double x, double y, uint8_t* out, InterpolationMethod mode) const
{
    if(out == NULL)
    {
        out = new uint8_t[3];
    }
    //Check (x,y) is within the image
    if(x<0 && y < 0 && x >= width && y>= height)
    {
        std::memset(out, 0, 3*sizeof(uint8_t));
        return;
    }
    //interpolate
    switch(mode){
        case INTERPOLATION_NEAREST:
            interpolate_nearest(x, y, out);
            return;
        case INTERPOLATION_BILINEAR:
            interpolate_bilinear(x, y, out);
            return;
        case INTERPOLATION_BICUBIC:
            interpolate_bicubic(x, y, out);
            return;
       /* case INTERPOLATION_LANCZOS:
            interpolate_lanczos(x, y, out);
            return;*/
    }
};
    
void VideoFrame::interpolate_nearest(double x, double y, uint8_t* out) const 
{
    int ix = int(x+0.5),
        iy = int(y+0.5);
    //this will round up eveything in range (width-0.5,width) to width
    //which could cause a seg-fault
    if(ix >= width) ix = width-1;
    if(iy >= height) iy = height -1;
    std::memcpy(out, get_pixel(ix,iy), 3*sizeof(uint8_t));
};

void VideoFrame::interpolate_bilinear(double x, double y, uint8_t* out) const
{
    int ix = int(x),
        iy = int(y);
    uint8_t a[3], b[3], c[3], d[3];
    double da[3], db[3];
    extrapolate(ix  , iy  , a),
    extrapolate(ix+1, iy  , b),
    extrapolate(ix  , iy+1, c),
    extrapolate(ix+1, iy+1, d);
    double fx = x - ix,
           fy = y - iy;

    //x-axis
    da[0] = (1.-fx)*double(a[0]) + fx*double(b[0]);
    da[1] = (1.-fx)*double(a[1]) + fx*double(b[1]);
    da[2] = (1.-fx)*double(a[2]) + fx*double(b[2]);
    db[0] = (1.-fx)*double(c[0]) + fx*double(d[0]);
    db[1] = (1.-fx)*double(c[1]) + fx*double(d[1]);
    db[2] = (1.-fx)*double(c[2]) + fx*double(d[2]);
    //y-axis
    out[0] = uint8_t(0.5+(1.-fy)*da[0] + fy*db[0]);
    out[1] = uint8_t(0.5+(1.-fy)*da[1] + fy*db[1]);
    out[2] = uint8_t(0.5+(1.-fy)*da[2] + fy*db[2]);
};

void bicubic_p(double t, 
               const uint8_t* a_1, 
               const uint8_t* a0, 
               const uint8_t* a1, 
               const uint8_t* a2,
               uint8_t* out){
    for(int i = 0; i < 2; i++){
        out[i] = uint8_t(std::min(std::max(
              0.5 * (
                    (2.*a0[i]) + 
                    (1.*a_1[i] - 1.*a_1[i]) * t + 
                    (2.*a_1[i] - 5.*a0[i] + 4.*a1[i] - 1.*a2[i]) * t*t +
                    (-1.*a_1[i] + 3.*a0[i] - 3.*a1[i] +1.*a2[i]) * t*t*t),
              0.), double(UINT8_MAX)));
    }
};


void VideoFrame::interpolate_bicubic(double x, double y, uint8_t* out) const
{
    int ix = int(x),
        iy = int(y);
    uint8_t b[4*3];
    uint8_t f[48];

    extrapolate(ix-1, iy-1, f+ 0); //0
    extrapolate(ix  , iy-1, f+ 3); //1
    extrapolate(ix+1, iy-1, f+ 6); //2
    extrapolate(ix+2, iy-1, f+ 9); //3

    extrapolate(ix-1, iy  , f+12); //4
    extrapolate(ix  , iy  , f+15); //5
    extrapolate(ix+1, iy  , f+18); //6
    extrapolate(ix+2, iy  , f+21); //7

    extrapolate(ix-1, iy+1, f+24); //8
    extrapolate(ix  , iy+1, f+27); //9
    extrapolate(ix+1, iy+1, f+30); //10
    extrapolate(ix+2, iy+1, f+33); //11

    extrapolate(ix-1, iy+2, f+36); //12
    extrapolate(ix  , iy+2, f+39); //13
    extrapolate(ix+1, iy+2, f+42); //14
    extrapolate(ix+2, iy+2, f+45); //15

    
    //apply horizontally
    bicubic_p(x-ix, f + 0, f+ 3, f+ 6, f+ 9, b  );
    bicubic_p(x-ix, f +12, f+15, f+18, f+21, b+3);
    bicubic_p(x-ix, f +24, f+27, f+30, f+33, b+6);
    bicubic_p(x-ix, f +36, f+39, f+42, f+45, b+9);

    //apply vertically
    bicubic_p(y-iy, b, b+3, b+6, b+9, out);
};

//uint8_t VideoFrame::interpolate_lanczos(double x, double y, uint8_t* out) const;

void VideoFrame::extrapolate_linear(int x, int y, uint8_t* out) const
{
    //if y in image
    if(y>=0 && y < height)
    {
        //extrapolate left
        if(x < 0)
            ext_lin(get_pixel(1,y),
                    get_pixel(0,y),
                    -x,
                    out);
        //extrapolate right
        else if(x >= width)
            ext_lin(get_pixel(width-2, y),
                    get_pixel(width-1, y),
                    1+x-width,
                    out);
        return;
    }
    //if x in image
    else if(x>=0 && x<width)
    {
        //extrapolate up
        if(y < 0)
            ext_lin(get_pixel(x,1),
                    get_pixel(x,0),
                    -y,
                    out);
        //extrapolate down
        else if(y >= height)
            ext_lin(get_pixel(x, height-2),
                    get_pixel(x, height-1),
                    1+y-height,
                    out);
        return;
    }
    //if we're still here, niether are in the image, corner.
    uint8_t a[3], b[3];
    //left
    if(x<0)
    {
        //top
        if(y<0)
        {
            ext_lin(get_pixel(1,1), get_pixel(0,1), -x, a); 
            ext_lin(get_pixel(1,0), get_pixel(0,0), -x, b); 
            ext_lin(a, b, -y, out);
        }
        //bottom
        else if(y>=height)
        {
            ext_lin(get_pixel(1,height-2), get_pixel(0,height-2), -x, a); 
            ext_lin(get_pixel(1,height-1), get_pixel(0,height-1), -x, b); 
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
            ext_lin(get_pixel(width-2,1), get_pixel(width-1,1), -x, a); 
            ext_lin(get_pixel(width-2,0), get_pixel(width-1,0), -x, b);
            ext_lin(a, b, -y, out);
        }
        //bottom
        else if(y>=height)
        {
            ext_lin(get_pixel(width-2,height-2), get_pixel(width-1,height-2), -x, a); 
            ext_lin(get_pixel(width-2,height-1), get_pixel(width-1,height-1), -x, b);
            ext_lin(a, b, 1+y-height, out);
        }
    }
};

const uint8_t* VideoFrame::get_pixel(int x, int y) const
{
    return data + (3*x + rowstride*y);
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
