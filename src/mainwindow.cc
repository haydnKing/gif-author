#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow() : 
    w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN)
{
    add_events(Gdk::KEY_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK );
    
    GetFile *gf = new GetFile();
    add_page(*p);
    Page *c = new Clipper(this, gf);
    add_page(*c);

    //show everything
    show_all();
}

MainWindow::~MainWindow(){};


