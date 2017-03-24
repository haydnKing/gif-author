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
void word_wrap(const string& line, int col_width, vector<string>& out)
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
vector<string> word_wrap(const string& s, int len)
{
    vector<string> out;
    word_wrap(s, len, out);
    return out;
};

Size::Size(int w, int h):
    w(w),
    h(h)
{};

Size::~Size() {};

OptionBase::OptionBase(string name, string description, char short_name) :
    my_name(name),
    my_description(description),
    my_short(short_name)
{};

OptionBase::~OptionBase()
{};
        
vector<string> OptionBase::format_description(int width) const 
{
    return word_wrap(my_description, width);
};

template <> string Option<string>::title(const string& mark, const string& shortmark) const
{
    ostringstream out;
    if(short_name() != '\0')
        out << shortmark << short_name() << ", ";
    out << mark << my_name;
    if(!my_value->empty()) {
        out << "[=" << *my_value << "]";
    }
    return out.str();
};

Option<bool>::Option(string name, string description, bool& value, char short_name) :
    OptionBase(name, description, short_name),
    my_value(&value)
{
    *my_value = false;
};
Option<bool>::~Option()
{};
pOption Option<bool>::create(string name, string description, bool& value, char short_name)
{
    return pOption(new Option<bool>(name, description, value, short_name));
};
string Option<bool>::title(const string& mark, const string& shortmark) const
{
    if(short_name() != '\0')
        return shortmark + short_name() + ", " + mark + my_name;
    return mark + my_name;
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

vector<string> OptionGroup::format_help(int width, const string& mark)
{
    vector<string> r, d;
    string title;
    int longest_title = 0;
    //r.push_back(my_name);
    //r.push_back(my_description);
    //r.push_back("  options:");
    for(auto it: options)
    {
        title = it.second->title(mark);
        if(title.size() > longest_title)
            longest_title = title.size();
    }
    for(auto it: options)
    {
        d = it.second->format_description(width-longest_title-1);
        d = indent(longest_title + 1, d);
        title = it.second->title(mark);
        d[0].replace(0, title.size(), title);
        r.insert(r.end(), d.begin(), d.end());
    }
    r = indent(2, r);
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
        }
        catch (out_of_range) {
            throw UnknownArgument(name);
            it++;
        }            
        op->parse(it);
    }

    return ret;
};

