#include "helloworld.h"
#include <iostream>

HelloWorld::HelloWorld()
    : w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN),
    w_label("Source Video")
{
    add_events(Gdk::KEY_PRESS_MASK);

    // Sets the border width of the window.
    set_border_width(10);
    w_grid.set_row_spacing(10);
    w_grid.set_column_spacing(10);

    //setup label
    w_label.set_halign(Gtk::ALIGN_END);
    w_grid.attach(w_label, 0,0,1,1);

    //setup file chooser
    w_file_chooser.set_width_chars(32);
    w_file_chooser.set_halign(Gtk::ALIGN_START);
    w_grid.attach(w_file_chooser, 1,0,1,1);

    //setup videoplayer
    w_player.connect_window_keypress(*this);
    w_grid.attach(w_player, 0,1,2,1);
    
    //add the grid
    add(w_grid);

    //connect to events
    w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
                &HelloWorld::on_file_set));


    //show everything
    show_all();
}

HelloWorld::~HelloWorld(){};

void HelloWorld::on_file_set()
{
    w_player.open_from_file(w_file_chooser.get_filename().c_str());
}

bool HelloWorld::on_key_press_event(GdkEventKey* event){
    return false;
};
