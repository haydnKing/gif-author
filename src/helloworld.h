#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>
#include <gdk/gdkkeysyms.h>

#include <glibmm/main.h>

#include "videofile.h"
#include "imagearea.h"
#include "videoscrollbar.h"
#include "videocontrol.h"

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
  virtual bool on_key_press_event(GdkEventKey* event);

  //Private functions
  void set_image(AVFrame* frame);
  bool frame_next();
  bool frame_prev();
  void play_fn();

  //Members
  AVFrame *frame;
  VideoFile video;
  bool playing, play_reverse;

  //Member widgets:
  Gtk::Grid w_grid;
  Gtk::FileChooserButton w_file_chooser;
  ImageArea w_image_area;
  VideoScrollbar w_video_scrollbar;
  Gtk::Label w_label;
  VideoControl w_video_control;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
