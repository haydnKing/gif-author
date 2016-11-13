#include "cline.h"

/*
 * Option
 */


Option::Option(std::string name, std::string description, std::string typestr) : 
    name(name),
    description(description),
    typestr(typestr)
{};

const std::string& Option::get_name() const {return name;};
const std::string& Option::get_description() const {return description;};

bool Option::get_bool() const {throw_error("bool");};
int Option::get_int() const {throw_error("bool");};
float Option::get_float() const {throw_error("float");};
std::string Option::get_str() const {throw_error("str");};
        
std::string Option::get_help_string() const
{
    std::stringstream ss;
    ss << name << "=" << to_str() << ": " << description;
    return ss.str();
}

void Option::throw_error(std::string attempted_type) const
{
    std::stringstream ss;
    ss << "Option " << name << " is of type " << typestr << " not " << attempted_type;
    throw std::runtime_error(ss.str());
}

/*
 * IntOption
 */
IntOption::IntOption(std::string name, std::string description, int default_value) :
    Option(name, description, "integer"),
    bounded(false),
    value(default_value)
{};

IntOption::IntOption(std::string name, std::string description, int default_value, int min_value, int max_value) :
    Option(name, description, true, "integer"),
    value(default_value),
    bounded(true),
    min_value(min_value),
    max_value(max_value)
{};

int IntOption::get_int() const {return value;};

bool IntOption::from_str(std::string rvalue)
{
    int v = std::stoi(rvalue);
    if(bounded && (v < min_value || v > max_value))
    {
        return false;
    }
    value = v;
    return true;
};

std::string IntOption::to_str() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/*
 * PositiveIntOption
 */
PositiveIntOption::PositiveIntOption(std::string name, std::string description, int default_value) :
    IntOption(name, description, default_value, 0, std::numeric_limits<int>::max())
{};
PositiveIntOption::PositiveIntOption(std::string name, std::string description, int default_value, int max_value) :
    IntOption(name, description, default_value, 0, max_value)
{};

/*
 * FloatOption
 */
FloatOption::FloatOption(std::string name, std::string description, float default_value) : 
    Option(name, description, "float")
    bounded(false),
    value(default_value)
{};
FloatOption::FloatOption(std::string name, std::string description, float default_value, float min_value, float max_value) :
    Option(name, description, "float"),
    bounded(true),
    value(default_value),
    min_value(min_value),
    max_value(max_value)
{};

float FloatOption::get_float() const {return value;};

bool FloatOption::from_str(std::string rvalue)
{
    float v = std::stof(rvalue);
    if(bounded && (v < min_value || v > max_value))
    {
        return false;
    }
    value = v;
    return true;
};

std::string FloatOption::to_str() const
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/*
 * PositiveFloatOption
 */
PositiveFloatOption::PositiveFloatOption(std::string name, std::string description, float default_value) : 
    FloatOption(name, description, default_value, 0, std::numeric_limits<float>::max())
{};
PositiveFloatOption::PositiveFloatOption(std::string name, std::string description, float default_value, float max_value) : 
    FloatOption(name, description, default_value, 0, max_value)
{};

/*
 * StringOption
 */
StringOption::StringOption(std::string name, std::string description, std::string default_value):
    Option(name, description, "string"),
    value(default_value)
{};

std::string StringOption::get_str() const {return value;};

bool StringOption::from_str(std::string rvalue)
{
    value = rvalue;
    return true;
}

std::string StringOption::to_str() const
{
    return value;
}



