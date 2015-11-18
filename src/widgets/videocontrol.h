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

/**
 * A set of widgets for controlling the video playback, maintains play state
 * and emits a set of signals to the videoplayer widget
 */
class VideoControl : public Gtk::Grid
{
    public:
        VideoControl();
        virtual ~VideoControl();

        /*
         * This function should be called with the main window so that keypresses
         * can be handled by the widget even when it doesn't have focus
         */
        void connect_window_keypress(Gtk::Window &window);

        bool is_playing() const;
        bool is_playing_forwards() const;
        void play(bool forwards=true);
        void pause();

        void next_frame();
        void prev_frame();

        //signals
        /**
         * event fired when the play state changes
         * bool: play_state true if playing
         * bool: forwards true if playing forwards (value undefined if play_state false)
         */
        sigc::signal<void, bool, bool> signal_play_state_changed();
        /**
         * Triggered when the video should be moved to the start
         */
        Glib::SignalProxy0<void> signal_to_start();
        /**
         * Triggered when the video should be moved to the end
         */
        Glib::SignalProxy0<void> signal_to_end();
        /**
         * Triggered when the used explicitly wants to see the next frame
         */
        sigc::signal<void> signal_frame_next();
        /**
         * Triggered when the used explicitly wants to see the previous frame
         */
        sigc::signal<void> signal_frame_prev();
        /**
         * Seek forward by 25 frames
         */
        Glib::SignalProxy0<void> signal_seek_forward();
        /** 
         * Seek backward by 25 frames
         */
        Glib::SignalProxy0<void> signal_seek_backward();

    protected:
        bool on_window_key_press_event(GdkEventKey* event);

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
