#include "stacknavigation.h"

StackNavigation::StackNavigation()
{
    set_column_spacing(10);

    w_left_btn.set_label("Previous");
    w_left_btn.set_halign(Gtk::ALIGN_START);
    w_label.set_label("Title");
    w_label.set_halign(Gtk::ALIGN_FILL);
    w_label.set_hexpand(true);
    w_right_btn.set_label("Next");
    w_right_btn.set_halign(Gtk::ALIGN_END);

    attach(w_left_btn, 0, 0, 1, 1);
    attach(w_label, 1, 0, 2, 1);
    attach(w_right_btn, 3, 0, 1, 1);

    set_hexpand(true);
    set_vexpand(false);
};

