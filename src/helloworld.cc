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

  //setup image area
  w_image_area.set_hexpand(true);
  w_image_area.set_vexpand(true);
  w_grid.attach(w_image_area, 0,1,2,1);

  //add the grid
  add(w_grid);

  //connect to events
  w_file_chooser.signal_file_set().connect(sigc::mem_fun(*this,
              &HelloWorld::on_file_set));

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
  
  w_image_area.update_image(frame);  

  if(the_frame!=NULL){
    the_frame->release();
    //delete old;
  }
  the_frame = frame;


}

bool HelloWorld::frame_next(){
  if(the_cap!=NULL){
      cv::Mat * frame = new cv::Mat;
      std::cout << the_cap->get(CV_CAP_PROP_FRAME_WIDTH) << std::endl;
      the_cap->read(*frame);
      set_image(frame);
    return true;
  }
  return false;
}

bool HelloWorld::frame_prev(){
  if(the_cap!=NULL){
      double pos = the_cap->get(CV_CAP_PROP_POS_FRAMES);
      if(pos <= 0){
          return false;
      }
      the_cap->set(CV_CAP_PROP_POS_FRAMES, (double)0.);
      cv::Mat * frame = new cv::Mat;
      the_cap->read(*frame);
      set_image(frame);
    return true;
  }
  return false;
}

bool HelloWorld::on_key_press_event(GdkEventKey* event){
    switch(event->keyval){
        case GDK_KEY_Right:
            frame_next();
            return true;
        case GDK_KEY_Left:
            frame_prev();
            return true;
        case GDK_KEY_space:
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
