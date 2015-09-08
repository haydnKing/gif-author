#ifndef GIF_AUTHOR_IMAGE_AREA_H
#define GIF_AUTHOR_IMAGE_AREA_H

#include <stdint.h>
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

#include "videoframe.h"

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
        void update_image(Glib::RefPtr<VideoFrame> img);

    protected:
        //override default on_draw handler
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        Glib::RefPtr<Gdk::Pixbuf> orig_image, scaled_image;
        Glib::RefPtr<VideoFrame> the_frame;
        int last_width, last_height;
};

#endif
