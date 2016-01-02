#include "headernavigation.h"

Page::Page() :
    pt(PAGETYPE_NORMAL),
    completed(false),
    w(NULL)
{};

Glib::ustring Page::get_title() const
{
    return title;
};

void Page::set_title(const Glib::ustring& t)
{
    if(title != t)
    {
        title = t;
        s_title.emit(t);
    }
};

bool Page::is_completed() const
{
    return completed;
};

void Page::set_completed(bool c)
{
    if(completed != c)
    {
        completed = c;
        s_completed.emit(c);
    }
};

PageType Page::get_type() const
{
    return pt;
};

void Page::set_type(PageType type)
{
    pt = type;
};

sigc::signal<void, bool> Page::signal_completed_changed()
{
    return s_completed;
};

sigc::signal<void, Glib::ustring> Page::signal_title_changed()
{
    return s_title;
};

NavigationBar::NavigationBar()
{
    w_left_btn.set_label("Back");
    w_left_btn.set_halign(Gtk::ALIGN_START);
    w_right_btn.set_label("Next");
    w_right_btn.set_halign(Gtk::ALIGN_END);

    pack_start(w_left_btn);
    pack_end(w_right_btn);
};

void NavigationBar::set_page(Page& p)
{
    set_title(p.get_title());
    switch (p.get_type())
    {
        case PAGETYPE_FIRST:
            w_left_btn.set_sensitive(false);
            w_right_btn.set_label("Next");
            break;
        case PAGETYPE_LAST:
            w_left_btn.set_sensitive(true);
            w_right_btn.set_label("Finish");
            break;
        case PAGETYPE_NORMAL:
            w_left_btn.set_sensitive(true);
            w_right_btn.set_label("Next");
            break;
    }
    w_right_btn.set_sensitive(p.is_completed());
    //disconnect from the old page and connect to the new one
    conn.disconnect();
    conn = p.signal_completed_changed().connect(
            sigc::mem_fun(*this, &NavigationBar::on_completed_changed));
};

sigc::signal<void> NavigationBar::signal_left()
{
    return w_left_button.signal_clicked();
};

sigc::signal<void> NavigationBar::signal_right()
{
    return w_right_button.signal_clicked();
};
    
void NavigationBar::on_completed_changed(bool c)
{
    w_right_btn.set_sensitive(c);
};

SideBar::SideBar()
{};

void SideBar::add_page(Page& new_page)
{
    v_pages.push_back(&new_page);
    new_page.signal_completed_changed().connect(
            sigc::mem_fun(*this, &SideBar::on_completed_changed));
    Gtk::Label *label = new Gtk::Label(new_page.get_title());
    append(*label);
};

sigc::signal<void, Page*> SideBar::signal_page_selected()
{
    return s_page_selected;
};

void SideBar::on_row_selected(Gtk::ListBoxRow* row)
{
    if(row)
    {
        s_page_selected.emit(v_pages[row->get_index()]);
    }
};

void SideBar::on_completed_changed(bool)
{
    update_selectable();
};

void SideBar::update_selectable()
{
    //disable all pages after an incomplete page
    bool selectable = true;
    Gtk::ListBoxRow* lb;
    for(int i=0; i < v_pages.size(); i++)
    {
        get_row_at_index(i)->set_selectable(selectable);
        if(!v_pages[i]->is_completed()) selectable = false;
    }
};

