#include "videoclipper.h"
#include <iostream>

VideoClipper::VideoClipper():
    VideoPlayer(),
    w_spin_start(1.),
    w_spin_end(1.),
    w_label_start("Start:"),
    w_label_end("End:"),
    w_grid()
{
    w_spin_start.set_hexpand(true);
    w_spin_start.set_increments(1.,25.);
    w_spin_end.set_hexpand(true);
    w_spin_end.set_increments(1.,25.);

    w_btn_start.set_image_from_icon_name("edit-cut");
    w_btn_start.set_tooltip_text("Start Frame to Current Frame");
    w_btn_end.set_image_from_icon_name("edit-cut");
    w_btn_end.set_tooltip_text("End Frame to Current Frame");

    w_grid.attach(w_label_start, 0, 0, 1, 1);
    w_grid.attach(w_spin_start, 1, 0, 1, 1);
    w_grid.attach(w_btn_start, 2, 0, 1, 1);
    w_grid.attach(w_label_end, 3, 0, 1, 1);
    w_grid.attach(w_spin_end, 4, 0, 1, 1);
    w_grid.attach(w_btn_end, 5, 0, 1, 1);

    signal_video_changed().connect(sigc::mem_fun(*this, &VideoClipper::on_video_changed));
    w_btn_start.signal_clicked().connect(sigc::mem_fun(*this, &VideoClipper::on_btn_start));
    w_btn_end.signal_clicked().connect(sigc::mem_fun(*this, &VideoClipper::on_btn_end));

    w_spin_start.signal_value_changed().connect(sigc::mem_fun(*this, &VideoClipper::on_start_changed));
    w_spin_end.signal_value_changed().connect(sigc::mem_fun(*this, &VideoClipper::on_end_changed));

    s_frame_changed.connect(sigc::mem_fun(*this, &VideoClipper::on_frame_changed));

    //This should work according to Gtkmm docs, but nope.
    //attach_next_to(w_grid, Gtk::POS_BOTTOM, 1,1);
    
    //this will break if VideoPlayer changes
    attach(w_grid, 0, 3, 3, 1); 
};

VideoClipper::~VideoClipper() {};

void VideoClipper::on_video_changed(){
    if(video_input && video_input->is_ok()){
        w_spin_start.set_range(0, video_input->length()-2);
        w_spin_end.set_range(1, video_input->length()-1);
        w_spin_start.set_value(0);
        w_spin_end.set_value(video_input->length()-1);
    }
};

void VideoClipper::on_btn_start(){
    if(video_input && video_input->is_ok()){
        w_spin_start.set_value(video_input->position());
    }
};

void VideoClipper::on_btn_end(){
    if(video_input && video_input->is_ok()){
        w_spin_end.set_value(video_input->position());
    }
};


void VideoClipper::on_start_changed(){
    if(w_spin_start.get_value() >= w_spin_end.get_value()){
        w_spin_end.set_value(w_spin_start.get_value() +1);
    }
};

void VideoClipper::on_end_changed(){
    if(w_spin_start.get_value() >= w_spin_end.get_value()){
        w_spin_start.set_value(w_spin_end.get_value() -1);
    }
};

void VideoClipper::on_frame_changed(int64_t frame_index){
    if(w_control.is_playing()){
        if(frame_index < w_spin_start.get_value() ||
           frame_index > w_spin_end.get_value()){
            seek_to_frame(w_spin_start.get_value()); 
        }
    }
};
            

