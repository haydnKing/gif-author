#include "cline.h"

BaseOption::BaseOption(string name, string description) :
    my_name(name),
    my_description(description)
{};

BaseOption::~BaseOption()
{};

Option<bool>::Option(string name, string description) :
    BaseOption(name, description),
    my_value(false)
{};
Option<bool>::~Option()
{};
pOption Option<bool>::create(string name, string description)
{
    return pOption(new Option<bool>(name, description));
};
string Option<bool>::help() const
{
    ostringstream out;
    out << "--" << my_name << ": " << my_description;
    return out.str();
};


OptionGroup::OptionGroup(string name) :
    my_name(name)
{};

OptionGroup::~OptionGroup()
{};

pOptionGroup OptionGroup::create(string name)
{
    return pOptionGroup(new OptionGroup(name));
};

string OptionGroup::help()
{
    ostringstream out;
    out << my_name << " options:\n";
    for(auto it = options.begin(); it != options.end(); ++it)
    {
        out << "  " << it->second->help() << "\n";
    }
    return out.str();
};
void OptionGroup::add_option(string name, string description)
{
    pOption op = Option<bool>::create(name, description);
    options[name] = op;
};




