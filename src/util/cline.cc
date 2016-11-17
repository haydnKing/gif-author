#include "cline.h"

BaseOption::BaseOption(string name, string description) :
    my_name(name),
    my_description(description)
{};

BaseOption::~BaseOption()
{};

Option<bool>::Option(string name, string description, bool& value) :
    BaseOption(name, description),
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
string Option<bool>::help() const
{
    ostringstream out;
    out << "--" << my_name << ": " << my_description;
    return out.str();
};
void Option<bool>::parse(vector<string>::const_iterator& it)
{
    cout << my_name << " = true" << endl;
    *my_value = true;
    it++;
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

vector<string> OptionGroup::parse(const vector<string>& args)
{
    vector<string> ret;
    vector<string>::const_iterator it = args.cbegin();
    string name;
    pOption op;

    while(it != args.end())
    {
        cout << "parse: " << *it << endl;
        name = "";
        if(it->compare(0, 2, "--") == 0)
        {
            name = it->substr(2);
        } else if(it->compare(0, 1, "-") == 0) {
            name = it->substr(1);
        } else {
            cout << "  ignoring: " << *it << endl;
            ret.push_back(*it);
            it++;
            continue;
        }
        
        cout << "--name = " << name << endl;
        try
        {
            op = options.at(name);
            cout << "  op->parse(\"" << name << "\")" << endl;
            op->parse(it);
        }
        catch (out_of_range) {
            cout << "  unknown name \""<< name <<"\", ignoring: " << *it << endl;
            ret.push_back(*it);
            it++;
        }            
    }

    return ret;
};

