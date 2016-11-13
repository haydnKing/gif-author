#ifndef GIFAUTHOR_PROCESS_H
#define GIFAUTHOR_PROCESS_H

#include <string>
#include <sstream>
#include <map>
#include <limits>
#include <iostream>
#include <vector>

/**
 * An individual setting
 */
class Option
{
    public:

        const std::string& get_name() const;
        const std::string& get_description() const;

        //get value - need to call the right method
        virtual bool get_bool() const;
        virtual int get_int() const;
        virtual float get_float() const;
        virtual std::string get_str() const;

        //configure from command line
        virtual bool from_str(std::string rvalue) = 0;

        virtual std::string to_str() const = 0;

        std::string get_help_string() const;
        const std::string get_typestr() const {return typestr;};

    protected:
        Option(std::string name, std::string description, std::string typestr);
        void throw_error(std::string attempted_type) const;
        std::string name, description, typestr;
};

class IntOption : public Option
{
    public:
        IntOption(std::string name, 
                  std::string description, 
                  int default_value=0);
        IntOption(std::string name, 
                  std::string description, 
                  int default_value, 
                  int min_value, 
                  int max_value);

        virtual int get_int() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        int value, min_value, max_value;
        bool bounded;
};

class PositiveIntOption : public IntOption
{
    public:
        PositiveIntOption(std::string name, 
                           std::string description, 
                           int default_value=0);
        PositiveIntOption(std::string name, 
                           std::string description, 
                           int default_value, 
                           int max_value);
};

class FloatOption : public Option
{
    public:
        FloatOption(std::string name, 
                     std::string description, 
                     float default_value=0);
        FloatOption(std::string name, 
                     std::string description, 
                     float default_value, 
                     float min_value, 
                     float max_value);

        virtual float get_float() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        float value, min_value, max_value;
        bool bounded;
};

class PositiveFloatOption : public FloatOption
{
    public:
        PositiveFloatOption(std::string name, 
                             std::string description, 
                             float default_value=0.);
        PositiveFloatOption(std::string name, 
                             std::string description, 
                             float default_value, 
                             float max_value);
};

class StringOption : public Option
{
    public:
        StringOption(std::string name, 
                      std::string description, 
                      std::string default_value="");

        virtual std::string get_str() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        std::string value;
};



#endif //GIFAUTHOR_PROCESS_H

