#ifndef GIF_AUTHOR_VIDEO_CLIPPER_H
#define GIF_AUTHOR_VIDEO_CLIPPER_H

#include "videoplayer.h"

#include <gtkmm/label.h>
#include <gtkmm/button.h>

/**
 * Widget to clip a video in time
 */
class VideoClipper : public VideoPlayer
{
    public:
        VideoClipper();
        virtual ~VideoClipper();

    protected: 
        void on_video_changed();
        void on_btn_start();
        void on_btn_end();
        void on_start_changed();
        void on_end_changed();
        void on_frame_changed(int64_t frame_index);

        Gtk::SpinButton w_spin_start, w_spin_end;
        Gtk::Label w_label_start, w_label_end;
        Gtk::Button w_btn_start, w_btn_end;
        Gtk::Grid w_grid;
};



#endif // GIF_AUTHOR_VIDEO_CLIPPER_H
