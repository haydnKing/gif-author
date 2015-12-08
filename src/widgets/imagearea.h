#ifndef GIF_AUTHOR_IMAGE_AREA_H
#define GIF_AUTHOR_IMAGE_AREA_H

#include <stdint.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

#include "../input/videoframe.h"

#include <iostream>

/**
 * Display an image, automatically scaled to the available space
 */
class ImageArea : public Gtk::DrawingArea
{
    public:
        /**
         * Default Constructor
         * \param width initial width of the image
         * \param height initial height of the image
         */
        ImageArea(int width=300, int height=200);
        virtual ~ImageArea();

        /**
         * Change the displayed image, automatically invoke a redraw
         * @param img The image to display
         */
        void update_image(pVideoFrame& img);

    protected:
        //override default on_draw handler
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        virtual bool on_scroll_event(GdkEventScroll* scroll_event);

        bool on_motion_notify_event(GdkEventMotion* motion_event);

        Glib::RefPtr<Gdk::Pixbuf> orig_image, scaled_image;
        pVideoFrame the_frame;
        int last_width, last_height;
};

#endif
