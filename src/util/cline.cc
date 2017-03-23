#include "cline.h"

vector<string> indent(int spaces, const vector<string>& vs)
{
    vector<string> r;
    for(auto it : vs)
    {
        r.push_back(pre + it);
    }
    return r;
};
vector<string> word_wrap(const vector<string>& vs, int col_width)
{
    vector<string> out;
    string outline;
    int line_start, line_width, indent, last_space;

    for(auto line : vs)
    {
        if(line.size() <= col_width)
        {
            out << line << endl;
            continue;
        }
        //find the indent
        for(indent = 0; indent < line.size(); indent++)
        {
            if(!(line[indent] == ' ' || line[indent] == '\t'))
                break;
        }
        //first wrapped line starts at indent
        line_start = indent;
        //beware long indents
        if(indent >= col_width)
            indent = 0;

        while(line_start < line.size())
        {
            outline = line.substr(line_start, col_width-indent);
            if(indent+outline.size() >= col_width)
            {
                last_space = outline.find_last_of(" ");
                //if we need to line break
                if(last_space == string::npos)
                {
                    outline = outline.substr(0, col_width-indent-1);
                    outline.append("-");
                } else
                {
                    outline = outline.substr(0, last_space);
                    //don't include the space on the next line
                    line_start += 1;
                }
            }
            out.push_back(line.substr(0, indent) + outline);
            line_start += outline.size();
        }

    }
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
        
vector<string> OptionBase::description(int width) const 
{
    vector<string> r;
    r.push_back(my_description);
    return word_wrap(r, width);
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

