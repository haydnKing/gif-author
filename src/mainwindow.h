#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>

#include <gtkmm/image.h>
#include <gtkmm/window.h>
#include <gtkmm/stack.h>
#include <gtkmm/stacksidebar.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/hvseparator.h>
#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "widgets/videocontrol.h"
#include "widgets/videoclipper.h"
#include "widgets/stacknavigation.h"

#include <iostream>

class MainWindow : public Gtk::Window
{

public:
  MainWindow();
  virtual ~MainWindow();

  void play(bool reverse=false);
  void pause();
  bool isPlaying();

protected:
  //Signal handlers:
  void on_file_set();
  bool on_key_press_event(GdkEventKey* event);

  //Member widgets:
  Gtk::FileChooserButton w_file_chooser;

  VideoClipper w_player;

  Gtk::Stack w_stack;
  Gtk::StackSidebar w_sidebar;
  Gtk::Grid w_grid;
  Gtk::VSeparator w_vsep;
  Gtk::HSeparator w_hsep;
  StackNavigation w_navigation;
};

#endif // GTKMM_MAINWINDOW_H
