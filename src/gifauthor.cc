#include "gifauthor.h"

GIFAuthor::GIFAuthor() :
    out(NULL),
    out_width(-1),
    out_height(-1),
    delay(40),
    out_file("out.gif")
{};

GIFAuthor::~GIFAuthor() 
{
    delete out;
};

pGIFAuthor GIFAuthor::create() {
    return pGIFAuthor(new GIFAuthor());
}

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

    GIFEncoder encoder(out_width, out_height, segmenter, ditherer, colorquantizer);
    int frame_no = 0;
    for(auto fr : frames)
    {
        //scale
        fr = fr->scale_to(out_width, out_height);
        
        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();
};

void GIFAuthor::from_files(const std::vector<std::string>& files)
{

    for(int i = 0; i < files.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(files[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << files[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        add_frame(pv);
    }

    update_output();

    std::ofstream outstream(out_file);
    std::cout << "Writing to " << out_file << std::endl;
    get_output()->write(outstream);
}
