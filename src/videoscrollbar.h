#ifndef GTKMM_GIFAUTHOR_VIDEOSCROLLBAR_H
#define GTKMM_GIFAUTHOR_VIDEOSCROLLBAR_H

#include <gtkmm/widget.h>
#include <string>

class VideoScrollbar : public Gtk::Widget
{
    public:
        VideoScrollbar();
        virtual ~VideoScrollbar();

        void set_frame_count(int64_t frame_count);
        int64_t get_frame_count() const;
        void set_current_frame(int64_t frame);

        //signals
        sigc::signal<void, int64_t> signal_frame_change();

    protected:

        //Overrides:
        virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
        virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
        virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
        virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
        virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
        virtual void on_size_allocate(Gtk::Allocation& allocation);
        virtual void on_map();
        virtual void on_unmap();
        virtual void on_realize();
        virtual void on_unrealize();
        virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        virtual bool on_button_press(GdkEventButton *evt);

        void draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
                       const std::string &str,
                       double x_pos, 
                       double y_pos);

        int64_t pixel_to_frame(double x_loc);

        Glib::RefPtr<Gdk::Window> m_refGdkWindow;

        int64_t frame_count, curr_frame;
        int64_t first_frame, frames_in_view;

        sigc::signal<void, int64_t> s_frame_change;
};

#endif //GTKMM_GIFAUTHOR_VIDEOSCROLLBAR_H
