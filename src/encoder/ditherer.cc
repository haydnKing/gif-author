#include "ditherer.h"

Ditherer::Ditherer(std::string name, std::string description) :
    OptionGroup(name, description)
{};

pGIFImage Ditherer::dither_image(const pFrame f,
                                 pcGIFColorTable ct) const {
    //autocrop the image
    cv::Rect roi = f->get_bounds();
    pFrame cropped = Frame::create(f, roi);
    pGIFImage out = GIFImage::create(roi.x, roi.y, roi.width, roi.height);
    out->set_local_colortable(ct);
    out->set_delay_time(f->delay());
    out->set_disposal_method(DISPOSAL_METHOD_NONE);

    _dither_image(out, f, ct);

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
                           const pFrame f,
                           pcGIFColorTable ct) const {
            //store 2 rows of RGB errors, set to zero
            int32_t* errors = new int32_t[6*f->cols];
            std::memset(errors, 0, 6*f->cols*sizeof(int32_t));
            //pointers to the errors that can be swapped around
            int32_t *this_row = errors, 
                    *next_row = errors + 3*f->cols, 
                    *swap;
            uint8_t pixel[3], * color;
            int32_t error[3];

            int x, y, index, i;

            uint8_t *row;

            for(y = 0; y < f->rows; y++)
            {
                row = f->ptr(y);
                for(x = 0; x < f->cols; x++)
                {
                    //transparency
                    if(row[4*x+3]==0)
                    {
                        out->set_value(x,y,ct->get_transparent_index());
                        continue;
                    }
                    //get the pixel
                    std::memcpy(pixel, row+4*x, 3*sizeof(uint8_t));
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
                    if(x+1 < f->cols)
                    {
                        for(i=0;i<3;i++)
                            this_row[3*x+3+i] += (7 * error[i]) / 16;
                    }
                    if(y+1 < f->rows)
                    {
                        if(x > 0)
                            for(i=0;i<3;i++)
                                next_row[3*x-3+i] += (3 * error[i]) / 16;
                        for(i=0;i<3;i++)
                            next_row[3*x+i] += (5 * error[i]) / 16;
                        if(x+1 < f->cols)
                            for(i=0;i<3;i++)
                                next_row[3*x+3+i] += error[i] / 16;
                    }
                }
                //moving to the next row
                swap = this_row;
                this_row = next_row;
                next_row = swap;
                memset(next_row, 0, 3*f->cols*sizeof(int32_t));
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
                           const pFrame f,
                           pcGIFColorTable ct) const {
            int x,y,index;
            uint8_t *row;
            for(y = 0; y < f->rows; y++)
            {
                row = f->ptr(y);
                for(x = 0; x < f->cols; x++)
                {
                    //transparency
                    if(row[4*x+3]==0)
                    {
                        out->set_value(x,y,ct->get_transparent_index());
                        continue;
                    }
                    index = ct->get_closest(row+4*x);
                    out->set_value(x, y, index);
                }
            }


        }
};



DithererFactory::DithererFactory(pDitherer& value) :
    FactoryOption("ditherer", "The ditherer takes a full colour image and a quantizer and produces a quantized image", value)
{
    auto def = FSDither::create();
    add_group(def);
    add_group(NoDither::create());
    value = def;
};
        
pOption DithererFactory::create(pDitherer& value)
{
    return pOption(new DithererFactory(value));
};


