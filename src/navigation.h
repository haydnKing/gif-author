#ifndef GIF_AUTHOR_HEADER_NAVIGATION_H
#define GIF_AUTHOR_HEADER_NAVIGATION_H

#include <gtkmm/button.h>
#include <gtkmm/headerbar.h>
#include <gtkmm/label.h>
#include <gtkmm/stack.h>
#include <gtkmm/listbox.h>

#include <vector>

enum PageType {
    PAGETYPE_FIRST,
    PAGETYPE_NORMAL,
    PAGETYPE_LAST
};

class Page
{
    public:
        Page();
        virtual ~Page() {};

        virtual Gtk::Widget* get_widget() = 0;

        Glib::ustring get_title() const;
        void set_title(const Glib::ustring& t);

        bool is_completed() const;
        void set_completed(bool c=true);

        PageType get_type() const;

        sigc::signal<void, bool> signal_completed_changed();
        sigc::signal<void, Glib::ustring> signal_title_changed();

    protected:
        void set_type(PageType type);

    private:
        PageType pt;
        bool completed;
        Glib::ustring title;
        sigc::signal<void, bool> s_completed;
        sigc::signal<void, Glib::ustring> s_title;
};

class NavigationBar : public Gtk::HeaderBar
{
public:
    NavigationBar();
    virtual ~NavigationBar() {};

    void set_page(Page& p);

    sigc::signal<void> signal_left();
    sigc::signal<void> signal_right();

private:
    void on_completed_changed(bool c);
    Gtk::Button w_left_btn, w_right_btn;
    sigc::connection conn;
};

class SideBar : public Gtk::ListBox
{
    public:
        SideBar();
        virtual ~SideBar() {};

        void add_page(Page& new_page);
        void select_page(Page* page);

        sigc::signal<void, Page*> signal_user_selected_page();

    private:
        void on_row_selected(Gtk::ListBoxRow* row);
        void on_completed_changed(bool c);
        void update_selectable();

        std::vector<Page*> v_pages;
        sigc::signal<void, Page*> s_page_selected;
};

class Wizzard : public Gtk::Window
{
    public:
        Wizzard();
        virtual ~Wizzard();

        void add_page(Page& new_page);
        void select_page(Page* page);

    private:
        void on_next();
        void on_prev();

        int index_of(Page* p);

        NavigationBar w_nav;
        SideBar w_side;
        Gtk::Stack w_stack;
        Gtk::Grid w_grid;

        std::vector<Page*> my_pages;
        Page* curr_page;
};

#endif //GIF_AUTHOR_STACK_NAVIGATION_H
