#include "gifauthor.h"

GIFAuthor::GIFAuthor(int argc, char* argv[]) :
    delay(40),
    out_file("out.gif")
{};

GIFAuthor::~GIFAuthor() 
{};

pGIFAuthor GIFAuthor::create(int argc, char* argv[]) {
    return pGIFAuthor(new GIFAuthor(argc, argv));
}

pGIF GIFAuthor::generate()
{
    pGIF out;

    if(frames.size()==0)
        return out;
    
    //work out size
    if(size_opts.width() < 0 && size_opts.height() < 0)
    {
        size_opts.width(frames[0]->get_width());
        size_opts.height(frames[0]->get_height());
    }
    else if(size_opts.height() < 0)
    {
        float r = float(size_opts.width()) / float(frames[0]->get_width());
        size_opts.height(int(0.5+r*float(frames[0]->get_height())));
    }
    else if(size_opts.width() < 0)
    {
        float r = float(size_opts.height()) / float(frames[0]->get_height());
        size_opts.width(int(0.5+r*float(frames[0]->get_width())));
    }

    GIFEncoder encoder(size_opts.width(), size_opts.height(), segmenter, ditherer, colorquantizer);
    int frame_no = 0;
    for(auto fr : frames)
    {
        //scale
        fr = fr->scale_to(size_opts.width(), size_opts.height());
        
        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();
};

void GIFAuthor::load_files()
{
    frames.clear();

    for(int i = 0; i < filenames.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(filenames[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << filenames[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        frames.push_back(pv);
    }

}
