#include "stacknavigation.h"

StackNavigation::StackNavigation() :
    the_stack(NULL)
{
    w_left_btn.set_label("Previous");
    w_left_btn.set_halign(Gtk::ALIGN_START);
    set_title("Title");
    w_right_btn.set_label("Next");
    w_right_btn.set_halign(Gtk::ALIGN_END);

    pack_start(w_left_btn);
    pack_end(w_right_btn);
};

void StackNavigation::set_stack(Gtk::Stack& stack)
{
    the_stack = &stack;
    //on_stack_update(stack.get_visible_child());
    //disconnect from any previous stack
    conn.disconnect();
    //connect to the new one
    conn = stack.connect_property_changed_with_return(
            "visible-child",
            sigc::mem_fun(*this, &StackNavigation::on_stack_update));
};
    
void StackNavigation::on_stack_update()
{
    Gtk::Widget* w = the_stack->get_visible_child();
    if(w)
    {
        set_title(the_stack->child_property_title(*w));
    }
};
