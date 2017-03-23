#include "cline.h"

vector<string> indent(int spaces, const vector<string>& vs)
{
    string pre;
    pre.reserve(spaces);
    for(int i = 0; i < spaces; i++)
        pre.push_back(' ');
    vector<string> r;
    for(auto it : vs)
    {
        r.push_back(pre + it);
    }
    return r;
};
void word_wrap(const string line, int col_width, vector<string>& out)
{
    string outline;
    int line_start=0, line_width=0, last_space=0;

    //trivial
    if(line.size() <= col_width)
    {
        out.push_back(line);
        return;
    }

    while(line_start < line.size())
    {
        outline = line.substr(line_start, col_width);
        if(outline.size() < col_width)
        {
            out.push_back(outline);
            break;
        }
        last_space = outline.find_last_of(" ");
        if(last_space == string::npos)
        {
            *outline.end() = '-';
            line_start += col_width-1;
        } else {
            outline = outline.substr(0, last_space);
            line_start += last_space + 1;
        }
        out.push_back(outline);
    }

};
vector<string> word_wrap(const vector<string>& vs, int len)
{
    vector<string> out;
    word_wrap(vs, len, out);
    return out;
};

Size::Size(int w, int h):
    w(w),
    h(h)
{};

Size::~Size() {};

OptionBase::OptionBase(string name, string description) :
    my_name(name),
    my_description(description)
{};

OptionBase::~OptionBase()
{};
        
vector<string> OptionBase::format_description(int width) const 
{
    return word_wrap(my_description, width);
};

template <> string Option<string>::title() const
{
    ostringstream out;
    out << "--" << my_name;
    if(!my_value->empty()) {
        out << "[=" << *my_value << "]";
    }
    return out.str();
};

Option<bool>::Option(string name, string description, bool& value) :
    OptionBase(name, description),
    my_value(&value)
{
    *my_value = false;
};
Option<bool>::~Option()
{};
pOption Option<bool>::create(string name, string description, bool& value)
{
    return pOption(new Option<bool>(name, description, value));
};
string Option<bool>::title() const
{
    return "--" + my_name;
};
void Option<bool>::parse(vector<string>::const_iterator& it)
{
    *my_value = true;
    it++;
};


OptionGroup::OptionGroup(string name, string description) :
    my_name(name),
    my_description(description)
{};

OptionGroup::~OptionGroup()
{};

pOptionGroup OptionGroup::create(string name, string description)
{
    return pOptionGroup(new OptionGroup(name, description));
};
        
void OptionGroup::add_option(pOption op)
{
    options[op->name()] = op;
};

vector<string> OptionGroup::format_help(int width)
{
    vector<string> r, desc;
    string title;
    int longest_title = 0;
    //r.push_back(my_name);
    //r.push_back(my_description);
    //r.push_back("  options:");
    for(auto it: options)
    {
        title = it.second->title();
        if(title.size() > longest_title)
            longest_title = title.size();
    }
    for(auto it: options)
    {
        desc = it.second->desription(width-longest_title-1);
        
    }
    return r;
};

vector<string> OptionGroup::parse(const vector<string>& args, bool shortform)
{
    vector<string> ret;
    vector<string>::const_iterator it = args.cbegin();
    string name;
    pOption op;

    while(it != args.end())
    {
        name = "";
        if(shortform)
        {
            name = *it;
        } else {
            if(it->compare(0, 2, "--") == 0)
            {
                name = it->substr(2);
            } else if(it->compare(0, 1, "-") == 0) {
                name = it->substr(1);
            } else {
                ret.push_back(*it);
                it++;
                continue;
            }
        }
        
        //check "=" form
        int eq = name.find("=");
        if(eq != string::npos)
        {
            name = name.substr(0,eq);
        }
        
        try
        {
            op = options.at(name);
            op->parse(it);
        }
        catch (out_of_range) {
            ret.push_back(*it);
            it++;
        }            
    }

    return ret;
};

