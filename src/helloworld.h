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

#include <opencv2/opencv.hpp>

#include "imagearea.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

  void play();
  void pause();
  bool playing();

protected:
  //Signal handlers:
  void on_file_set();
  virtual bool on_key_press_event(GdkEventKey* event);

  //Private functions
  void set_image(cv::Mat* frame);
  bool frame_next();

  //Members
  cv::Mat* the_frame;
  cv::VideoCapture* the_cap;
  sigc::connection playback_conn;

  //Member widgets:
  Gtk::Grid w_grid;
  Gtk::FileChooserButton w_file_chooser;
  ImageArea w_image_area;
  Gtk::Label w_label;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
