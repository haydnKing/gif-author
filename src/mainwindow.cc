#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow() : 
    w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN)
{
    add_events(Gdk::KEY_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK );

    // Sets the border width of the window.
    set_border_width(10);
    w_grid.set_column_spacing(10);
    w_grid.set_row_spacing(10);

    //setup file chooser
    w_file_chooser.set_width_chars(32);
    w_file_chooser.set_halign(Gtk::ALIGN_CENTER);
    w_file_chooser.set_valign(Gtk::ALIGN_START);
    w_file_chooser.set_vexpand(false);

    w_stack.add(w_file_chooser, "input", "Choose input video");

    //setup videoplayer
    w_player.connect_window_keypress(*this);
    w_stack.add(w_player, "extract", "Extract Clip");

    //connect to events
    w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
                &MainWindow::on_file_set));

    //sidebar
    w_sidebar.set_stack(w_stack);

    //add to grid
    w_grid.attach(w_navigation, 0, 0, 3, 1);
    w_grid.attach(w_hsep,       0, 1, 3, 1);
    w_grid.attach(w_sidebar,    0, 2, 1, 1);
    w_grid.attach(w_vsep,       1, 2, 1, 1);
    w_grid.attach(w_stack,      2, 2, 1, 1);

    //add grid
    add(w_grid);

    //show everything
    show_all();
}

MainWindow::~MainWindow(){};

void MainWindow::on_file_set()
{
    w_player.open_from_file(w_file_chooser.get_filename().c_str());
}

bool MainWindow::on_key_press_event(GdkEventKey* event){
    return false;
};
