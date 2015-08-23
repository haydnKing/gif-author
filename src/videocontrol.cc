#include "videocontrol.h"

#include <iostream>

VideoControl::VideoControl():
    play_state(false),
    play_forwards(true)
{
    to_start.set_image_from_icon_name("media-skip-backward");
     rv_jump.set_image_from_icon_name("media-seek-backward");
     fw_play.set_image_from_icon_name("media-playback-start");
     fw_jump.set_image_from_icon_name("media-seek-forward");
      to_end.set_image_from_icon_name("media-skip-forward");

    Glib::RefPtr<Gtk::IconTheme> iconTheme = Gtk::IconTheme::get_default();
    Glib::RefPtr<Gdk::Pixbuf> pb = iconTheme->load_icon("media-playback-start",
                                                        Gtk::ICON_SIZE_BUTTON);
    img_play_backward.set(pb->flip());
    rv_play.set_image(img_play_backward);

    attach(to_start, 0,0,1,1);
    attach(rv_jump,  1,0,1,1);
    attach(rv_play,  2,0,1,1);
    attach(fw_play,  3,0,1,1);
    attach(fw_jump,  4,0,1,1);
    attach(to_end,   5,0,1,1);


    fw_play.signal_clicked().connect(sigc::mem_fun(*this, &VideoControl::on_fw_play));
    rv_play.signal_clicked().connect(sigc::mem_fun(*this, &VideoControl::on_rv_play));
}

VideoControl::~VideoControl(){

}

void VideoControl::connect_window_keypress(Gtk::Window &window){
    window.signal_key_press_event().connect(sigc::mem_fun(*this, &VideoControl::on_window_key_press_event));
};

bool VideoControl::is_playing() const {
    return play_state;
};

bool VideoControl::is_playing_forwards() const {
    return play_forwards;
};

void VideoControl::play(bool forwards) {
    if(forwards){
        fw_play.clicked();
    }
    else{
        rv_play.clicked();
    }
};

void VideoControl::pause() {
    if(play_state && play_forwards){
        fw_play.clicked();
    }
    else if(play_state) {
        rv_play.clicked();
    }
};

void VideoControl::next_frame(){
    s_frame_next.emit();
};

void VideoControl::prev_frame(){
    s_frame_prev.emit();
};

void VideoControl::on_fw_play(){
    if(play_state && play_forwards){
        play_state = false;
    }
    else{
        play_state = true;
        play_forwards = true;
    }
    update_icons();
    s_play_state_changed.emit(play_state, play_forwards);
    on_play_tick();
};

void VideoControl::on_rv_play(){
    if(play_state && !play_forwards){
        play_state = false;
    }
    else{
        play_state = true;
        play_forwards = false;
    }
    update_icons();
    s_play_state_changed.emit(play_state, play_forwards);
    on_play_tick();
};

void VideoControl::update_icons(){
    if(play_state && play_forwards){
        rv_play.set_image(img_play_backward);
        fw_play.set_image_from_icon_name("media-playback-pause");
    }
    else if (play_state && !play_forwards){
        rv_play.set_image_from_icon_name("media-playback-pause");
        fw_play.set_image_from_icon_name("media-playback-start");
    }
    else {
        rv_play.set_image(img_play_backward);
        fw_play.set_image_from_icon_name("media-playback-start");
    }
};

sigc::signal<void, bool, bool> VideoControl::signal_play_state_changed(){
    return s_play_state_changed;
};

Glib::SignalProxy0<void> VideoControl::signal_to_start(){
    return to_start.signal_clicked();
};

Glib::SignalProxy0<void> VideoControl::signal_to_end(){
    return to_end.signal_clicked();
};

Glib::SignalProxy0<void> VideoControl::signal_seek_forward(){
    return fw_jump.signal_clicked();
};

Glib::SignalProxy0<void> VideoControl::signal_seek_backward(){
    return rv_jump.signal_clicked();
};

sigc::signal<void> VideoControl::signal_frame_next(){
    return s_frame_next;
};

sigc::signal<void> VideoControl::signal_frame_prev(){
    return s_frame_prev;
};

bool VideoControl::on_window_key_press_event(GdkEventKey* event){
    switch(event->keyval){
        case GDK_KEY_Right:
            if(event->state & GDK_SHIFT_MASK)
            {
                fw_jump.clicked();
            }
            else
            {
                s_frame_next.emit();
            }
            return true;
        case GDK_KEY_Left:
            if(event->state & GDK_SHIFT_MASK)
            {
                rv_jump.clicked();
            }
            else
            {
                s_frame_prev.emit();
            }
            return true;
        case GDK_KEY_space:
            if(play_state && !play_forwards){
                rv_play.clicked();
            }
            else{
                fw_play.clicked();
            }
            return true;
    }
    return false;
};

void VideoControl::on_play_tick(){
    bool r;
    int64_t time = g_get_monotonic_time();
    play_forwards ? next_frame() : prev_frame();
    if(play_state){
        time = (g_get_monotonic_time()- time)/1000;
        if(time < 35){
            time = 40 - time;
        }
        else{
            time = 5;
        }
        Glib::signal_timeout().connect_once(
                sigc::mem_fun(*this, &VideoControl::on_play_tick), 
                time);
    }
};
