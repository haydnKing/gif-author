#include "videoplayer.h"

#include <iostream>

VideoPlayer::VideoPlayer(){
    frame = NULL;

    set_row_spacing(10);

    w_image.set_vexpand(true);
    w_image.set_hexpand(true);
    attach(w_image, 0,0,1,1);
    w_control.set_halign(Gtk::ALIGN_CENTER);
    attach(w_control, 0,1,1,1);

    w_control.signal_frame_next().connect(sigc::mem_fun(*this, &VideoPlayer::on_frame_next));
    w_control.signal_frame_prev().connect(sigc::mem_fun(*this, &VideoPlayer::on_frame_prev));
    w_control.signal_seek_forward().connect(sigc::mem_fun(*this, &VideoPlayer::on_seek_fw));
    w_control.signal_seek_backward().connect(sigc::mem_fun(*this, &VideoPlayer::on_seek_rv));
    w_control.signal_to_start().connect(sigc::mem_fun(*this, &VideoPlayer::on_to_start));
    w_control.signal_to_end().connect(sigc::mem_fun(*this, &VideoPlayer::on_to_end));
};

VideoPlayer::~VideoPlayer(){};

bool VideoPlayer::open_from_file(const char* filename){
    if(!video_input.open(filename)){
        return false;
    }
    w_control.next_frame();
};

void VideoPlayer::connect_window_keypress(Gtk::Window &window){
    w_control.connect_window_keypress(window);
};

const VideoControl& VideoPlayer::get_controller() const {
    return w_control;
};

VideoControl& VideoPlayer::get_controller() {
    return w_control;
};

sigc::signal<void, int64_t> VideoPlayer::signal_frame_changed(){
    return s_frame_change;
};

void VideoPlayer::on_frame_next(){
    if(video_input.is_open()){
        if(!video_input.get_next_frame(&frame))
            return;
        update_image();
        s_frame_change.emit(video_input.get_frame_index());
    }
};

void VideoPlayer::on_frame_prev(){
    if(video_input.is_open()){
        if(!video_input.get_prev_frame(&frame))
            return;
        update_image();
        s_frame_change.emit(video_input.get_frame_index());
    }
};

void VideoPlayer::update_image(){
    w_image.update_image(frame->data[0],
            frame->width,
            frame->height,
            frame->linesize[0]);  
};

void VideoPlayer::on_seek_fw(){
    video_input.skip_to_frame(video_input.get_frame_index()+25);
    if(!w_control.is_playing()){
        w_control.next_frame();
    }
};

void VideoPlayer::on_seek_rv(){
    video_input.skip_to_frame(video_input.get_frame_index()-25);
    if(!w_control.is_playing()){
        w_control.next_frame();
    }
};

void VideoPlayer::on_to_start(){
    video_input.skip_to_frame(0);
    if(!w_control.is_playing()){
        w_control.next_frame();
    }
};

void VideoPlayer::on_to_end(){
    video_input.skip_to_frame(video_input.get_length_frames());
    if(!w_control.is_playing()){
        w_control.next_frame();
    }
};

