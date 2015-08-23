#ifndef GTKMM_GIFAUTHOR_VIDEOCONTROL_H
#define GTKMM_GIFAUTHOR_VIDEOCONTROL_H

#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/window.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/main.h>
#include <string>

class VideoControl : public Gtk::Grid
{
    public:
        VideoControl();
        virtual ~VideoControl();

        void connect_window_keypress(Gtk::Window &window);

        bool is_playing() const;
        bool is_playing_forwards() const;
        void play(bool forwards=true);
        void pause();

        void next_frame();
        void prev_frame();

        //signals
        sigc::signal<void, bool, bool> signal_play_state_changed();
        Glib::SignalProxy0<void> signal_to_start();
        Glib::SignalProxy0<void> signal_to_end();
        sigc::signal<void> signal_frame_next();
        sigc::signal<void> signal_frame_prev();
        Glib::SignalProxy0<void> signal_seek_forward();
        Glib::SignalProxy0<void> signal_seek_backward();


    protected:
        bool on_window_key_press_event(GdkEventKey* event);
        void on_play_tick();

        bool play_state;
        bool play_forwards;
        Gtk::Button to_start, 
                    rv_jump, 
                    rv_play, 
                    fw_play,
                    fw_jump,
                    to_end;
        Gtk::Image img_play_backward;

        void on_fw_play();
        void on_rv_play();
        void update_icons();

        sigc::signal<void, bool, bool> s_play_state_changed;
        sigc::signal<void> s_frame_next;
        sigc::signal<void> s_frame_prev;
};

#endif //GTKMM_GIFAUTHOR_VIDEOCONTROL_H
