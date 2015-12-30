#ifndef GIF_AUTHOR_HEADER_NAVIGATION_H
#define GIF_AUTHOR_HEADER_NAVIGATION_H

#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/stack.h>

class HeaderNavigation : public Gtk::HeaderBar
{
public:
    HeaderNavigation();
    virtual ~HeaderNavigation() {};

    void set_stack(Gtk::Stack& stack);

private:
    void on_stack_update();
    Gtk::Button w_left_btn, w_right_btn;

    Gtk::Stack* the_stack;
    sigc::connection conn;
};

#endif //GIF_AUTHOR_STACK_NAVIGATION_H
