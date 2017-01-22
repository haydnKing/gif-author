#include "ditherer.h"

Ditherer::Ditherer(std::string name, std::string description) :
    OptionGroup(name, description)
{};

pGIFImage Ditherer::dither_image(const pVideoFrame vf,
                                 const pBitset mask,
                                 const GIFColorTable *ct) const {
    //Create the output image
    pGIFImage out(new GIFImage(0, 0, vf->get_width(), vf->get_height()));
    out->set_local_colortable(ct);

    _dither_image(out, vf, mask, ct);

    return out;
};

class FSDither : public Ditherer
{
    public:
        static pDitherer create() {
            return pDitherer(new FSDither());
        };

    protected:
        FSDither() :
            Ditherer("FS", "Classical FloydSteinberg dithering")
        {};
        void _dither_image(pGIFImage out,
                           const pVideoFrame vf,
                           const pBitset mask,
                           const GIFColorTable *ct) const {
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
                        out->set_value(x,y,ct->get_transparent_index());
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
                    index = ct->get_closest(pixel);
                    //set the pixel
                    out->set_value(x, y, index);
                    //calculate the errors. Shift 8 places for accuracy
                    for(i = 0; i < 3; i++)
                        error[i] -= int32_t(ct->get_index(index)[i])*256;

                    
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
                next_row = swap;
                memset(next_row, 0, 3*vf->get_width()*sizeof(int32_t));
            }

            //the best way to cover up mistakes ;)
            delete [] errors;


        }
};

class NoDither : public Ditherer
{
    public:
        static pDitherer create() {
            return pDitherer(new NoDither());
        };

    protected:
        NoDither() :
            Ditherer("none", "Don't dither, just choose the closest colour")
        {};
        void _dither_image(pGIFImage out,
                           const pVideoFrame vf,
                           const pBitset mask,
                           const GIFColorTable *ct) const {
            int x,y,index;
            for(y = 0; y < vf->get_height(); y++)
            {
                for(x = 0; x < vf->get_width(); x++)
                {
                    //transparency
                    if(mask && !mask->get(x,y))
                    {
                        out->set_value(x,y,ct->get_transparent_index());
                        continue;
                    }
                    index = ct->get_closest(vf->get_pixel(x,y));
                    out->set_value(x, y, index);
                }
            }


        }
};



DithererFactory::DithererFactory(pDitherer& value) :
    FactoryOption("ditherer", "The ditherer takes a full colour image and a quantizer and produces a quantized image", value)
{
    add_group(FSDither::create());
    add_group(NoDither::create());
};
        
pOption DithererFactory::create(pDitherer& value)
{
    return pOption(new DithererFactory(value));
};


