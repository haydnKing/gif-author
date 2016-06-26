#include "gifencoder.h"

//utility to compare pixels
bool px_equal(uint8_t *lhs, uint8_t *rhs)
{
    return (lhs[0]==rhs[0] && lhs[1]==rhs[1] && lhs[2] == rhs[2]);
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
    GIF *out = new GIF(canvas_width, canvas_height);
    pVideoFrame fr;
    pBitset fr_mask;
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    const uint8_t *px;
    int64_t last_timestamp, delay = 4;
    int64_t frame_no = 0;

    std::vector<pBitset> masks;
    std::vector<pVideoFrame> sframes;
    Segmenter *sm = segmenterFactory.get_chosen_item();
    sm->segment(frames, sframes, masks);

    std::cout << "Background detection done" << std::endl;
    std::cout << "frames: " << frames.size() << std::endl;
    std::cout << "sframes: " << sframes.size() << std::endl;
    std::cout << "masks: " << masks.size() << std::endl;

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

pGIFImage GIFEncoder::create_gif_image(int left, int top, int width, int height) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(left,
                                 top, 
                                 width, 
                                 height);

    return Glib::RefPtr<GIFImage>(ret);
};
 

