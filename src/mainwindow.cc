#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow() : 
    w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN)
{
    add_events(Gdk::KEY_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK );

    // Sets the border width of the window.
    set_border_width(10);

    //setup file chooser
    w_file_chooser.set_width_chars(32);
    w_file_chooser.set_halign(Gtk::ALIGN_START);

    append_page(w_file_chooser);
    set_page_title(w_file_chooser, "Choose input video");
    //set_page_has_padding(w_file_chooser);

    //setup videoplayer
    w_player.connect_window_keypress(*this);
    append_page(w_player);
    set_page_title(w_player, "Extract Clip");
    

    //connect to events
    w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
                &MainWindow::on_file_set));


    //show everything
    show_all();
}

MainWindow::~MainWindow(){};

void MainWindow::on_file_set()
{
    set_page_complete(w_file_chooser);
    w_player.open_from_file(w_file_chooser.get_filename().c_str());
}

bool MainWindow::on_key_press_event(GdkEventKey* event){
    return false;
};

void MainWindow::on_cancel()
{
    std::cout << "Cancel()" << std::endl;
    get_application()->quit();
};

void MainWindow::on_close()
{
    std::cout << "Cancel()" << std::endl;
    get_application()->quit();
};
