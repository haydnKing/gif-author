#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>
#include <gdk/gdkkeysyms.h>
#include <glibmm/main.h>

#include <gtkmm/window.h>
#include <gtkmm/flowbox.h>

#include <iostream>

class MainWindow : public Gtk::Window
{

public:
  MainWindow();
  virtual ~MainWindow();

 

protected:
  //Signal handlers:

  //Member widgets:
  
  Gtk::FlowBox w_flowbox;

};

#endif // GTKMM_MAINWINDOW_H
