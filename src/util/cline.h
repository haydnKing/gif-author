#ifndef GIFAUTHOR_PROCESS_H
#define GIFAUTHOR_PROCESS_H

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <limits>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

vector<string> indent(int spaces, const vector<string>& vs);
vector<string> word_wrap(const string& s, int len);
void word_wrap(const string& s, int len, vector<string>& out);

class Size
{
    public:
        Size(int w=-1, int h=-1);
        virtual ~Size();

        operator bool() const {return w>=0 || h>=0;};

        int width() const {return w;};
        int height() const {return h;};

        void width(int _w) {w = _w;};
        void height(int _h) {h = _h;};

    protected:
        int w,h;
};

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& 
    operator<<(std::basic_ostream<CharT, Traits>& os, 
               const Size& size)
{
    os << size.width() << "x" << size.height();
    return os;
};

template <class CharT, class Traits>
std::basic_istream<CharT, Traits>& 
    operator>>(std::basic_istream<CharT, Traits>& is, 
               Size& size)
{
    int w,h;
    string s;
    is >> s;

    regex re("([0-9]+|_)x([0-9]+|_)");
    smatch m;
    if(!regex_match(s, m, re))
    {
        //bad stuff
        is.setstate(std::istream::failbit);
        return is;
    }

    if(m.str(1) != "_")
    {
        w = stoi(m.str(1));
    } else {
        w = -1;
    }
    if(m.str(2) != "_")
    {
        h = stoi(m.str(2));
    } else {
        h = -1;
    }
    size.width(w);
    size.height(h);
    return is;
};

class Crop
{
    public:
        Crop(int x=-1, int y=-1, int w=-1, int h=-1) :
            x(x),
            y(y),
            w(w),
            h(h)
        {};
        virtual ~Crop() {};

        int xpos() const {return x;};
        int ypos() const {return y;};
        int width() const {return w;};
        int height() const {return h;};

        operator bool() const {return !(x<0 && y<0 && w<0 && h<0);};

        void xpos(int _x) {x = _x;};
        void ypos(int _y) {y = _y;};
        void width(int _w) {w = _w;};
        void height(int _h) {h = _h;};

    protected:
        int x,y,w,h;
};

template <class CharT, class Traits>
std::basic_istream<CharT, Traits>& 
    operator>>(std::basic_istream<CharT, Traits>& is, 
               Crop& c)
{
    string s;
    is >> s;

    regex re("([0-9]+),([0-9]+)+([0-9]+)x([0-9]+)");
    smatch m;
    if(!regex_match(s, m, re))
    {
        //bad stuff
        is.setstate(std::istream::failbit);
        return is;
    }

    c.xpos(stoi(m.str(1)));
    c.ypos(stoi(m.str(2)));
    c.width(stoi(m.str(3)));
    c.height(stoi(m.str(4)));
    return is;
};

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& 
    operator<<(std::basic_ostream<CharT, Traits>& os, 
               const Crop& c)
{
    os << c.xpos() << "," << c.ypos() << "+" << c.width() << "x" << c.height();
    return os;
};

class OptionBase
{
    public:
        virtual ~OptionBase();

        string name() const {return my_name;};
        string description() const {return my_description;};

        virtual string title() const = 0;
        virtual vector<string> format_description(int width) const;
        virtual void parse(vector<string>::const_iterator& it) = 0;

    protected:
        OptionBase(string name, string description);

        string my_name, my_description;
};

typedef shared_ptr<OptionBase> pOption;

template <typename T>
class Option : public OptionBase
{
    public:
        virtual ~Option();
        
        static pOption create(string name, string description, T& value);

        virtual string title() const;
        virtual void parse(vector<string>::const_iterator& it);

    protected:
        Option(string name, string description, T& value);

        T* my_value;
};
template <typename T> Option<T>::Option(string name, string description, T& value) :
    OptionBase(name, description),
    my_value(&value)
{};
template <typename T> Option<T>::~Option()
{};
template <typename T> pOption Option<T>::create(string name, string description, T& value)
{
    return pOption(new Option<T>(name, description, value));
};
template <typename T> string Option<T>::title() const
{
    ostringstream out;
    out << "--" << my_name;
    if((bool)*my_value) {
        out << "[=" << *my_value << "]";
    }
    return out.str();
};
template <> string Option<string>::title() const;
template <typename T> void Option<T>::parse(vector<string>::const_iterator& it)
{
    int eq = it->find("=");
    string rvalue;
    if(eq == string::npos)
    {
        //attempt to consume the next option
        it++;
        rvalue = *it;
    } else {
        rvalue = it->substr(eq+1);
    }
    it++;
    istringstream in(rvalue);
    in >> *my_value;
    bool fail = in.fail();
    string rem;
    getline(in, rem);
    if(fail || !rem.empty())
    {
        ostringstream err;
        err << "Invalid argument \""<< rvalue << "\" to \"" << my_name << "\"";
        throw(std::invalid_argument(err.str()));
    }
};

template <>
class Option<bool> : public OptionBase
{
    public:
        virtual ~Option();
        
        static pOption create(string name, string description, bool& value);

        virtual string title() const;
        
        virtual void parse(vector<string>::const_iterator& it);

    protected:
        Option(string name, string description, bool& value);

        bool* my_value;
};

class OptionGroup;
typedef shared_ptr<OptionGroup> pOptionGroup;

class OptionGroup
{
    public:
        virtual ~OptionGroup();

        static pOptionGroup create(string name, string description="");

        template <typename T> void add_option(string name, string description, T& value);
        void add_option(pOption op);


        vector<string> format_help(int width);

        string name() const {return my_name;};
        string description() const {return my_description;};

        //parse the arguments, return vector of unused arguments
        vector<string> parse(const vector<string>& args, bool shortform=false);

    protected:
        OptionGroup(string name, string description);

        string my_name, my_description;
        map<string, pOption> options;
};
template <typename T> void OptionGroup::add_option(string name, string description, T& value)
{
    pOption op = Option<T>::create(name, description, value);
    options[name] = op;
};
template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& 
    operator<<(std::basic_ostream<CharT, Traits>& os, 
               const pOptionGroup& og)
{
    for(auto it : og->format_help(80))
        os << it << endl;
    return os;
};

template <typename T>
class FactoryOption : public OptionBase
{
    public:
        virtual ~FactoryOption() {};

        static pOption create(string name, string description, shared_ptr<T>& value);

        virtual string title() const;
        virtual vector<string> format_description(int width) const;
        virtual void parse(vector<string>::const_iterator& it);

        void add_group(shared_ptr<T> group);

        void set_default(string name);

    protected:
        FactoryOption(string name, string description, shared_ptr<T>& value);
        map<string, shared_ptr<T>> groups;

        shared_ptr<T>* value;
};
template <typename T> FactoryOption<T>::FactoryOption(string name, string description, shared_ptr<T>& value):
    OptionBase(name, description),
    value(&value)
{};
template <typename T> pOption FactoryOption<T>::create(string name, string description, shared_ptr<T>& value)
{
    return pOption(new FactoryOption<T>(name, description, value));
};
template <typename T> string FactoryOption<T>::title() const 
{
    ostringstream out;
    out << "--" << my_name;
    if((bool)*value)    
        out << "[=" << (*value)->name() << "]";
    return out.str();
};
template <typename T> vector<string> FactoryOption<T>::format_description(int width) const 
{
    vector<string> r, d;
    int longest_name=0;
    string name;
    d = word_wrap(description(), width);
    r.insert(r.end(), d.begin(), d.end());
    r.push_back("Valid options:");
    for(auto it : groups)
    {
        if(it.second->name().size() > longest_name)
            longest_name = it.second->name().size();
    }
    for(auto it : groups)
    {
        d = word_wrap(it.second->description(), (width-longest_name-1));
        d = indent(longest_name+3, d);
        name = it.second->name();
        d[0] = "\"" + name + "\"" + d[0].substr(name.size());
        r.insert(r.end(), d.begin(), d.end());
    }

    return r;
};
template <typename T> void FactoryOption<T>::parse(vector<string>::const_iterator& it)
{
    it++;
    stringstream in(*it);
    it++;

    //get tokens
    vector<string> args;
    string val;
    //char delim = ':';
    while(getline(in, val, ':'))
    {
        args.push_back(val);
    }

    shared_ptr<T> group = groups.at(args[0]);
    *value = group;
    args.erase(args.cbegin());
    group->parse(args, true);
};
template <typename T> void FactoryOption<T>::add_group(shared_ptr<T> group)
{
    groups[group->name()] = group;
};
template <typename T> void FactoryOption<T>::set_default(string name)
{
    *value = groups.at(name);
};


#endif //GIFAUTHOR_PROCESS_H

