#include "framebox.h"

int next_root2(float val)
{
    int l = std::log(val)/std::log(2);
    //case if val is a power of 2
    if(float(std::pow(2,l)) == val)
        return l;
    return l+1;
};

FrameBox::FrameBox(pVideoFrame & _vf){
};

FrameBox::~FrameBox(){};

void FrameBox::set_scale(int _scale)
{
    scale = _scale;
    update_thumbnail();
};

int  FrameBox::get_scale() const
{
    return scale;
};

void FrameBox::zoom_in()
{
    if(scale != 0)
    {
        scale -= 1;
        update_thumbnail();
    }
};

void FrameBox::zoom_out()
{
    scale += 1;
    update_thumbnail();
};

bool FrameBox::on_draw(const Cairo::RefPtr<Cairo::Context>& ctx){
    Gtk::Allocation alloc = get_allocation();
    
    //scale the image down if the allocation is too small
    if(alloc.get_width() < thumb.get_width() ||
       alloc.get_height() - 2*BORDER_HEIGHT < thumb.get_height())
    {
        scale = std::max(
                    next_root2(frame.get_width() / alloc.get_width()),
                    next_root2(frame.get_width() /
                                (alloc.get_width()-2*BORDER_HEIGHT)));
        update_thumbnail();
    };

    //draw the image
    Gdk::Cairo::set_source_buffer(ctx, thumb, 0, BORDER_HEIGHT);
    ctx->paint();
    

};

void FrameBox::update_thumbnail()
{
    Gtk::RefPtr<Gdk::Pixbuf> orig = frame.get_pixbuf();
    thumb = orig->scale_simple(frame->get_width()/std::pow(2,scale),
                               frame->get_height()/std::pow(2,scale),
                               Gdk::INTERP_BILINEAR);
    queue_draw();
};
