#include "gifauthor.h"

GIFAuthor::GIFAuthor() :
    Gtk::Application::Application("org.gtkmm.gifauthor"/*, Gio::APPLICATION_HANDLES_COMMAND_LINE*/),
    out(NULL),
    out_width(-1),
    out_height(-1)
{
    Glib::set_application_name("GIFAuthor");

    signal_handle_local_options().connect(
        sigc::mem_fun(*this, &GIFAuthor::on_handle_local_options), false);

    add_main_option_entry(Gio::Application::OPTION_TYPE_BOOL, "version", 'v', "Show the application version.");
};

GIFAuthor::~GIFAuthor() 
{
    delete out;
};

Glib::RefPtr<GIFAuthor> GIFAuthor::create() {
    return Glib::RefPtr<GIFAuthor>(new GIFAuthor());
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

    GIFEncoder encoder(out_width, out_height);
    int frame_no = 0;
    for(auto fr : frames)
    {
        //scale
        fr = fr->scale_to(out_width, out_height);
        
        encoder.push_frame(fr);
    }
    
    out = encoder.get_output();
};

void GIFAuthor::register_command_line() 
{
    std::cout << "register_command_line" << std::endl;
    add_main_option_entry(Gio::Application::OPTION_TYPE_BOOL,
                          "version",
                          'v',
                          "Show the version number and exit");

  /*  // parse arguments:
    Glib::OptionContext ctx("Create GIFs from images");
    Glib::OptionGroup group("extract", "Author a gif from the command line");


    int width = -1;
    Glib::OptionEntry width_entry;
    width_entry.set_long_name("width");
    width_entry.set_short_name('w');
    width_entry.set_description("the output width of the gif");
    group.add_entry(width_entry, width);

    int delay = 4;
    Glib::OptionEntry delay_entry;
    delay_entry.set_long_name("delay");
    delay_entry.set_short_name('d');
    delay_entry.set_description("delay between frames, *10 ms");
    group.add_entry(delay_entry, delay);
  
    
    ctx.set_main_group(group);
    Glib::OptionGroup sg = segmenterFactory.get_option_group();
    Glib::OptionGroup qg = quantizerFactory.get_option_group();
    Glib::OptionGroup dg = dithererFactory.get_option_group();

    ctx.add_group(sg);
    ctx.add_group(qg);
    ctx.add_group(dg);

    // add GTK options, --help-gtk, etc
    Glib::OptionGroup gtkgroup(gtk_get_option_group(true));
    ctx.add_group(gtkgroup);
    int argc;
    char **argv = cmd->get_arguments(argc);
    ctx.parse(argc, argv);

    std::vector<std::string> fnames;
    //argv should be filenames
    for(int i = 1; i < argc; i++)
    {
        fnames.push_back(std::string(argv[i]));
    }

    from_images(fnames, delay, width);
*/
}

int GIFAuthor::on_handle_local_options(const Glib::RefPtr<Glib::VariantDict>& options)
{
    std::cout << "on_handle_local_options" << std::endl;
    bool version;
    options->lookup_value("version", version);
    if(version){
        std::cout << "GifAuthor 0.1" << std::endl;
        return 0;
    }
    return 0;
}

void GIFAuthor::from_images(std::vector<std::string> fnames, int delay, int width)
{
    for(int i = 0; i < fnames.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(fnames[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << fnames[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        add_frame(pv);
    }

    set_output_size(width);
    update_output();

    std::ofstream outfile("out.gif");
    std::cout << "Writing" << std::endl;
    get_output()->write(outfile);
}

