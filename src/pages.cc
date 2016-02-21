#include "pages.h"

GetFile::GetFile() : 
    w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN)
{
    //setup file chooser
    w_file_chooser.set_width_chars(32);
    w_file_chooser.set_halign(Gtk::ALIGN_CENTER);
    w_file_chooser.set_valign(Gtk::ALIGN_START);
    w_file_chooser.set_border_width(20);
    w_file_chooser.set_vexpand(false);
    //connect to events
    w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
                &GetFile::on_file_set));
    set_title("Select File");
};

Gtk::Widget* GetFile::get_widget()
{
    return &w_file_chooser;
};

void GetFile::on_file_set()
{
    //should include error checking
    set_completed(true);
}

const char* GetFile::get_filename() const
{
    return w_file_chooser.get_filename().c_str();
};

Clipper::Clipper(Gtk::Window* wnd, GetFile* file):
    the_file(file)
{
    //setup videoplayer
    w_player.connect_window_keypress(*wnd);
    w_player.set_border_width(10);

    w_player.signal_show().connect(sigc::mem_fun(
                *this, 
                &Clipper::on_show));

    set_title("Select Clip");
    set_completed(true);
};

Gtk::Widget* Clipper::get_widget()
{
    return &w_player;
};

void Clipper::on_show()
{
    w_player.open_from_file(the_file->get_filename());
};
