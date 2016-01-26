#ifndef GIF_AUTHOR_FRAME_BOX_H
#define GIF_AUTHOR_FRAME_BOX_H

#include <gtkmm/flowbox.h>
#include <gtkmm/drawingarea.h>

#include "../input/videoframe.h"

/**
 * visual representation of a Frame
 */
class FrameStrip : public Gtk::DrawingArea
{
    public:
        FrameStrip(pVideoFrame & _vf);
        virtual ~FrameStrip();

    protected:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        pVideoFrame frame;

        int height;
};

/**
 * display a series of FrameStrips
 */
class FrameBox : public Gtk::FlowBox
{
    public:
        FrameBox();
        virtual ~FrameBox();

    protected: 
};



#endif // GIF_AUTHOR_VIDEO_CLIPPER_H
