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

void ImageArea::update_image(cv::Mat* image){
    if(image){
        orig_image = Gdk::Pixbuf::create_from_data(image->ptr(),
                                    Gdk::COLORSPACE_RGB,
                                    false,
                                    8,
                                    image->size().width,
                                    image->size().height,
                                    image->size().width * 3);
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

    //keep aspect the same
    float ratio_width  = (float) width  / (float) orig_image->get_width();
    float ratio_height = (float) height / (float) orig_image->get_height();
    if(ratio_width < ratio_height){
        height = ratio_width * orig_image->get_height();
    }
    else {
        width = ratio_height * orig_image->get_width();
    }
    
    if(width != last_width || height != last_height){
        scaled_image = orig_image->scale_simple(width,
                                                height,
                                                Gdk::INTERP_BILINEAR);
        last_width = width;
        last_height = height;
    }

    Gdk::Cairo::set_source_pixbuf(cr, scaled_image, 0, 0);
    cr->paint();

    return true;
}
