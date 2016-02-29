#include "gifauthor.h"

GIFAuthor::GIFAuthor() :
    out(NULL),
    //dm(DITHER_NONE),
    dm(DITHER_FLOYD_STEINBERG),
    qm(QUANT_MMC),
    out_width(-1),
    out_height(-1)
{};

GIFAuthor::~GIFAuthor() 
{
    delete out;
};


DitherMethod GIFAuthor::get_dm() const
{
    return dm;
};

void GIFAuthor::set_dm(DitherMethod _dm)
{
    dm = _dm;
};

QuantizerMethod GIFAuthor::get_qm() const
{
    return qm;
};

void GIFAuthor::set_dm(QuantizerMethod _qm)
{
    qm = _qm;
};

void GIFAuthor::clear_frames()
{
    frames.clear();
};

void GIFAuthor::add_frame(pVideoFrame f)
{
    frames.push_back(f);
};

const std::vector<pVideoFrame> GIFAuthor::get_frames() const
{
    return frames;
};

int GIFAuthor::count_frames() const
{
    return frames.size();
};

const GIF *GIFAuthor::get_output() const
{
    return out;
};

void debug_ct(pGIFImage &img, const GIFColorTable *ct)
{
    for(int x = 0; x < img->get_width(); x++)
    {
        for(int y = 0; y < img->get_height(); y++)
        {
            if(x+img->get_width()*y >= ct->num_colors())
                break;
            img->set_value(x,y,x+img->get_width()*y);
        }
    }
};

void GIFAuthor::update_output()
{
    //delete the old;
    delete out;
    out = NULL;

    if(frames.size()==0)
        return;
    //preprocess frames here
    //
    //
    //
    //work out size
    if(out_width < 0 && out_height < 0)
    {
        out_width = frames[0]->get_width();
        out_height = frames[0]->get_height();
    }
    else if(out_height < 0)
    {
        float r = float(out_width) / float(frames[0]->get_width());
        out_height = int(0.5+r*float(frames[0]->get_height()));
    }
    else if(out_width < 0)
    {
        float r = float(out_height) / float(frames[0]->get_height());
        out_width = int(0.5+r*float(frames[0]->get_width()));
    }

    out = new GIF(out_width, out_height);
    std::vector<pVideoFrame>::iterator it;
    pVideoFrame fr;
    int x, y;
    for(it = frames.begin(); it < frames.end(); it++)
    {
        fr = *it;
        std::cout << "Scaling..." << std::endl;
        //scale
        fr = fr->scale_to(out_width, out_height);
        std::cout << "done" << std::endl;


        //create a quantizer
        pColorQuantizer cq = ColorQuantizer::get_quantizer(qm);
        cq->set_max_colors(fr->get_height() * fr->get_width());
        for(y = 0; y < fr->get_height(); y++)
        {
            for(x = 0; x < fr->get_width(); x++)
            {
                cq->add_color(fr->get_pixel(x,y));
            }
        }
        cq->build_ct();

        //Dither the image
        pGIFImage img = dither_image(fr, cq);
        debug_ct(img, cq->get_ct());
        //TODO: set delay_time
        out->push_back(img);
    }
};



Glib::RefPtr<GIFImage> GIFAuthor::dither_image(pVideoFrame vf,
                                               pColorQuantizer cq) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(0,
                                 0, 
                                 vf->get_width(), 
                                 vf->get_height(),
                                 0,
                                 false,
                                 cq->get_ct());

    switch(dm)
    {
        case DITHER_FLOYD_STEINBERG:
            dither_FS(vf, ret, cq);
            break;
        case DITHER_NONE:
            dither_none(vf, ret, cq);
            break;
    }

    return Glib::RefPtr<GIFImage>(ret);
};

void GIFAuthor::dither_FS(const pVideoFrame vf,
                          GIFImage* out, 
                          pColorQuantizer cq) const
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

void GIFAuthor::dither_none(const pVideoFrame vf,
                         GIFImage* out, 
                         const pColorQuantizer cq) const
{
    std::cout << "dither_none" << std::endl;
    int x,y,index;
    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            index = cq->map_to_ct(vf->get_pixel(x,y));
            out->set_value(x, y, index);
        }
    }
    
};
