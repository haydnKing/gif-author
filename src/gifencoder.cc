#include "gifencoder.h"

ImageBitset::ImageBitset(int width, int height, bool initial):
    w(width),
    h(height)
{
    bitset = new uint8_t[(width*height+7)/8];
    set(initial);
};

ImageBitset::~ImageBitset()
{
    delete [] bitset;
};

bool ImageBitset::get(int x, int y) const
{
    x = x + w * y;
    return bitset[x/8] & (1 << x%8);
};

void ImageBitset::set(int x, int y, bool v)
{
    x = x + w*y;
    if(v)
        bitset[x/8] = bitset[x/8] | (1 << x%8);
    else
        bitset[x/8] = bitset[x/8] & ~(1 << x%8);
};
        
void ImageBitset::set(bool v)
{
    std::memset(bitset, 0xFF*v, (w*h+7)/8);
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
    ImageBitset update(canvas_width, canvas_height);
    pVideoFrame fr, prev_fr;
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    const uint8_t *px, *prev_px;
    int64_t delay, last_delay = 4;

    std::ofstream debug("debug.csv");
    int64_t frame_num = 0, change;
    int hist[256];

    for(it = frames.begin(); it != frames.end(); it++)
    {
        debug << frame_num++;

        fr = *it;
        update.set(true);

        //create a quantizer
        pColorQuantizer cq = ColorQuantizer::get_quantizer(qm);
        cq->set_max_colors(fr->get_height() * fr->get_width());
        std::memset(hist, 0, 256*sizeof(int));
        int maximum = std::sqrt(3*255*255);
        
        for(y = 0; y < fr->get_height(); y++)
        {
            for(x = 0; x < fr->get_width(); x++)
            {
                if(prev_fr)
                {
                    px = fr->get_pixel(x,y);
                    prev_px = prev_fr->get_pixel(x,y);
                    change = std::sqrt(
                                std::pow(int(px[0])-int(prev_px[0]),2) +
                                std::pow(int(px[1])-int(prev_px[1]),2) +
                                std::pow(int(px[2])-int(prev_px[2]),2));
                    hist[(255*(change))/maximum]++;

                    if(change == 0)
                        update.set(x,y,false);
                    else
                        cq->add_color(px);
                }
                else
                    cq->add_color(fr->get_pixel(x,y));
            }
        }
        for(y = 0; y < 255; y++)
            debug << ", " << hist[y];
        debug << "\n";
        cq->build_ct();

        //Dither the image
        pGIFImage img = dither_image(fr, cq);
        //debug_ct(img, cq->get_ct());
        
        //set delay_time
        it++;
        if(it == frames.end())
            delay = last_delay;
        else
            delay = ((*it)->get_timestamp() - fr->get_timestamp())/10;
        last_delay = delay;
        it--;
        img->set_delay_time(delay);
        
        out->push_back(img);

        prev_fr = fr;
    }

    return out;
};



Glib::RefPtr<GIFImage> GIFEncoder::dither_image(pVideoFrame vf,
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

void GIFEncoder::dither_FS(const pVideoFrame vf,
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

void GIFEncoder::dither_none(const pVideoFrame vf,
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
 

