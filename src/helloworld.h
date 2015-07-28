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
  void on_image_resize(Gtk::Allocation& allocation);
  virtual bool on_key_press_event(GdkEventKey* event);

  //Private functions
  void set_image(cv::Mat* frame);
  bool frame_next();

  //Members
  cv::Mat* the_frame;
  cv::VideoCapture* the_cap;
  Glib::RefPtr<Gdk::Pixbuf> the_pixbuf;
  sigc::connection playback_conn;

  //Member widgets:
  Gtk::Grid w_grid;
  Gtk::ScrolledWindow w_sw;
  Gtk::FileChooserButton w_file_chooser;
  Gtk::Image w_image;
  Gtk::Label w_label;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
