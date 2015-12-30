#include "stacknavigation.h"

StackNavigation::StackNavigation()
{
    //set_column_spacing(10);

    w_left_btn.set_label("Previous");
    w_left_btn.set_halign(Gtk::ALIGN_START);
    set_title("Title");
    w_right_btn.set_label("Next");
    w_right_btn.set_halign(Gtk::ALIGN_END);

    pack_start(w_left_btn);
    pack_end(w_right_btn);

    //set_hexpand(true);
    //set_vexpand(false);
};

