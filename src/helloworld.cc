#include "helloworld.h"
#include <iostream>

HelloWorld::HelloWorld()
: w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN),
  w_label("Source Video")
{
  the_frame = NULL;
  the_cap = NULL;
  

  // Sets the border width of the window.
  set_border_width(10);

  w_label.set_halign(Gtk::ALIGN_END);
  w_grid.attach(w_label, 0,0,1,1);

  w_file_chooser.set_width_chars(32);
  w_file_chooser.set_halign(Gtk::ALIGN_START);
  w_grid.attach(w_file_chooser, 1,0,1,1);

  w_image.set_size_request(400,300);
  w_image.set_hexpand(true);
  w_image.set_vexpand(true);
  w_grid.attach(w_image, 0,1,2,1);

  add(w_grid);

  w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
              &HelloWorld::on_file_set));

  show_all();
}

HelloWorld::~HelloWorld()
{
    if(the_frame != NULL){
        the_frame->release();
    }
    if(the_cap != NULL){
        delete the_cap;
    }
}

void HelloWorld::on_file_set()
{
  std::cout << "Filename: " << w_file_chooser.get_filename() << std::endl;

  if(the_cap != NULL){
      delete the_cap;
  }
  the_cap = new cv::VideoCapture(w_file_chooser.get_filename());

  if(!the_cap->isOpened()){
      std::cerr << "Failed to open file: " 
                << w_file_chooser.get_filename() << std::endl;
      return;
  }

  cv::Mat * frame = new cv::Mat();
  (*the_cap) >> *frame;

  set_image(frame);
}

void HelloWorld::set_image(cv::Mat* frame){
  cv::Mat* old = the_frame;
  the_frame = frame;
  
  the_frame->addref();
  the_pixbuf = Gdk::Pixbuf::create_from_data(the_frame->ptr(),
          Gdk::COLORSPACE_RGB,
          false,
          8,
          the_frame->size().width,
          the_frame->size().height,
          the_frame->size().width * 3);

  w_image.set(the_pixbuf->scale_simple(w_image.get_allocated_width(),
              w_image.get_allocated_height(),
              Gdk::INTERP_BILINEAR));

  if(old!=NULL){
    old->release();
  }

}

