#ifndef GIFAUTHOR_PROCESS_H
#define GIFAUTHOR_PROCESS_H

#include <string>
#include <sstream>
#include <map>
#include <limits>
#include <iostream>
#include <memory>
#include <regex>

using namespace std;

class Size
{
    public:
        Size(int w, int h);
        virtual ~Size();

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
    if(size.width() < 0)
        os << "_x";
    else
        os << size.width() << "x";
    if(size.height() < 0)
        os << "_";
    else
        os << size.height();
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

        virtual string help() const = 0;
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

        virtual string help() const;
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
template <typename T> string Option<T>::help() const
{
    ostringstream out;
    out << "--" << my_name << "[=" << *my_value << "]: " << my_description;
    return out.str();
};
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

        virtual string help() const;
        
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


        string help();
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

template <typename T>
class FactoryOption : public OptionBase
{
    public:
        virtual ~FactoryOption() {};

        static pOption create(string name, string description, shared_ptr<T>& value);

        virtual string help() const;
        virtual void parse(vector<string>::const_iterator& it);

        void add_group(shared_ptr<T> group);

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
template <typename T> string FactoryOption<T>::help() const 
{
    ostringstream out;
    out << my_name << ": " << my_description << endl;
    for(auto it : groups)
    {
        out << it.second->help() << endl;
    }
    return out.str();
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
    args.erase(args.cbegin());
    group->parse(args, true);
};
template <typename T> void FactoryOption<T>::add_group(shared_ptr<T> group)
{
    groups[group->name()] = group;
};


#endif //GIFAUTHOR_PROCESS_H

