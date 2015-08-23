#ifndef GIF_AUTHOR_VIDEO_PLAYER_H
#define GIF_AUTHOR_VIDEO_PLAYER_H

#include <gtkmm/grid.h>

#include "imagearea.h"
#include "videocontrol.h"
#include "videofile.h"

/**
 * Display and play a video
 */
class VideoPlayer : public Gtk::Grid
{
    public:
        VideoPlayer();
        virtual ~VideoPlayer();
        
        bool open_from_file(const char* filename);

        void connect_window_keypress(Gtk::Window &window);

        const VideoControl& get_controller() const;
        VideoControl& get_controller();
        
        sigc::signal<void, int64_t> signal_frame_changed();

    protected:
        void on_frame_next();
        void on_frame_prev();
        void on_seek_fw();
        void on_seek_rv();
        void on_to_start();
        void on_to_end();

        void update_image();

        AVFrame* frame;
        ImageArea w_image;
        VideoControl w_control;
        VideoFile video_input;

        sigc::signal<void, int64_t> s_frame_change;
};

#endif
