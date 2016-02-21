#ifndef GIF_AUTHOR_PAGES_H
#define GIF_AUTHOR_PAGES_H

#include "navigation.h"

#include <gtkmm/filechooserbutton.h>
#include "widgets/videoclipper.h"

class GetFile : public Page
{
    public:
        GetFile();
        virtual ~GetFile() {};

        virtual Gtk::Widget* get_widget();
        const char* get_filename() const;

    private:
        void on_file_set();
        Gtk::FileChooserButton w_file_chooser;
};

class Clipper : public Page
{
    public:
        Clipper(Gtk::Window* wnd, GetFile* file);
        virtual ~Clipper() {};

        virtual Gtk::Widget* get_widget();

    private:
        void on_show();

        VideoClipper w_player;
        GetFile* the_file;
};


#endif//GIF_AUTHOR_PAGES_H
