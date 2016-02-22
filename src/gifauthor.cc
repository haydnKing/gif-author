#include "gifauthor.h"

GIFAuthor::GIFAuthor() :
    out(NULL),
    dm(DITHER_NONE),
    qm(QUANT_MMC)
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

    out = new GIF(frames[0]->get_width(),
                  frames[0]->get_height());
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    for(it = frames.begin(); it < frames.end(); it++)
    {
        std::cout << "get_quantizer" << std::endl;
        std::cout << "frame " << (*it)->get_width() << "x"
                              << (*it)->get_height() << " rs "
                              << (*it)->get_rowstride() << std::endl;
        //create a quantizer
        pColorQuantizer cq = ColorQuantizer::get_quantizer(qm);
        cq->set_max_colors((*it)->get_height() * (*it)->get_width());
        std::cout << "adding colours" << std::endl;
        for(y = 0; y < (*it)->get_height(); y++)
        {
            for(x = 0; x < (*it)->get_width(); x++)
            {
                cq->add_color((*it)->get_pixel(x,y));
            }
        }
        std::cout << "build_ct" << std::endl;
        cq->build_ct();

        //Dither the image
        std::cout << "dither image" << std::endl;
        pGIFImage img = dither_image(*it, cq);
        //TODO: set delay_time
        std::cout << "done with image" << std::endl;
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
    //store 2 rows of RGB errors, set to zero
    int16_t* errors = new int16_t[6*vf->get_width()];
    std::memset(errors, 0, 6*vf->get_width()*sizeof(int16_t));
    int16_t *this_row = errors, 
            *next_row = errors + 3*vf->get_width(), 
            *swap;
    uint8_t pixel[3], * color;
    int16_t error[3];

    int x, y, index, i;

    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //get the pixel
            std::memcpy(pixel, vf->get_pixel(x,y), 3*sizeof(uint8_t));
            //add the errors, being wary of overflow
            for(i = 0; i < 3; i++)
                pixel[i] = (uint8_t) std::min(UINT8_MAX,
                        std::max(0,this_row[3*x+i]>>4 + uint16_t(pixel[i])));

            //get the closest ct
            index = cq->map_to_ct(pixel);
            //calculate the errors. Shift 4 places so that we're accurate to 1/16 of a colour gradation
            for(i = 0; i < 3; i++)
                error[i] = (int16_t(cq->get_ct()->get_index(index)[i]) - int16_t(pixel[i])) << 4;
            //set the pixel
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
        memset(next_row, 0, 3*vf->get_width()*sizeof(int16_t));
    }

    //the best way to cover up mistakes ;)
    delete [] errors;
};

void GIFAuthor::dither_none(const pVideoFrame vf,
                         GIFImage* out, 
                         const pColorQuantizer cq) const
{
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
