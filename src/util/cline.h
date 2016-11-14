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
        
        static pOption create(string name, string description, const T& default_value);

        virtual string help() const;

    protected:
        Option(string name, string description, const T& default_value);

        T my_value;
};
template <typename T> Option<T>::Option(string name, string description, const T& default_value) :
    BaseOption(name, description),
    my_value(default_value)
{};
template <typename T> Option<T>::~Option()
{};
template <typename T> pOption Option<T>::create(string name, string description, const T& default_value)
{
    return pOption(new Option<T>(name, description, default_value));
};
template <typename T> string Option<T>::help() const
{
    ostringstream out;
    out << "--" << my_name << "[=" << my_value << "]: " << my_description;
    return out.str();
};

class OptionGroup;
typedef shared_ptr<OptionGroup> pOptionGroup;

class OptionGroup
{
    public:
        virtual ~OptionGroup();

        static pOptionGroup create(string name);

        template <typename T> void add_option(string name, string description, const T& default_value);

        string get_help();

    protected:
        OptionGroup(string name);

        string my_name;
        map<string, pOption> options;
};
template <typename T> void OptionGroup::add_option<T>(string name, string description, const T& default_value)
{
    pOption op = Option<T>::create(name, description, default_value);
    options[name] = op;
};



#endif //GIFAUTHOR_CLINE_H

