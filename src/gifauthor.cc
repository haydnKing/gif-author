#include "gifauthor.h"

GAFrame::GAFrame(pVideoFrame& _pVF, int _delay_t) 
{
};

GAFrame::~GAFrame()
{
};

std::vector<pGIFImage> GAFrame::process(int w, int h, InterpolationMethod im)
{
};

bool GAFrame::get_color_table(GIFColorTable *& ct, pVideoFrame scaled_vf) const
{
};

Glib::RefPtr<GIFImage> GAFrame::dither_image(GIFColorTable& ct, pVideoFrame scaled_vf) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(0,
                                 0, 
                                 scaled_vf->get_width(), 
                                 scaled_vf->get_height(),
                                 delay_t,
                                 false,
                                 &ct);

    switch(dither_method)
    {
        case DITHER_FLOYD_STEINBERG:
            dither_FS(ret, ct, scaled_vf);
            break;
        case DITHER_NONE:
            dither_none(ret, ct, scaled_vf);
            break;
    }

    return Glib::RefPtr<GIFImage>(ret);
};

void GAFrame::dither_FS(GIFImage* out, 
                        GIFColorTable& ct, 
                        pVideoFrame vf) const
{
    //store 2 rows of RGB errors, set to zero
    int16_t* errors = new int16_t[6*vf->get_width()];
    std::memset(errors, 6*vf->get_width()*sizeof(int16_t));
    int16_t* this_row = errors, 
             next_row = error + 3*vf->get_width(), 
             swap;
    uint8_t* pixel, * color;
    int16_t error[3];

    int x, y, index, i;

    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //get the pixel
            pixel = vf->get_pixel(x,y);
            //add the errors, being wary of overflow
            for(i = 0; i < 3; i++)
                pixel[i] = (uint8_t) std::min(UINT8_MAX,
                        std::max(0,this_row[3*x+i]>>4 + uint16_t(pixel[i])));

            //get the closest ct
            index = ct.get_closest_index(pixel);
            //calculate the errors. Shift 4 places so that we're accurate to 1/16 of a colour gradation
            for(i = 0; i < 3; i++)
                error[i] = (int16_t(ct[index][i]) - int16_t(pixel[i])) << 4;
            //set the pixel
            memcpy(pixel, ct[index], 3*sizeof(uint8_t));
            out->set_value(x, y, index);

            //propagate the errors
            if(x+1 < vf->get_width())
            {
                for(i=0;i<3;i++)
                    this_row[3*x+3+i] += 7 * error[i] / 16;
            }
            if(y+1 < vf->get_height())
            {
                if(x > 0)
                    for(i=0;i<3;i++)
                        next_row[3*x-3+i] += 3 * error[i] / 16;
                for(i=0;i<3;i++)
                    next_row[3*x+i] += 5 * error[i] / 16;
                if(x+1 < vf->get_width())
                    for(i=0;i<3;i++)
                        next_row[3*x+3+i] += error[i] / 16;
            }
        }
        //moving to the next row
        swap = this_row;
        this_row = next_row;
        next_row = this_row;
        memset(0, next_row, 3*vf->get_width()*sizeof(int16_t));
    }

    //the best way to cover up mistakes ;)
    delete [] errors;
};

void GAFrame::dither_none(GIFImage* out, 
                          GIFColorTable& ct, 
                          pVideoFrame vf) const
{
    int x,y,index;
    uint8_t* px;
    for(y = 0; y < vf->get_width(); y++)
    {
        for(x = 0; x < vf->get_height(); x++)
        {
            px = vf->get_pixel(x,y);
            index = ct.get_closest_index(px);
            out->set_value(x, y, index);
            std::memcpy(px, ct[index], 3*sizeof(uint8_t));
        }
    }
};
