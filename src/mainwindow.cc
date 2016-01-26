#include "mainwindow.h"

MainWindow::MainWindow()
{
    //I assume these propagate?
    add_events(Gdk::KEY_PRESS_MASK | 
               Gdk::SCROLL_MASK | 
               Gdk::POINTER_MOTION_MASK );
        
}

MainWindow::~MainWindow(){};


