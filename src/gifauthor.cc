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

void GIFAuthor::load_files()
{
    frames.clear();

    for(int i = 0; i < fnames.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(fnames[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << files[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        frames.push_back(pv);
    }

}
