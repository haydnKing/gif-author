#include "cline.h"

vector<string> indent(const string& pre, const vector<string>& vs)
{
    vector<string> r;
    for(auto it : vs)
    {
        r.push_back(pre + it);
    }
    return r;
};
string word_wrap(const vector<string>& vs, int col_width)
{
    ostringstream out;
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
            out << line.substr(0, indent) << outline << std::endl;
            line_start += outline.size();
        }

    }
    return out.str();
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

template <> vector<string> Option<string>::help() const
{
    vector<string> r;
    ostringstream out;
    out << "--" << my_name;
    if(my_value->empty()) {
        out << "[=" << *my_value << "]";
    }
    r.push_back(out.str()); 
    r.push_back("  " + my_description);
    return r;
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
vector<string> Option<bool>::help() const
{
    vector<string> r;
    r.push_back("--" + my_name);
    r.push_back("  " + my_description);
    return r;
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

vector<string> OptionGroup::help()
{
    vector<string> r;
    r.push_back(my_name);
    r.push_back("  " + my_description);
    r.push_back("  options:");
    for(auto it = options.begin(); it != options.end(); ++it)
    {
        vector<string> h = indent("    ", it->second->help());
        r.insert(r.end(), h.begin(), h.end());
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

