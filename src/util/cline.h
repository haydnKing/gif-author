#ifndef GIFAUTHOR_CLINE_H
#define GIFAUTHOR_CLINE_H

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

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
    cout << "parsed " << my_name << " = " << *my_value << endl;
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


#endif //GIFAUTHOR_CLINE_H

