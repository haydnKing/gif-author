#ifndef GIFAUTHOR_CLINE_H
#define GIFAUTHOR_CLINE_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

class BaseOption
{
    public:
        virtual ~BaseOption();

        virtual string help() const = 0;
        string name() const {return my_name;};
        string description() const {return my_description;};

        virtual void parse(vector<string>::const_iterator& it) = 0;

    protected:
        BaseOption(string name, string description);

        string my_name, my_description;
};

typedef shared_ptr<BaseOption> pOption;

template <typename T>
class Option : public BaseOption
{
    public:
        virtual ~Option();
        
        static pOption create(string name, string description, T& value);

        virtual string help() const;
        virtual void parse(vector<string>::const_iterator& it);

    protected:
        Option(string name, string description, T& value);

        T my_value;
};
template <typename T> Option<T>::Option(string name, string description, T& value) :
    BaseOption(name, description),
    my_value(value)
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
    out << "--" << my_name << "[=" << my_value << "]: " << my_description;
    return out.str();
};
template <typename T> void Option<T>::parse(vector<string>::const_iterator& it)
{
    //attempt to consume the next option
    it++;
    istringstream in(*it);
    in >> my_value;
    cout << "my_name = " << my_value << endl;
    it++;
};

template <>
class Option<bool> : public BaseOption
{
    public:
        virtual ~Option();
        
        static pOption create(string name, string description, bool& value);

        virtual string help() const;
        
        virtual void parse(vector<string>::const_iterator& it);

    protected:
        Option(string name, string description, bool& value);

        bool my_value;
};

class OptionGroup;
typedef shared_ptr<OptionGroup> pOptionGroup;

class OptionGroup
{
    public:
        virtual ~OptionGroup();

        static pOptionGroup create(string name);

        template <typename T> void add_option(string name, string description, T& value);

        template <typename T> T value(string name);

        string help();

        //parse the arguments, return vector of unused arguments
        vector<string> parse(const vector<string>& args);

    protected:
        OptionGroup(string name);

        string my_name;
        map<string, pOption> options;
};
template <typename T> void OptionGroup::add_option(string name, string description, T& value)
{
    pOption op = Option<T>::create(name, description, value);
    options[name] = op;
};


#endif //GIFAUTHOR_CLINE_H

