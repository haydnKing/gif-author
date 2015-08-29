#include "videoscrollbar.h"

#include <gdkmm/general.h>
#include <gtkmm/cssprovider.h>

#include <iostream>
#include <sstream>

const int VIDEOSCROLLBAR_HEIGHT = 70;
const int VIDEOSCROLLBAR_WIDTH = 200;

std::string convertInt(int number);

VideoScrollbar::VideoScrollbar():
    Glib::ObjectBase("videoscrollbar"),
    Gtk::Widget(),
    frame_count(100),
    curr_frame(0)
{
    set_has_window(true);
    
    set_vexpand(false);
    set_hexpand(true);
}

VideoScrollbar::~VideoScrollbar() {};

void VideoScrollbar::set_frame_count(int64_t _frame_count)
{
    frame_count = _frame_count;
    queue_draw();
}

void VideoScrollbar::set_current_frame(int64_t frame)
{
    curr_frame = frame;
    queue_draw();
}

int64_t VideoScrollbar::get_frame_count() const
{
    return frame_count;
}

Gtk::SizeRequestMode VideoScrollbar::get_request_mode_vfunc() const
{
    //default value
    return Gtk::Widget::get_request_mode_vfunc();
}

void VideoScrollbar::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
    minimum_width = VIDEOSCROLLBAR_WIDTH;
    natural_width = VIDEOSCROLLBAR_WIDTH;
}

void VideoScrollbar::get_preferred_height_for_width_vfunc(int /* width */,
        int& minimum_height, int& natural_height) const
{
    minimum_height = VIDEOSCROLLBAR_HEIGHT;
    natural_height = VIDEOSCROLLBAR_HEIGHT;
}

void VideoScrollbar::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
    minimum_height = VIDEOSCROLLBAR_HEIGHT;
    natural_height = VIDEOSCROLLBAR_HEIGHT;
}

void VideoScrollbar::get_preferred_width_for_height_vfunc(int /* height */,
        int& minimum_width, int& natural_width) const
{
    minimum_width = VIDEOSCROLLBAR_WIDTH;
    natural_width = VIDEOSCROLLBAR_WIDTH;
}

void VideoScrollbar::on_size_allocate(Gtk::Allocation& allocation)
{
    set_allocation(allocation);

    //Use the offered allocation for this container:
    set_allocation(allocation);

    if(m_refGdkWindow)
    {
        m_refGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
                allocation.get_width(), allocation.get_height() );
    }
}

void VideoScrollbar::on_map()
{
    //Call base class:
    Gtk::Widget::on_map();
}

void VideoScrollbar::on_unmap()
{
    //Call base class:
    Gtk::Widget::on_unmap();
}

void VideoScrollbar::on_realize()
{
    //Do not call base class Gtk::Widget::on_realize().
    //It's intended only for widgets that set_has_window(false).

    set_realized();

    if(!m_refGdkWindow)
    {
        //Create the GdkWindow:

        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //Set initial position and size of the Gdk::Window:
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes,
                GDK_WA_X | GDK_WA_Y);
        set_window(m_refGdkWindow);

        //set colors
        //override_background_color(Gdk::RGBA("red"));
        //override_color(Gdk::RGBA("blue"));

        //make the widget receive expose events
        m_refGdkWindow->set_user_data(gobj());
    }
}

void VideoScrollbar::on_unrealize()
{
    m_refGdkWindow.reset();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

bool VideoScrollbar::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    const double TEXT_HEIGHT = 20.;
    const double PAD = 5.;
    double x = 0;
    double y = 0;
    double w = (double)get_allocation().get_width();
    double h = (double)get_allocation().get_height();
    Glib::RefPtr<Gtk::StyleContext> context = get_style_context();

    //get border
    Gtk::Border border = context->get_border(); 

    w -= border.get_left() + border.get_right();
    h -= border.get_top() + border.get_bottom();
    x += border.get_left();
    y += border.get_top();


    // paint the view background
    context->context_save();
    context->add_class(GTK_STYLE_CLASS_ENTRY);
    context->render_background(cr, x,y,w,h-TEXT_HEIGHT);
    context->context_restore();

    // paint the scale background
    context->context_save();
    context->add_class(GTK_STYLE_CLASS_BUTTON);
    context->render_background(cr, x, y + h-TEXT_HEIGHT, w, TEXT_HEIGHT);
    context->render_frame(cr, x,y,w,h);
    context->context_restore();
    
    // draw a line separating the view and the scale
    context->context_save();
    context->add_class(GTK_STYLE_CLASS_PANE_SEPARATOR);
    Gdk::Cairo::set_source_rgba(cr, context->get_border_color(context->get_state()));
    cr->set_line_width(1.0);
    cr->move_to(x+1, 0.5+std::floor(y+h-TEXT_HEIGHT));
    cr->line_to(w-1, 0.5+std::floor(y+h-TEXT_HEIGHT));
    cr->stroke();
    context->context_restore();

    
    //temp
    int64_t frames_in_view = frame_count;
    int64_t first_frame = 1;
    
    //Work out where to put marks
    double px_per_frame = (w-2*PAD) / frames_in_view;
    int64_t frames_per_mark = static_cast<int64_t>(50. / px_per_frame);
    
    double rm = std::log10(frames_per_mark) - std::floor(std::log10(frames_per_mark));
    frames_per_mark = std::pow(10, std::floor(std::log10(frames_per_mark)));
    if(rm <= std::log10(2))
    {
        frames_per_mark *= 2;
    }
    else if(rm <= std::log10(5))
    {
        frames_per_mark *= 5;
    }
    else if(rm <= std::log10(10))
    {
        frames_per_mark *= 10;
    }
    
    //draw the marks
    cr->set_line_width(0.5);
    int64_t frame = ((first_frame / frames_per_mark)+1) * frames_per_mark;
    std::ostringstream convert;
    for(;frame < first_frame + frames_in_view; frame += frames_per_mark){
        context->render_line(cr,
                             std::floor(PAD+(frame-first_frame)*px_per_frame)+0.5, y+2,
                             std::floor(PAD+(frame-first_frame)*px_per_frame)+0.5, h-TEXT_HEIGHT);
        context->context_save();
        context->add_class(GTK_STYLE_CLASS_BUTTON);
        Gdk::Cairo::set_source_rgba(cr, context->get_color(context->get_state()));
        draw_text(cr, convertInt(frame), PAD+(frame-first_frame)*px_per_frame, h-0.5*TEXT_HEIGHT); 
        context->context_restore();
    }


    if(curr_frame > first_frame && (curr_frame-first_frame) < frames_in_view)
    {
        cr->set_source_rgb(0.0,1.0,0.0);
        cr->set_line_width(1.);
        cr->move_to(PAD+(curr_frame - first_frame)*px_per_frame, y+2);
        cr->line_to(PAD+(curr_frame - first_frame)*px_per_frame, h-TEXT_HEIGHT);
        cr->stroke();
    }

    return true;
}
        

void VideoScrollbar::draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
                        const std::string &str,
                        double x_pos, 
                        double y_pos)
{
  // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
  Pango::FontDescription font;

  font.set_family("Monospace");

  // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
  Glib::RefPtr<Pango::Layout> layout = create_pango_layout(str);

  layout->set_font_description(font);

  int text_width;
  int text_height;

  //get the text dimensions (it updates the variables -- by reference)
  layout->get_pixel_size(text_width, text_height);

  // Position the text in the middle
  cr->move_to(x_pos-text_width/2, y_pos - text_height/2);

  layout->show_in_cairo_context(cr);
}

std::string convertInt(int number)
{
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}
