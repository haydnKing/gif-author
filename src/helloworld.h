#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>
#include <gdkmm/pixbuf.h>

#include <opencv2/opencv.hpp>

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:
  //Signal handlers:
  void on_file_set();
  void on_image_resize(Gtk::Allocation& allocation);

  //Private functions
  void set_image(cv::Mat* frame);

  //Members
  cv::Mat* the_frame;
  cv::VideoCapture* the_cap;
  Glib::RefPtr<Gdk::Pixbuf> the_pixbuf;

  //Member widgets:
  Gtk::Grid w_grid;
  Gtk::FileChooserButton w_file_chooser;
  Gtk::Image w_image;
  Gtk::Label w_label;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
