#include "gifauthor.h"

GIFAuthor::GIFAuthor(int argc, char* argv[]) :
    delay(40),
    is_error(false),
    out_file_name("out.gif")
{
    pOptionGroup og = OptionGroup::create("mainGroup");
    bool help = false;
    
    og->add_option<bool>("help", "Display help options and exit", help);
    og->add_option<int>("delay", "delay between frames in ms", delay);
    og->add_option<string>("out", "name of the output file", out_file_name);
    og->add_option<Crop>("crop", "cropping of the output image", crop_opts);
    og->add_option<Size>("size", "size of the output image", scale_opts);
    og->add_option(QuantizerFactory::create(colorquantizer));
    og->add_option(DithererFactory::create(ditherer));
    og->add_option(SegmenterFactory::create(segmenter));

    std::vector<string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
    in_file_names = og->parse(args);

    if(help) {
        std::cout << og->help() << std::endl;
        is_error = true;
    }

};

GIFAuthor::~GIFAuthor() 
{};

pGIFAuthor GIFAuthor::create(int argc, char* argv[]) {
    return pGIFAuthor(new GIFAuthor(argc, argv));
}

pGIF GIFAuthor::generate()
{
    pGIF out;

    if(frames.size()==0 && is_error)
        return NULL;
    
    //work out size
    int out_width = scale_opts.width();
    int out_height= scale_opts.height();
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
        if(crop_opts) {
            fr = fr->crop(crop_opts.xpos(), crop_opts.ypos(), crop_opts.width(), crop_opts.height());
        }
        //scale
        fr = fr->scale_to(out_width, out_height);
        
        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();

    std::ofstream os(out_file_name);
    out->write(os);

    return out;
};

void GIFAuthor::load_files()
{
    frames.clear();

    for(int i = 0; i < in_file_names.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(in_file_names[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << in_file_names[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        frames.push_back(pv);
    }

}
