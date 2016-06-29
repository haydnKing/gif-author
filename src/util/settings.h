/* Code to store algorithm settings and display them to a UI */
#ifndef GIFAUTHOR_SETTINGS_H
#define GIFAUTHOR_SETTINGS_H

#include <string>
#include <sstream>
#include <map>
#include <typeinfo>

using namespace std;

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>

/**
 * An individual setting
 */
class Setting
{
    public:
        Setting(std::string name, std::string description);

        const std::string& get_name() const;
        const std::string& get_description() const;

        //get value - need to call the right method
        bool get_bool() const;
        int get_int() const;
        float get_float() const;
        std::string get_str() const;

        //configure from command line
        bool from_str(std::string rvalue) = 0;

        std::string to_str() const = 0;

    private:
        void throw_error(std::string attempted_type);
        std::string name, description, typestr;
        bool bounded;
};

class IntSetting : public Setting
{
    public:
        IntSetting(std::string name, std::string description, int default_value=0);
        IntSetting(std::string name, std::string description, int default_value, int min_value, int max_value);

        int get_int() const;

        bool from_str(std::string rvalue);
        std::string to_str() const = 0;

    private:
        int value, min_value, max_value;
};

class PositiveIntSetting : public IntSetting
{
    public:
        PositiveIntSetting(std::string name, std::string description, int default_value=0);
        PositiveIntSetting(std::string name, std::string description, int default_value, int max_value);
};

class FloatSetting : public Setting
{
    public:
        FloatSetting(std::string name, std::string description, float default_value=0);
        FloatSetting(std::string name, std::string description, float default_value, float min_value, float max_value);

        float get_float() const;

        bool from_str(std::string rvalue);
        std::string to_str() const = 0;

    private:
        float value, min_value, max_value;
};

class PositiveFloatSetting : public FloatSetting
{
    public:
        PositiveFloatSetting(std::string name, std::string description, float default_value=0.);
        PositiveFloatSetting(std::string name, std::string description, float default_value, float max_value);
};

class StringSetting : public Setting
{
    public:
        StringSetting(std::string name, std::string description, string default_value="");

        std::string get_str() const;

        bool from_str(std::string rvalue);
        std::string to_str() const = 0;

    private:
        std::string value;
};


/**
 * An object which stores settings
 */
class Configurable
{
    public:
        Configurable(std::string name, std::string description) :
            name(name),
            description(description)
        {};

        /**
         * Add a new setting
         */
        bool add_setting(Setting s);
        
        const Setting& get_setting(const string& name) const;

        Setting& get_setting(const string& name);

        std::string get_description() const;

        bool configure(const string& cmd);

    private:
        map<std::string, Setting> my_map;
        std::string name, description;
};

#endif //GIFAUTHOR_SETTINGS_H

