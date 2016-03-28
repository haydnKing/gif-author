#include "gifencoder.h"

//utility to compare pixels
bool px_equal(uint8_t *lhs, uint8_t *rhs)
{
    return (lhs[0]==rhs[0] && lhs[1]==rhs[1] && lhs[2] == rhs[2]);
};


Bitset::Bitset(int _width, int _height, bool initial):
    width(_width),
    height(_height)
{
    data = new uint8_t[(width*height+7)/8];
    clear(initial);
};

Bitset::~Bitset()
{
    delete [] data;
};

pBitset Bitset::create(int _width, int _height, bool initial)
{
    return pBitset(new Bitset(_width, _height, initial));
};

bool Bitset::get(int x, int y) const
{
    x = x+y*width;
    return (data[x/8] & (1<<(x%8)));
};

void Bitset::set(int x, int y, bool s)
{
    x = x+y*width;
    if(s)
        data[x/8] = data[x/8] | (1 << (x%8));
    else
        data[x/8] = data[x/8] & ~(1 << (x%8));
};

void Bitset::clear(bool v)
{
    if(v)
        std::memset(data, 255, (width*height+7)/8);
    else
        std::memset(data, 0, (width*height+7)/8);
};

void Bitset::remove_islands()
{
    bool t;
    int x,y;
    for(y=1; y < height-1; y++)
    {
        //left column, x=0
        if(get(0,y) &&
           !get(0,y-1) &&
           !get(0,y+1) &&
           !get(1,y  ) &&
           !get(1,y-1) &&
           !get(1,y-1))
        {
            set(0,y,false);
        }
        //right column, x=width-1
        if(get(width-1,y) &&
           !get(width-1,y-1) &&
           !get(width-1,y+1) &&
           !get(width-2,y  ) &&
           !get(width-2,y-1) &&
           !get(width-2,y-1))
        {
            set(0,y,false);
        }
        for(x=1; x < width-1; x++)
        {
            //general case
            //if pixel is high and surroundings are low
            if(get(x  ,y  ) &&
               !get(x  ,y+1) &&
               !get(x  ,y-1) &&
               !get(x-1,y  ) &&
               !get(x-1,y+1) &&
               !get(x-1,y-1) &&
               !get(x+1,y  ) &&
               !get(x+1,y+1) &&
               !get(x+1,y-1))
            {
                //set low
                set(x,y,false);
            }
        }
    }
    for(x=1; x < width-1; x++)
    {
        //top row, y=0
        if(get(x,0) &&
           !get(x-1,0) &&
           !get(x+1,0) &&
           !get(x-1,1) &&
           !get(x  ,1) &&
           !get(x+1,1))
        {
            set(x,0,false);
        }
        //bottom row, y=height-1
        if(get(x,height-1) &&
           !get(x-1,height-1) &&
           !get(x+1,height-1) &&
           !get(x-1,height-2) &&
           !get(x  ,height-2) &&
           !get(x+1,height-2))
        {
            set(x,height-1,false);
        }
    }
    //x==0, y==0
    if(get(0,0) &&
       !get(1,0) &&
       !get(1,1) &&
       !get(0,1))
    {
        set(0,0,false);
    }
    //x==width-1, y==0
    if(get(width-1,0) &&
       !get(width-1,1) &&
       !get(width-2,0) &&
       !get(width-2,1))
    {
        set(width-1,0,false);
    }
    //x==0, y==height-1
    if(get(0,height-1) &&
       !get(1,height-1) &&
       !get(1,height-2) &&
       !get(0,height-2))
    {
        set(0,height-1,false);
    }
    //x==width-1, y==height-1
    if(get(width-1,height-1) &&
       !get(width-2,height-1) &&
       !get(width-2,height-2) &&
       !get(width-1,height-2))
    {
        set(width-1,height-1,false);
    }
};


GIFEncoder::GIFEncoder(int cw, int ch, QuantizerMethod _qm, DitherMethod _dm):
    canvas_width(cw),
    canvas_height(ch),
    qm(_qm),
    dm(_dm)
{};

GIFEncoder::~GIFEncoder() {};

void GIFEncoder::push_frame(pVideoFrame fr)
{
    frames.push_back(fr);
};

GIF *GIFEncoder::get_output()
{
    /*
    dbg_save_POI(220, 105, "elbow");
    dbg_save_POI(170, 88, "light");
    dbg_save_POI(104, 97, "wall1");
    dbg_save_POI(205, 95, "wall2");
    dbg_save_POI(272,166, "hip");
    */
    dbg_save_POI(217,96, "hood");

    GIF *out = new GIF(canvas_width, canvas_height);
    pVideoFrame fr;
    pBitset fr_mask;
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    const uint8_t *px;
    int64_t last_timestamp, delay = 4;
    int64_t frame_no = 0;

    std::vector<pBitset> masks = detect_bg();
    //in case I want to re-introduce smoothing
    std::vector<pVideoFrame> sframes = frames;
    std::cout << "Background detection done" << std::endl;

    for(int i = 0; i < sframes.size(); i++)
    {
        std::cout << "Frame " << i << " of " << frames.size() << std::endl;
        fr = sframes[i];
        fr_mask = masks[i];

        //create a quantizer
        pColorQuantizer cq = ColorQuantizer::get_quantizer(qm);
        cq->set_max_colors(fr->get_height() * fr->get_width());
        
        if(fr_mask)
        {
            for(y = 0; y < fr->get_height(); y++)
                for(x = 0; x < fr->get_width(); x++)
                    if(fr_mask->get(x,y))
                        cq->add_color(fr->get_pixel(x,y));
        }
        else
        {
            for(y = 0; y < fr->get_height(); y++)
                for(x = 0; x < fr->get_width(); x++)
                    cq->add_color(fr->get_pixel(x,y));
        }

        if(cq->get_num_colors() > 0)
        {
            //Dither the image
            pGIFImage img = create_gif_image(0,0,fr->get_width(),fr->get_height());
            dither_image(img, fr, fr_mask, cq, 255);
            //debug_ct(img, cq->get_ct());
            
            //set delay_time
            if(i > 0)
            {
                delay = (frames[i]->get_timestamp() - last_timestamp)/10;
                out->back()->set_delay_time(delay);
            }
            last_timestamp = frames[i]->get_timestamp();
            
            out->push_back(img);
        }
    }
    //set delay for last frame
    out->back()->set_delay_time(delay);

    return out;
};

void GIFEncoder::dither_image(pGIFImage out,
                              const pVideoFrame vf,
                              const pBitset mask,
                              const pColorQuantizer cq,
                              uint8_t colors) const
{
    if(mask)
    {
        out->set_transparency(true);
        out->set_transparent_index(colors);
        out->set_disposal_method(DISPOSAL_METHOD_NONE);
        //out->set_disposal_method(DISPOSAL_METHOD_RESTORE_BACKGROUND);
        cq->build_ct(colors-1);
    }
    else
        cq->build_ct(colors);
    out->set_local_colortable(cq->get_ct());
    switch(dm)
    {
        case DITHER_FLOYD_STEINBERG:
            dither_FS(vf, mask, out, cq);
            break;
        case DITHER_NONE:
            dither_none(vf, mask, out, cq);
            break;
    }
};

void GIFEncoder::dither_FS(const pVideoFrame vf,
                           const pBitset mask,
                           pGIFImage out, 
                           const pColorQuantizer cq) const
{
    std::cout << "dither_fs" << std::endl;
    //store 2 rows of RGB errors, set to zero
    int32_t* errors = new int32_t[6*vf->get_width()];
    std::memset(errors, 0, 6*vf->get_width()*sizeof(int32_t));
    //pointers to the errors that can be swapped around
    int32_t *this_row = errors, 
            *next_row = errors + 3*vf->get_width(), 
            *swap;
    uint8_t pixel[3], * color;
    int32_t error[3];

    int x, y, index, i;

    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //transparency
            if(mask && !mask->get(x,y))
            {
                out->set_value(x,y,out->transparent_index());
                continue;
            }
            //get the pixel
            std::memcpy(pixel, vf->get_pixel(x,y), 3*sizeof(uint8_t));
            //add the errors, being wary of overflow
            for(i = 0; i < 3; i++)
            {
                error[i] = this_row[3*x+i] + int32_t(pixel[i])*256;
                pixel[i] = (uint8_t)std::max(0, std::min(UINT8_MAX, (error[i]+128)/256));
            }

            //get the closest ct
            index = cq->map_to_ct(pixel);
            //set the pixel
            out->set_value(x, y, index);
            //calculate the errors. Shift 8 places for accuracy
            for(i = 0; i < 3; i++)
                error[i] -= int32_t(cq->get_ct()->get_index(index)[i])*256;

            
            //propagate the errors
            if(x+1 < vf->get_width())
            {
                for(i=0;i<3;i++)
                    this_row[3*x+3+i] += (7 * error[i]) / 16;
            }
            if(y+1 < vf->get_height())
            {
                if(x > 0)
                    for(i=0;i<3;i++)
                        next_row[3*x-3+i] += (3 * error[i]) / 16;
                for(i=0;i<3;i++)
                    next_row[3*x+i] += (5 * error[i]) / 16;
                if(x+1 < vf->get_width())
                    for(i=0;i<3;i++)
                        next_row[3*x+3+i] += error[i] / 16;
            }
        }
        //moving to the next row
        swap = this_row;
        this_row = next_row;
        next_row = this_row;
        memset(next_row, 0, 3*vf->get_width()*sizeof(int32_t));
    }

    //the best way to cover up mistakes ;)
    delete [] errors;
};

void GIFEncoder::dither_none(const pVideoFrame vf,
                             const pBitset mask,
                             pGIFImage out, 
                             const pColorQuantizer cq) const
{
    std::cout << "dither_none" << std::endl;
    int x,y,index;
    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //transparency
            if(mask && !mask->get(x,y))
            {
                out->set_value(x,y,out->transparent_index());
                continue;
            }
            index = cq->map_to_ct(vf->get_pixel(x,y));
            out->set_value(x, y, index);
        }
    }
    
};
        
void GIFEncoder::dbg_save_POI(int x, int y, const char* name) const
{
    std::stringstream ss;
    ss << "./POI/POI_" << x << "_" << y << "_" << name << ".csv";
    std::ofstream f(ss.str().c_str());
    f << "r, g, b" << std::endl;
    const uint8_t* px;

    for(int i = 0; i < frames.size(); i++)
    {
        px = frames[i]->get_pixel(x,y);
        f << int(px[0]) << ", " << int(px[1]) << ", " << int(px[2]) << std::endl;
    }
};
        
void GIFEncoder::dbg_thresholding(int len, uint8_t *px, float *fpx, const char *name) const
{
    std::stringstream ss;
    ss << "./POI/thresh_" << name << ".csv";
    std::ofstream f(ss.str().c_str());
    f << "r, g, b, r, g, b" << std::endl;

    for(int i = 0; i < len; i++)
    {
        f << int(px[3*i+0]) << ", "
          << int(px[3*i+1]) << ", "
          << int(px[3*i+2]) << ", "
          << fpx[3*i+0] << ", "
          << fpx[3*i+1] << ", "
          << fpx[3*i+2] << std::endl;
    }
};

std::vector<pBitset> GIFEncoder::detect_bg(float alpha, float alpha_max, float beta, float sig_t, float sig_s) const
{
    std::cout << "GIFEncoder::detect_bg(...)" << std::endl;

    std::vector<pBitset> ret;
    ret.reserve(frames.size());
    int scene_start = 0;
    float d;
    for(int i = 1; i < frames.size();i++)
    {
        d = get_fraction_above(frames[i], frames[i-1], beta);
        if(d > beta)
        {
            std::cout << "frame[" << i << "] = " << d << std::endl;
            std::vector<pVideoFrame> scene(frames.begin()+scene_start, frames.begin()+i);
            std::vector<pBitset> temp = threshold(scene, alpha, alpha_max, sig_t, sig_s);
            ret.insert(ret.end(), temp.begin(), temp.end());
            scene_start = i;
        }
    }
    //last scene
    std::vector<pVideoFrame> scene(frames.begin()+scene_start, frames.end());
    std::vector<pBitset> temp = threshold(scene, alpha, alpha_max, sig_t, sig_s);
    ret.insert(ret.end(), temp.begin(), temp.end());

    for(int i = 0; i < ret.size(); i++)
        ret[i]->remove_islands();
    return ret;
};


std::vector<pBitset> GIFEncoder::threshold(std::vector<pVideoFrame> segment, float alpha, float alpha_max, float sig_t, float sig_s) const
{
    int i, j,k, y, x;

    uint8_t *pixels = new uint8_t[3*segment.size()], *last, max_d;
    float  *fpixels = new float[3*segment.size()], *flast;

    int kernel_length =int(6*sig_t) + 1;
    int kernel_center = kernel_length/2;
    float kernel[kernel_length], norm;
    for(j=0; j < kernel_length; j++)
    {
        kernel[j] = std::exp(-(j-kernel_center)*(j-kernel_center)/(2*sig_t*sig_t));
    }

    //prepare output images
    std::vector<pBitset> ret;
    for(i=0; i < segment.size(); i++)
        ret.push_back(Bitset::create(segment[i]->get_width(), segment[i]->get_height(), false));

    //blur input images
    std::vector<pVideoFrame> blurred;
    for(i=0; i < segment.size(); i++)
        blurred.push_back(segment[i]->blur(sig_s));

    unsigned long c = 0;
    //pixel by pixel
    for(y = 0; y < segment[0]->get_height(); y++)
    {
        for(x = 0; x < segment[0]->get_width(); x++)
        {
            //get the pixels
            for(i = 0; i < segment.size(); i++)
            {
                std::memcpy(pixels+3*i, blurred[i]->get_pixel(x,y), 3*sizeof(uint8_t));
            }

            //smooth
            for(i=0; i<segment.size(); i++)
            {
                //find the max delta
                max_d = 0;
                if(i > 0)
                    for(j=0; j<3; j++)
                        if(std::abs(pixels[3*i+j]-pixels[3*i+j-3]) > max_d)
                            max_d = std::abs(pixels[3*i+j]-pixels[3*i+j-3]);
                if(i < segment.size()-1)
                    for(j=0; j<3; j++)
                        if(std::abs(pixels[3*i+j]-pixels[3*i+j+3]) > max_d)
                            max_d = std::abs(pixels[3*i+j]-pixels[3*i+j+3]);

                //only smooth if max delta is below alpha_max. This prevents
                //sudden large jumps from being lost
                if(max_d < alpha_max)
                {

                    for(j=0; j<3; j++)
                    {
                        fpixels[3*i+j] = 0;
                        norm = 0;
                        for(k=0; k < kernel_length; k++)
                        {
                            if((i+k-kernel_center > 0) &&
                               (i+k-kernel_center < segment.size()))
                            {
                                fpixels[3*i+j] += kernel[k] * float(pixels[3*(i+k-kernel_center)+j]);
                                norm += kernel[k];
                            }
                        }
                        fpixels[3*i+j] /= norm;
                    }
                }
                else
                {
                    for(j=0; j < 3; j++)
                    {
                        fpixels[3*i+j] = float(pixels[3*i+j]);
                    }
                }
            }

            if(x==308 && y==157) dbg_thresholding(segment.size(), pixels, fpixels, "hood");

            //decide whether to update
            int run_start = 0;
            ret[0]->set(x,y,true);
            for(i=1; i < ret.size(); i++)
            {
                for(j=0; j < 3; j++)
                {
                    if(std::abs(fpixels[3*i+j] - fpixels[3*run_start+j]) > alpha)
                    {
                        run_start = i;
                        ret[i]->set(x,y,true);
                    }
                }
            }
        }
    }

    return ret;
};
        
float GIFEncoder::get_fraction_above(const pVideoFrame lhs, const pVideoFrame rhs, float beta) const
{
    unsigned int c = 0;
    const uint8_t *l_px, *r_px;
    for(int y = 0; y < lhs->get_height(); y++)
    {
        for(int x = 0; x < lhs->get_width(); x++)
        {
            l_px = lhs->get_pixel(x,y);
            r_px = rhs->get_pixel(x,y);
            if(std::abs(l_px[0] - r_px[0]) > beta ||
               std::abs(l_px[1] - r_px[1]) > beta ||
               std::abs(l_px[2] - r_px[2]) > beta)
            {
                c++;
            }
            
        }
    }
    return float(c) / float(lhs->get_width()*lhs->get_height());
};

pGIFImage GIFEncoder::create_gif_image(int left, int top, int width, int height) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(left,
                                 top, 
                                 width, 
                                 height);

    return Glib::RefPtr<GIFImage>(ret);
};
 

