#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

#include <stdint.h>

#include <gtkmm/image.h>
#include <gtkmm/assistant.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>
#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "widgets/videocontrol.h"
#include "widgets/videoclipper.h"

#include <iostream>

class MainWindow : public Gtk::Assistant
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
  virtual void on_cancel();
  virtual void on_close();

  //Member widgets:
  Gtk::FileChooserButton w_file_chooser;

  VideoClipper w_player;
};

#endif // GTKMM_MAINWINDOW_H
