#include "gifauthor.h"

GIFAuthor::GIFAuthor(int argc, char* argv[]) :
    delay(40),
    colors(256),
    out_file("out.gif"),
    help_opt(false)
{
    og = OptionGroup::create("gif-author");

    og->add_option<bool>("help", "Show help and exit", help_opt, 'h');
    og->add_option<int>("delay", "Delay between frames, ms", delay, 'd');
    og->add_option<Size>("size", "Size of the output image, <w>x<h>. If either width w or height h is \'_\', the value is calculated keeping aspect ratio constant", size_opts, 's');
    og->add_option("colors", "Limit the number of colours per image in the resulting GIF. 256 is the maximum", colors, 'c');
    og->add_option<Crop>("crop", "Cropping of the output image to the rectangle given by <x>,<y>+<w>x<h>", crop_opts);
    og->add_option<std::string>("out", "Name of the output file", out_file, 'o');
    og->add_option(QuantizerFactory::create(colorquantizer));
    og->add_option(DithererFactory::create(ditherer));
    og->add_option(SegmenterFactory::create(segmenter));

    std::vector<string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }

    try
    {
        filenames = og->parse(args);
    }
    catch(const std::exception& ce)
    {
        const char* what = ce.what();
        if(what != 0)
            cout << "Error: " << what << endl;
        is_error = true;
    }

};

GIFAuthor::~GIFAuthor() 
{};

pGIFAuthor GIFAuthor::create(int argc, char* argv[]) {
    return pGIFAuthor(new GIFAuthor(argc, argv));
}

void GIFAuthor::write_help() const 
{
    std::cout << "gif-author converts a series of images into a GIF\n\n" 
        << "USAGE: gif-author [OPTIONS] image1 image2 ...\n\n"
        << "Options:\n" 
        << og << std::endl;
};

void GIFAuthor::print_overview() const 
{
    std::cout << "gif-author settings:\n"
        << "\t" << filenames.size() << " frames with " << delay << "ms delay\n"
        << "\tframe size is " << size_opts << "\n"
        << "\tcropping: " << crop_opts << "\n"
        << "\tcolors: " << colors << "\n"
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
    if(error()) {
        return out;
    }

    if(filenames.size() == 0) {
        std::cout << "No files given, add --help for help" << std::endl;
        return out;
    }

    load_files();

    if(frames->size()==0)
        return out;
    
    if(crop_opts) 
        frames = frames->crop(crop_opts.xpos(), 
                              crop_opts.ypos(), 
                              crop_opts.width(), 
                              crop_opts.height());
    //work out size
    if(size_opts.width() < 0 && size_opts.height() < 0)
    {
        size_opts.width(frames->width());
        size_opts.height(frames->height());
    }
    else if(size_opts.height() < 0)
    {
        float r = float(size_opts.width()) / float(frames->width());
        size_opts.height(int(0.5+r*float(frames->height())));
    }
    else if(size_opts.width() < 0)
    {
        float r = float(size_opts.height()) / float(frames->height());
        size_opts.width(int(0.5+r*float(frames->width())));
    }

    print_overview();
    //scale
    frames = frames->resize(size_opts.width(), size_opts.height());
        
    GIFEncoder encoder(size_opts.width(), 
                       size_opts.height(), 
                       segmenter, 
                       ditherer, 
                       colorquantizer, 
                       colors);
    out = encoder.encode(frames);

    int len = out->save(out_file);
    cout << out->as_string() << endl;
    cout << "Written " << humanize(len) << " bytes to file" << std::endl;
    return out;
};

void GIFAuthor::load_files()
{
    frames = Sequence::create_from_filenames(filenames, delay);
}
