#include "mainwindow.h"
#include <gtkmm/application.h>
#include <glibmm/optioncontext.h>
#include <string>
#include <fstream>

#include "input/videofile.h"
#include "gifauthor.h"

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
    for(std::list<pVideoFrame>::iterator it = frames.begin(); it != frames.end(); it++)
        ga.add_frame(*it);
    std::cout << "Got frames" << std::endl;
    ga.set_output_size(out_width);
    ga.update_output();
    std::cout << "Opening file" << std::endl;

    std::ofstream outfile("out.gif");
    std::cout << "Writing" << std::endl;
    ga.get_output()->write(outfile);
};

int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmd,
                    Glib::RefPtr<Gtk::Application> &app) 
{
    // parse arguments:
    Glib::OptionContext ctx("Create GIFs");
    Glib::OptionGroup group("options", "main options");

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
    frame_entry.set_description("the frame to extract");
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

    ctx.add_group(group);

    // add GTK options, --help-gtk, etc
    Glib::OptionGroup gtkgroup(gtk_get_option_group(true));
    ctx.add_group(gtkgroup);
    int argc;
    char **argv = cmd->get_arguments(argc);
    ctx.parse(argc, argv);

    // maybe show the gui
    if(fname == "")
        app->activate();
    else
    {
        extract(fname, frame, length, width);
    }
    return 0;
}

int main (int argc, char *argv[])
{
    
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.gifauthor", Gio::APPLICATION_HANDLES_COMMAND_LINE);

    app->signal_command_line().connect(
            sigc::bind(sigc::ptr_fun(*on_command_line), app), false);

    MainWindow mainwindow;

    //Shows the window and returns when it is closed.
    return app->run(mainwindow);
}
