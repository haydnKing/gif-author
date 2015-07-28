#include "helloworld.h"
#include <iostream>

HelloWorld::HelloWorld()
: w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN),
  w_label("Source Video"),
  playback_conn()
{
  the_frame = NULL;
  the_cap = NULL;
  

  // Sets the border width of the window.
  set_border_width(10);
  w_grid.set_row_spacing(10);
  w_grid.set_column_spacing(10);

  //setup label
  w_label.set_halign(Gtk::ALIGN_END);
  w_grid.attach(w_label, 0,0,1,1);

  //setup file chooser
  w_file_chooser.set_width_chars(32);
  w_file_chooser.set_halign(Gtk::ALIGN_START);
  w_grid.attach(w_file_chooser, 1,0,1,1);

  //setup scrolledWindow
  w_sw.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  //change to the line below when 3.16 ships
  //w_sw.set_policy(Gtk::POLICY_EXTERNAL, Gtk::POLICY_EXTERNAL);
  w_sw.set_size_request(400,300);

  //setup image
  w_image.set_hexpand(true);
  w_image.set_vexpand(true);
  w_sw.add(w_image);
  w_grid.attach(w_sw, 0,1,2,1);

  //add the grid
  add(w_grid);

  //connect to events
  w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
              &HelloWorld::on_file_set));

  w_sw.signal_size_allocate().connect(sigc::mem_fun(*this,
              &HelloWorld::on_image_resize));

  add_events(Gdk::KEY_PRESS_MASK);

  //show everything
  show_all();
}

HelloWorld::~HelloWorld()
{
    if(the_frame != NULL){
        the_frame->release();
    }
    if(the_cap != NULL){
        the_cap->release();
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
  frame_next();
}

void HelloWorld::play(){
  //if we're not playing, start playing
  if(playback_conn.empty()){
    playback_conn = Glib::signal_timeout().connect(sigc::mem_fun(*this,
                &HelloWorld::frame_next), 40);
  }
}

void HelloWorld::pause(){
    playback_conn.disconnect();
}


void HelloWorld::set_image(cv::Mat* frame){
  cv::Mat* old = the_frame;
  the_frame = frame;
  
  //the_frame->addref();
  the_pixbuf = Gdk::Pixbuf::create_from_data(the_frame->ptr(),
          Gdk::COLORSPACE_RGB,
          false,
          8,
          the_frame->size().width,
          the_frame->size().height,
          the_frame->size().width * 3);

  w_image.set(the_pixbuf->scale_simple(w_image.get_allocated_width()-1,
              w_image.get_allocated_height()-1,
              Gdk::INTERP_BILINEAR));

  if(old!=NULL){
    old->release();
    //delete old;
  }


}

void HelloWorld::on_image_resize(Gtk::Allocation& allocation){
  if(the_pixbuf){
      w_image.set(the_pixbuf->scale_simple(allocation.get_width(),
                  allocation.get_height(),
                  Gdk::INTERP_BILINEAR));
  }
}

bool HelloWorld::frame_next(){
  if(the_cap!=NULL){
      cv::Mat * frame = new cv::Mat;
      the_cap->read(*frame);
      set_image(frame);
    return true;
  }
  return false;
}


bool HelloWorld::on_key_press_event(GdkEventKey* event){
    if(event->keyval == GDK_KEY_Right){
        frame_next();
        return true;
    }
    if(event->keyval == GDK_KEY_space){
        if(playing()){
            pause();
        }
        else{
            play();
        }
        return true;
    }
    return false;
}

bool HelloWorld::playing(){
    return !playback_conn.empty();
}
