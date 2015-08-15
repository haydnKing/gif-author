#ifndef GIF_AUTHOR_IMAGE_AREA_H
#define GIF_AUTHOR_IMAGE_AREA_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <opencv2/opencv.hpp>

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
         * The data in image should not be deleted until another image is 
         * provided.
         * Setting image=NULL will draw a blank image
         */
        void update_image(uint8_t *data, int width, int height, int linesize);

    protected:
        //override default on_draw handler
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        Glib::RefPtr<Gdk::Pixbuf> orig_image, scaled_image;
        int last_width, last_height;
};

#endif
