#include "cline.h"

BaseOption::BaseOption(string name, string description) :
    my_name(name),
    my_description(description)
{};

BaseOption::~BaseOption()
{};


OptionGroup::OptionGroup(string name) :
    my_name(name)
{};

OptionGroup::~OptionGroup()
{};

pOptionGroup OptionGroup::create(string name)
{
    return pOptionGroup(new OptionGroup(name));
};

string OptionGroup::get_help()
{
    ostringstream out;
    out << my_name << " options:\n";
    for(auto it = options.begin(); it != options.end(); ++it)
    {
        out << "  " << it->second->help() << "\n";
    }
};




