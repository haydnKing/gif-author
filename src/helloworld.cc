#include "helloworld.h"
#include <iostream>

HelloWorld::HelloWorld()
: w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN),
  w_label("Source Video")
{
  // Sets the border width of the window.
  set_border_width(10);

  w_label.set_halign(Gtk::ALIGN_END);
  w_grid.attach(w_label, 0,0,1,1);

  w_file_chooser.set_width_chars(32);
  w_file_chooser.set_halign(Gtk::ALIGN_START);
  w_grid.attach(w_file_chooser, 1,0,1,1);

  w_image.set_size_request(400,300);
  w_grid.attach(w_image, 0,1,2,1);

  add(w_grid);

  w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
              &HelloWorld::on_file_set));

  show_all();
}

HelloWorld::~HelloWorld()
{
}

void HelloWorld::on_file_set()
{
  std::cout << "Filename: " << w_file_chooser.get_filename() << std::endl;

  cv::VideoCapture cap(w_file_chooser.get_filename());
  cv::Mat frame;
  if(!cap.isOpened()){
      std::cerr << "Failed to open file: " 
                << w_file_chooser.get_filename() << std::endl;
      return;
  }

  cap >> frame;

  frame.addref();
  std::cout << frame.isContinuous() << std::endl;

  Glib::RefPtr<Gdk::Pixbuf> pb = Gdk::Pixbuf::create_from_data(frame.ptr(),
          Gdk::COLORSPACE_RGB,
          false,
          8,
          frame.size().width,
          frame.size().height,
          frame.size().width * 3);
  
  w_image.set(pb);
}
