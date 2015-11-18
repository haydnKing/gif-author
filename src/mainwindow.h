#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>

#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>
#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "widgets/videocontrol.h"
#include "widgets/videoclipper.h"

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
  Gtk::Grid w_grid;
  Gtk::FileChooserButton w_file_chooser;

  VideoClipper w_player;

  Gtk::Label w_label;
};

#endif // GTKMM_MAINWINDOW_H
