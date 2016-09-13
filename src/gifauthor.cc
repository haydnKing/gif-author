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

    register_command_line();
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
    add_main_option_entry(Gio::Application::OPTION_TYPE_BOOL,
                          "version",
                          'v',
                          "Show the version number and exit");

    add_main_option_entry(sigc::mem_fun(*this, &GIFAuthor::parse_width_height),
                          "size",
                          's',
                          "Change the size of the output GIF. Setting either W or H to _ will preserve aspect ration",
                          "WxH");

    add_main_option_entry(Gio::Application::OPTION_TYPE_INT,
                          "delay",
                          'd',
                          "The delay beween frames in ms (default = 40ms)",
                          "delta");
  

    add_main_option_entry(sigc::mem_fun(segmenterFactory, &SegmenterFactory::on_parse),
                          "segmenter",
                          '\0',
                          segmenterFactory.get_help_string(),
                          segmenterFactory.get_format_string());

    add_main_option_entry(sigc::mem_fun(quantizerFactory, &QuantizerFactory::on_parse),
                          "quantizer",
                          '\0',
                          quantizerFactory.get_help_string(),
                          quantizerFactory.get_format_string());

    add_main_option_entry(sigc::mem_fun(dithererFactory, &DithererFactory::on_parse),
                          "ditherer",
                          '\0',
                          dithererFactory.get_help_string(),
                          dithererFactory.get_format_string());
/* 
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

bool GIFAuthor::parse_width_height(const Glib::ustring& name, const Glib::ustring& value, bool has_value) {
    if(!has_value) 
    {
        return false;
    }
    int pos = value.find('x');
    if (pos < 0) return false;
    
    try {
        Glib::ustring lhs = value.substr(0, pos);
        if(lhs.compare("_") == 0)
        {
            out_width = -1;
        } else {
            out_width = std::stoi(lhs);
            if(out_width <= 0) {
                return false;
            }
        }
        Glib::ustring rhs = value.substr(pos+1, Glib::ustring::npos);
        if(rhs.compare("_") == 0)
        {
            out_height = -1;
        } else {
            out_height = std::stoi(rhs);
            if(out_height <= 0) 
            {
                return false;
            }
        }
    } catch (...) {
        return false;
    }


    return true;
}

