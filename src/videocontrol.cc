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
    std::cout << "pb = " << pb << std::endl;
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

void VideoControl::on_fw_play(){
    if(play_state && play_forwards){
        play_state = false;
    }
    else{
        play_state = true;
        play_forwards = true;
    }
    update_icons();
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

