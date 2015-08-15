#include "helloworld.h"
#include <iostream>

HelloWorld::HelloWorld()
    : w_file_chooser("Select a Video", Gtk::FILE_CHOOSER_ACTION_OPEN),
    w_label("Source Video"),
    playing(false)
{
    frame =NULL;

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
    if(frame != NULL){
        av_free(frame);
        frame = NULL;
    }
}

void HelloWorld::on_file_set()
{
    video.open(w_file_chooser.get_filename().c_str());
    frame_next();
}

void HelloWorld::play(bool reverse){
    //if we're not playing, start playing
    if(!isPlaying()){
        playing = true;
        play_reverse = reverse;
        play_fn();
    }
}

void HelloWorld::pause(){
    playing = false;
}


void HelloWorld::set_image(AVFrame* frame){
    std::cout << video.get_frame_index() << std::endl;
    w_image_area.update_image(frame->data[0],
            frame->width,
            frame->height,
            frame->linesize[0]);  
}

bool HelloWorld::frame_next(){
    if(video.isOpen()){
        if(!video.get_next_frame(&frame)){
            return false;
        }
        set_image(frame);
        return true;
    }
    return false;
}

bool HelloWorld::frame_prev(){
    if(video.isOpen()){
        if(!video.get_prev_frame(&frame)){
            return false;
        }
        set_image(frame);
        return true;
    }
    return false;
}

void HelloWorld::play_fn(){
    bool r;
    int64_t time = g_get_monotonic_time();
    if(play_reverse){
        r = frame_prev();
    }
    else {
        r = frame_next();
    }
    if(!r){
        pause();
        return;
    }
    if(playing){
        time = (g_get_monotonic_time()- time)/1000;
        if(time < 35){
            time = 40 - time;
        }
        else{
            time = 5;
        }
        Glib::signal_timeout().connect_once(
                sigc::mem_fun(*this,
                              &HelloWorld::play_fn), 
                time);
    }
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
            if(isPlaying()){
                pause();
            }
            else{
                play(event->state & GDK_SHIFT_MASK);
            }
            return true;
    }
    return false;
}

bool HelloWorld::isPlaying(){
    return playing;
}
