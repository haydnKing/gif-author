#ifndef GIF_AUTHOR_STACK_NAVIGATION_H
#define GIF_AUTHOR_STACK_NAVIGATION_H

#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/stack.h>

class StackNavigation : public Gtk::HeaderBar
{
public:
    StackNavigation();
    virtual ~StackNavigation() {};

private:
    Gtk::Button w_left_btn, w_right_btn;
    //Gtk::Label w_label;
};

#endif //GIF_AUTHOR_STACK_NAVIGATION_H
