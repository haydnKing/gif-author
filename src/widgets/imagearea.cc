#include "imagearea.h"
#include <cairomm/context.h>
#include <gdkmm/general.h>

ImageArea::ImageArea(int width, int height) :
    x_off(0),
    y_off(0),
    zoom(0),
    old_w(width),
    old_h(height)
{
    add_events(Gdk::SCROLL_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
    set_size_request(width, height);

    orig_image = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
                                     false,
                                     8,
                                     width,
                                     height);
    orig_image->fill(0x000000ff);

};

ImageArea::~ImageArea() {};

void ImageArea::update_image(pVideoFrame& img){
    if(img->is_ok()){
        //keep a reference to the image so that the data doesn't go out of scope
        the_frame = img;
        orig_image = Gdk::Pixbuf::create_from_data(img->get_data(),
                                    Gdk::COLORSPACE_RGB,
                                    false,
                                    8,
                                    img->get_width(),
                                    img->get_height(),
                                    img->get_rowstride());
    }
    else {
        orig_image->fill(0x000000ff);
    }
    queue_draw();
}

void ImageArea::reset_zoom(bool redraw){
    zoom = 0;
    if(redraw)
        queue_draw();
}

bool ImageArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
    Gtk::Allocation allocation = get_allocation();

    //get screen width and height
    int screen_w = allocation.get_width();
    int screen_h = allocation.get_height();

    //position of image in screen (aka thickness of black bars)
    screen_x = 0;
    screen_y = 0;

    //adjust zoom if window changed size
    zoom *= std::min((double)screen_w/old_w, (double)screen_h/old_h);
    old_w = screen_w;
    old_h = screen_h;

    //set minimum zoom
    float zoom_min = std::min(((float)screen_w)/orig_image->get_width(),
                              ((float)screen_h)/orig_image->get_height());
    zoom = std::max(zoom, zoom_min);

    //source width and height rounded to nearest int
    int src_h = (int) (0.5 + (float)screen_h / zoom),
        src_w = (int) (0.5 + (float)screen_w / zoom);
    if(src_h > orig_image->get_height()){
        src_h = orig_image->get_height();
        screen_y = (int) (0.5 + 0.5 * (screen_h - src_h * zoom));
    }
    if(src_w > orig_image->get_width()){
        src_w = orig_image->get_width();
        screen_x = (int) (0.5 + 0.5 * (screen_w - src_w * zoom));
    }
    x_off = std::min(std::max(x_off,0.),(double)(orig_image->get_width()-src_w));
    y_off = std::min(std::max(y_off,0.),(double)(orig_image->get_height()-src_h));

    //crop

    Glib::RefPtr<Gdk::Pixbuf> cropped = Gdk::Pixbuf::create_subpixbuf(
            orig_image,
            x_off,
            y_off,
            src_w,
            src_h);


    //scale
    scaled_image = cropped->scale_simple(screen_w - 2*screen_x,
                                         screen_h - 2*screen_y,
                                         Gdk::INTERP_BILINEAR);

    //if black bars are visible
    if(screen_x > 0 || screen_y > 0){
        //Fill with black
        cr->set_source_rgb(0.,0.,0.);
        cr->rectangle(0,
                      0,
                      allocation.get_width(),
                      allocation.get_height());
        cr->fill();
    }
    
    //draw the image
    Gdk::Cairo::set_source_pixbuf(cr, scaled_image, screen_x, screen_y);
    cr->paint();

    return true;
}
        
bool ImageArea::on_scroll_event(GdkEventScroll* ev){
    switch(ev->direction){
        case GDK_SCROLL_UP:
            change_zoom(1.25, ev->x, ev->y);
            break;
        case GDK_SCROLL_DOWN:
            change_zoom(0.8, ev->x, ev->y);
            break;
        default:
            break;
    }
    return true;
};
        
void ImageArea::change_zoom(float factor, double x, double y){
    x_off += (factor*(x-screen_x)-x) / (factor*zoom);
    y_off += (factor*(y-screen_y)-y) / (factor*zoom);
    zoom *= factor;
    queue_draw();
};
bool ImageArea::on_button_press_event(GdkEventButton* ev){
    if(ev->button == 1){
        last_x = ev->x;
        last_y = ev->y;
    }
};

bool ImageArea::on_motion_notify_event(GdkEventMotion* ev){
    if(ev->state & GDK_BUTTON1_MASK){
        x_off -= (ev->x - last_x)/zoom;
        y_off -= (ev->y - last_y)/zoom;
        last_x = ev->x;
        last_y = ev->y;
        queue_draw();
    }
    return true;
};

