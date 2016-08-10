#include "mainwindow.h"
#include <gtkmm/application.h>
#include <glibmm/optioncontext.h>
#include <string>
#include <fstream>
#include <sstream>

#include "video/videofile.h"
#include "gifauthor.h"
#include "encoder/segmenter.h"

bool extract(std::string fname, int frame, int length, int out_width)
{
    std::cout << "Extract frame " << frame << " from \"" << fname << "\"" << std::endl;
    VideoFile vfile;
    if(!vfile.open(fname.c_str()))
    {
        std::cerr << "Failed to open file" << std::endl;
        return false;
    }

    if(vfile.length() < frame)
    {
        std::cerr << "Video is only " << vfile.length() << " frames long" << std::endl;
        return false;
    }

    GIFAuthor ga;
    std::list<pVideoFrame> frames = vfile.extract(frame, frame+length);
    int frame_no = 0;
    for(auto it : frames)
    {
        /*
        stringstream ss;
        ss << "frames/" << frame_no++ << ".ppm";
        it->write_ppm(ss.str().c_str());
        */
        ga.add_frame(it);
    }
    std::cout << "Got frames" << std::endl;
    ga.set_output_size(out_width);
    ga.update_output();
    std::cout << "Opening file" << std::endl;

    std::ofstream outfile("out.gif");
    std::cout << "Writing" << std::endl;
    ga.get_output()->write(outfile);
};

void from_images(std::vector<std::string> fnames, int delay, int width)
{
    GIFAuthor ga;
    for(int i = 0; i < fnames.size(); i++)
    {
        pVideoFrame pv = VideoFrame::create_from_file(fnames[i], i*delay, i);

        std::cout << "Load frame " << i << ": " << fnames[i] << " -> (" << pv->get_width() << "x" << pv->get_height() << ")" << std::endl;
        ga.add_frame(pv);
    }

    ga.set_output_size(width);
    ga.update_output();

    std::ofstream outfile("out.gif");
    std::cout << "Writing" << std::endl;
    ga.get_output()->write(outfile);
}

int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd,
                    Glib::RefPtr<Gtk::Application> &app) 
{
    // parse arguments:
    Glib::OptionContext ctx("Create GIFs");
    Glib::OptionGroup group("extract", "Author a gif from the command line");

    std::string fname;
    Glib::OptionEntry file_entry;
    file_entry.set_long_name("input");
    file_entry.set_short_name('i');
    //file_entry.set_flags(Glib::FLAG_FILENAME);
    file_entry.set_description("set the input file");
    group.add_entry_filename(file_entry, fname);

    int frame = 0;
    Glib::OptionEntry frame_entry;
    frame_entry.set_long_name("frame");
    frame_entry.set_short_name('f');
    frame_entry.set_description("the first frame to extract");
    group.add_entry(frame_entry, frame);

    int length = 1;
    Glib::OptionEntry length_entry;
    length_entry.set_long_name("length");
    length_entry.set_short_name('l');
    length_entry.set_description("the number of frames to extract");
    group.add_entry(length_entry, length);

    int width = -1;
    Glib::OptionEntry width_entry;
    width_entry.set_long_name("width");
    width_entry.set_short_name('w');
    width_entry.set_description("the output width of the gif");
    group.add_entry(width_entry, width);

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

    // maybe show the gui
    if(fname == "")
    {
        std::cout << "Starting GUI, see --help to run on commang line" << std::endl;
        app->activate();
    }
    else
    {
        extract(fname, frame, length, width);
    }
    return 0;
}
void frame_test()
{
    pVideoFrame f = VideoFrame::create_from_file("input/3649.png");

    f->write_ppm("out/full.ppm");

    pVideoFrame s = f->crop(10,10,10,10);

    s->write_ppm("out/small.ppm");

    std::cout << "f @ " << &f << std::endl;
    std::cout << "s @ " << &s << std::endl;
}

int gif_from_images_cline(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd,
                    Glib::RefPtr<Gtk::Application> &app) 
{
    // parse arguments:
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

    return 0;
}


int main (int argc, char *argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.gifauthor", Gio::APPLICATION_HANDLES_COMMAND_LINE);

    app->signal_command_line().connect(
            sigc::bind(sigc::ptr_fun(*gif_from_images_cline), app), false);

    MainWindow mainwindow;

    //Shows the window and returns when it is closed.
    return app->run(mainwindow);
}
