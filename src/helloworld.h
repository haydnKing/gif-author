#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

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

#include "videocontrol.h"
#include "videoplayer.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

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

  VideoPlayer w_player;

  Gtk::Label w_label;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
