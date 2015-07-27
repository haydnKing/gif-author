#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/image.h>
#include <gtkmm/grid.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/window.h>
#include <gtkmm/label.h>


class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:
  //Signal handlers:
  void on_file_set();

  //Member widgets:
  Gtk::Grid w_grid;
  Gtk::FileChooserButton w_file_chooser;
  Gtk::Image w_image;
  Gtk::Label w_label;
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H
