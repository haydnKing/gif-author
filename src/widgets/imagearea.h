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

        /**
         * Zoom out fully so that the whole image is displayed
         * @param redraw Whether or not to trigger a redraw immediately - 
         * leave false if the image is about to be updated
         */
        void reset_zoom(bool redraw=false);

    protected:
        //override default on_draw handler
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        virtual bool on_scroll_event(GdkEventScroll* scroll_event);

        void change_zoom(float factor, double x, double y);

        virtual bool on_button_press_event(GdkEventButton* button_event);
        virtual bool on_motion_notify_event(GdkEventMotion* motion_event);

        Glib::RefPtr<Gdk::Pixbuf> orig_image, scaled_image;
        pVideoFrame the_frame;
        float zoom;
        double x_off, y_off;
        int screen_x, screen_y;
        double last_x, last_y;
};

#endif
