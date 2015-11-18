#ifndef GIF_AUTHOR_VIDEO_PLAYER_H
#define GIF_AUTHOR_VIDEO_PLAYER_H

#include <gtkmm/grid.h>
#include <gtkmm/spinbutton.h>

#include "../input/video.h"
#include "imagearea.h"
#include "videocontrol.h"
#include "videoscrollbar.h"

/**
 * Display and play a video
 */
class VideoPlayer : public Gtk::Grid
{
    public:
        VideoPlayer();
        virtual ~VideoPlayer();
        
        bool open_from_file(const char* filename);
        bool open_from_video(Video *video);

        void connect_window_keypress(Gtk::Window &window);

        const VideoControl& get_controller() const;
        VideoControl& get_controller();
        
        sigc::signal<void, int64_t> signal_frame_changed();
        sigc::signal<void> signal_video_changed();

    protected:
        void seek_to_frame(int64_t frame);

        void on_frame_next();
        void on_frame_prev();
        void on_seek_fw();
        void on_seek_rv();
        void on_to_start();
        void on_to_end();

        void on_frame_changed(int64_t frame_index);
        void on_spin_changed();

        void update_image();

        ImageArea w_image;
        VideoControl w_control;
        Video *video_input;
        Gtk::SpinButton w_frame;
        VideoScrollbar w_scrollbar;

        Glib::RefPtr<VideoFrame> frame;

        sigc::signal<void, int64_t> s_frame_changed;
        sigc::signal<void> s_video_changed;
};

#endif
