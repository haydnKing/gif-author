#ifndef GIF_AUTHOR_FRAME_BOX_H
#define GIF_AUTHOR_FRAME_BOX_H

#include <gtkmm/flowbox.h>
#include <gtkmm/drawingarea.h>

#include "../input/videoframe.h"

/**
 * visual representation of a Frame
 * Displays a thumbnail of the frame, scaled down by 2^scale,
 * surrounded by a filmstrip skeuomorph
 */
class FrameBox : public Gtk::DrawingArea
{
    public:
        FrameBox(pVideoFrame & _vf);
        virtual ~FrameBox();

        void set_scale(int _scale);
        int get_scale() const;
        void zoom_in();
        void zoom_out();

    protected:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& ctx);

        void update_thumbnail();

        pVideoFrame frame;
        Glib::RefPtr<Gdk::Pixbuf> thumb;
        int scale;
};

/**
 * display a series of FrameBoxes
 */
class FrameFlow : public Gtk::FlowBox
{
    public:
        FrameFlow();
        virtual ~FrameFlow();

    protected: 
};



#endif // GIF_AUTHOR_VIDEO_CLIPPER_H
