#include "process.h"

/*
 * Setting
 */


Setting::Setting(std::string name, std::string description, bool bounded, std::string typestr) : 
    name(name),
    description(description),
    bounded(bounded),
    typestr(typestr)
{};

const std::string& Setting::get_name() const {return name;};
const std::string& Setting::get_description() const {return description;};

bool Setting::get_bool() const {throw_error("bool");};
int Setting::get_int() const {throw_error("bool");};
float Setting::get_float() const {throw_error("float");};
std::string Setting::get_str() const {throw_error("str");};
        
std::string Setting::get_help_string() const
{
    std::stringstream ss;
    ss << name << "=" << to_str() << ": " << description;
    return ss.str();
}

void Setting::throw_error(std::string attempted_type) const
{
    std::stringstream ss;
    ss << "Setting " << name << " is of type " << typestr << " not " << attempted_type;
    throw std::runtime_error(ss.str());
}

/*
 * IntSetting
 */
IntSetting::IntSetting(std::string name, std::string description, int default_value) :
    Setting(name, description, false, "integer"),
    value(default_value)
{};

IntSetting::IntSetting(std::string name, std::string description, int default_value, int min_value, int max_value) :
    Setting(name, description, true, "integer"),
    value(default_value),
    min_value(min_value),
    max_value(max_value)
{};

int IntSetting::get_int() const {return value;};

bool IntSetting::from_str(std::string rvalue)
{
    int v = std::stoi(rvalue);
    if(bounded && (v < min_value || v > max_value))
    {
        return false;
    }
    value = v;
    return true;
};

std::string IntSetting::to_str() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/*
 * PositiveIntSetting
 */
PositiveIntSetting::PositiveIntSetting(std::string name, std::string description, int default_value) :
    IntSetting(name, description, default_value, 0, std::numeric_limits<int>::max())
{};
PositiveIntSetting::PositiveIntSetting(std::string name, std::string description, int default_value, int max_value) :
    IntSetting(name, description, default_value, 0, max_value)
{};

/*
 * FloatSetting
 */
FloatSetting::FloatSetting(std::string name, std::string description, float default_value) : 
    Setting(name, description, false, "float"),
    value(default_value)
{};
FloatSetting::FloatSetting(std::string name, std::string description, float default_value, float min_value, float max_value) :
    Setting(name, description, true, "float"),
    value(default_value),
    min_value(min_value),
    max_value(max_value)
{};

float FloatSetting::get_float() const {return value;};

bool FloatSetting::from_str(std::string rvalue)
{
    float v = std::stof(rvalue);
    if(bounded && (v < min_value || v > max_value))
    {
        return false;
    }
    value = v;
    return true;
};

std::string FloatSetting::to_str() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/*
 * PositiveFloatSetting
 */
PositiveFloatSetting::PositiveFloatSetting(std::string name, std::string description, float default_value) : 
    FloatSetting(name, description, default_value, 0, std::numeric_limits<float>::max())
{};
PositiveFloatSetting::PositiveFloatSetting(std::string name, std::string description, float default_value, float max_value) : 
    FloatSetting(name, description, default_value, 0, max_value)
{};

/*
 * StringSetting
 */
StringSetting::StringSetting(std::string name, std::string description, std::string default_value):
    Setting(name, description, false, "string"),
    value(default_value)
{};

std::string StringSetting::get_str() const {return value;};

bool StringSetting::from_str(std::string rvalue)
{
    value = rvalue;
    return true;
}

std::string StringSetting::to_str() const
{
    return value;
}

/*
 * Process
 */
Process::Process(std::string name, std::string description) :
    name(name),
    description(description)
{};

Process::~Process()
{
    for(auto it : my_map)
    {
        delete it.second;
    }
}

bool Process::add_setting(Setting* s)
{
    my_map.insert(std::make_pair(s->get_name(), s));
}

const Setting* Process::get_setting(const std::string& name) const
{
    return my_map.at(name);
}

Setting* Process::get_setting(const std::string& name)
{
    return my_map.at(name);
}

std::string Process::get_description() const
{
    return description;
}

std::string Process::get_name() const
{
    return name;
}

std::vector<std::string> Process::get_help_strings() const
{
    std::vector<std::string> ret;
    for(auto it : my_map)
    {
        ret.push_back(it.second->get_help_string());
    }
    return ret;
}

bool Process::configure(std::string arg)
{
    std::cout << "\tProcess::configure(\"" << arg << "\")" << std::endl;
    int end, equals;
    bool success;
    while(!arg.empty())
    {
        end = arg.find(',');
        if(end == std::string::npos)
            end = arg.length()-1;
        equals = arg.find('=');
        if(equals == std::string::npos)
            equals = arg.length();

        std::cout << "\t\tequals, end = (" << equals << ", " << end << ")" << std::endl;
        try 
        {
            success = my_map.at(arg.substr(0,equals))->from_str(arg.substr(equals+1, end-equals-1));
        }
        catch(std::invalid_argument err)
        {
            std::stringstream ss;
            ss << "Couldn't parse " << my_map.at(arg.substr(0,equals))->get_typestr() << " from argument " << arg.substr(0,end);
            throw Glib::OptionError(Glib::OptionError::BAD_VALUE, ss.str());
        }
        if(!success)
        {
            std::stringstream ss;
            ss << "Value " << arg.substr(equals+1, end-equals-1) << " outside allowable range for option " << arg.substr(0,equals);
            throw Glib::OptionError(Glib::OptionError::BAD_VALUE, ss.str());
        }

        std::cout << "\t\targ = arg.substr(" << end+1 << ")" << std::endl;
        arg = arg.substr(end+1);
        std::cout << "done" << std::endl;
    }
    return true;
}

