#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow()
{
    add_events(Gdk::KEY_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK );
    
    GetFile *gf = new GetFile();
    add_page(*gf);
    Page *c = new Clipper(this, gf);
    add_page(*c);

    //show everything
    show_all();
}

MainWindow::~MainWindow(){};


