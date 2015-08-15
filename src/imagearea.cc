#include "imagearea.h"
#include <cairomm/context.h>
#include <gdkmm/general.h>

ImageArea::ImageArea(int width, int height) : 
    last_width(-1),
    last_height(-1)
{
    set_size_request(width, height);

    orig_image = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,
                                     false,
                                     8,
                                     width,
                                     height);
    orig_image->fill(0x000000ff);
};

ImageArea::~ImageArea() {};

void ImageArea::update_image(uint8_t *data, int width, int height, int linesize){
    if(data){
        orig_image = Gdk::Pixbuf::create_from_data(data,
                                    Gdk::COLORSPACE_RGB,
                                    false,
                                    8,
                                    width,
                                    height,
                                    linesize);
    }
    else {
        orig_image->fill(0x000000ff);
    }
    last_width = last_height = -1;
    queue_draw();
}

bool ImageArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){
    Gtk::Allocation allocation = get_allocation();

    int width = allocation.get_width();
    int height = allocation.get_height();
    int x_off = 0, y_off = 0;

    //keep aspect the same
    float ratio_width  = (float) width  / (float) orig_image->get_width();
    float ratio_height = (float) height / (float) orig_image->get_height();
    if(ratio_width < ratio_height){
        height = ratio_width * orig_image->get_height();
        y_off = (allocation.get_height() - height)/2;
    }
    else {
        width = ratio_height * orig_image->get_width();
        x_off = (allocation.get_width() - width)/2;
    }
    
    if(width != last_width || height != last_height){
        scaled_image = orig_image->scale_simple(width,
                                                height,
                                                Gdk::INTERP_BILINEAR);
        last_width = width;
        last_height = height;
    }

    //Fill with black
    cr->set_source_rgb(0.,0.,0.);
    cr->rectangle(0,0, allocation.get_width(), allocation.get_height());
    cr->fill();
    
    //draw the image
    Gdk::Cairo::set_source_pixbuf(cr, scaled_image, x_off, y_off);
    cr->paint();

    return true;
}
