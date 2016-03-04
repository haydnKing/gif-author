#include "gifauthor.h"

GIFAuthor::GIFAuthor() :
    out(NULL),
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

    std::vector<pVideoFrame>::iterator it;
    pVideoFrame fr;
    GIFEncoder encoder(out_width, out_height, qm, dm);
    for(it = frames.begin(); it < frames.end(); it++)
    {
        fr = *it;
        //scale
        fr = fr->scale_to(out_width, out_height);

        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();
};



