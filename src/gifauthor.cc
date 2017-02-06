#include "gifauthor.h"

GIFAuthor::GIFAuthor(int argc, char* argv[]) :
    delay(40),
    out_file("out.gif"),
    help_opt(false)
{
    og = OptionGroup::create("gif-author");

    og->add_option<bool>("help", "show help and exit", help_opt);
    og->add_option<int>("delay", "delay between frames, ms", delay);
    og->add_option<Crop>("crop", "cropping of the output image", crop_opts);
    og->add_option<Size>("size", "size of the output image", size_opts);
    og->add_option<std::string>("out", "name of the output file", out_file);
    og->add_option(QuantizerFactory::create(colorquantizer));
    og->add_option(DithererFactory::create(ditherer));
    og->add_option(SegmenterFactory::create(segmenter));

    std::vector<string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    filenames = og->parse(args);

};

GIFAuthor::~GIFAuthor() 
{};

pGIFAuthor GIFAuthor::create(int argc, char* argv[]) {
    return pGIFAuthor(new GIFAuthor(argc, argv));
}

void GIFAuthor::write_help() const 
{
    std::cout << "usage: gif-author [OPTIONS] image1 image2 ...\n"
        << "\tconvert a series of images into a GIF\n" 
        << og->help() << std::endl;
};

void GIFAuthor::print_overview() const 
{
    std::cout << "gif-author settings:\n"
        << "\t" << filenames.size() << " frames with " << delay << "ms delay\n"
        << "\tframe size is " << size_opts << "\n"
        << "\tcropping: " << crop_opts << "\n"
        << "\tsegmenter: " << segmenter->name() << "\n"
        << "\tcolorquantizer: " << colorquantizer->name() << "\n"
        << "\tditherer: " << ditherer->name() << std::endl;
    
};

pGIF GIFAuthor::run()
{
    pGIF out;
    if(help_opt) {
        write_help();
        return out;
    }

    if(filenames.size() == 0) {
        std::cout << "No files given, add --help for help" << std::endl;
        return out;
    }

    load_files();

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
    
    print_overview();

    GIFEncoder encoder(size_opts.width(), size_opts.height(), segmenter, ditherer, colorquantizer);
    int frame_no = 0;
    for(auto fr : frames)
    {
        if(crop_opts) {
            fr = fr->crop(crop_opts.xpos(), crop_opts.ypos(), crop_opts.width(), crop_opts.height());
        }
        //scale
        fr = fr->scale_to(size_opts.width(), size_opts.height());
        
        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();

    out->save(out_file);
    return out;
};

void GIFAuthor::load_files()
{
    frames.clear();

    for(int i = 0; i < filenames.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(filenames[i], i*delay, i);

        frames.push_back(pv);
    }

}
