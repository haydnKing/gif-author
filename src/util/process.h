#ifndef GIFAUTHOR_PROCESS_H
#define GIFAUTHOR_PROCESS_H

#include <string>
#include <sstream>
#include <map>
#include <limits>

#include <glibmm/optiongroup.h>
#include <glibmm/optionentry.h>
#include <glibmm/optioncontext.h>

/**
 * An individual setting
 */
class Setting
{
    public:
        Setting(std::string name, std::string description, bool bounded, std::string typestr);

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
        void throw_error(std::string attempted_type) const;
        std::string name, description, typestr;
        bool bounded;
};

class IntSetting : public Setting
{
    public:
        IntSetting(std::string name, 
                   std::string description, 
                   int default_value=0);
        IntSetting(std::string name, 
                   std::string description, 
                   int default_value, 
                   int min_value, 
                   int max_value);

        virtual int get_int() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        int value, min_value, max_value;
};

class PositiveIntSetting : public IntSetting
{
    public:
        PositiveIntSetting(std::string name, 
                           std::string description, 
                           int default_value=0);
        PositiveIntSetting(std::string name, 
                           std::string description, 
                           int default_value, 
                           int max_value);
};

class FloatSetting : public Setting
{
    public:
        FloatSetting(std::string name, 
                     std::string description, 
                     float default_value=0);
        FloatSetting(std::string name, 
                     std::string description, 
                     float default_value, 
                     float min_value, 
                     float max_value);

        virtual float get_float() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        float value, min_value, max_value;
};

class PositiveFloatSetting : public FloatSetting
{
    public:
        PositiveFloatSetting(std::string name, 
                             std::string description, 
                             float default_value=0.);
        PositiveFloatSetting(std::string name, 
                             std::string description, 
                             float default_value, 
                             float max_value);
};

class StringSetting : public Setting
{
    public:
        StringSetting(std::string name, 
                      std::string description, 
                      std::string default_value="");

        virtual std::string get_str() const;

        bool from_str(std::string rvalue);
        std::string to_str() const;

    protected:
        std::string value;
};


/**
 * A class which represents a general operation with settings
 */
class Process
{
    public:
        Process(std::string name, std::string description);
        ~Process();

        /**
         * Add a new setting
         */
        bool add_setting(Setting* s);
        
        const Setting* get_setting(const std::string& name) const;

        Setting* get_setting(const std::string& name);

        std::string get_name() const;
        std::string get_description() const;

        bool configure(std::string cmd);

        std::vector<std::string> get_help_strings() const;

    private:
        std::map<std::string, Setting*> my_map;
        std::string name, description;
};


/**
 * A factory class template for Processes, allowing Process selection
 * and configuration from command line or (in future) a GUI
 */
template<class P> class ProcessFactory
{
    public:
        ProcessFactory(std::string factory_name, 
                       std::string factory_description) : 
            my_name(factory_name),
            my_desc(factory_description),
            my_option(),
            my_default()
        {};
        virtual ~ProcessFactory() {};

        /**
         * get a Process by name
         */
        P *by_name(const std::string& name)
        {
            return my_map.at(name);
        };

        /**
         * return the name of the selected Process
         */
        const std::string& get_selected_name() const
        {
            if(my_option.empty()) return my_default;
            return my_option;
        };

        /**
         * return the chosen Process
         */
        P* get_selected()
        {
            if(my_option.empty()) return my_map.at(my_default);
            return my_map.at(my_option);
        };

        /**
         * OptionGroup to give to an OptionContext for
         * Glib Command line parsing
         */
        std::string get_help_string()
        {
            std::stringstream ss;
            ss << my_desc << ". Valid arguments are:\n";
            
            //settings
            std::string indent = "          ";
            for(auto f_type = my_map.begin(); f_type != my_map.end(); f_type++)
            {
                ss << indent << f_type->first << " ";
                if(f_type->first == my_default) ss << "(default) ";
                ss << f_type->second->get_description();
                std::vector<std::string> help_strings = f_type->second->get_help_strings();
                if(!help_strings.empty())
                {
                    ss << std::endl;
                    for(auto help_string = help_strings.begin(); help_string != help_strings.end(); help_string++)
                    {
                        ss << indent << "     " << *help_string;
                        if(help_string != --help_strings.end()) ss << std::endl;
                    }
                }
                if(f_type != --my_map.end()) ss << std::endl;
            }
            return ss.str();
        };

        std::string get_format_string() 
        {
            return "name;arg1=val1;...";
        };

        bool on_parse(const Glib::ustring& option_name, 
                      const Glib::ustring& option_value,
                      bool has_value)
        {
            std::stringstream ss;
            if(!has_value) 
            {
                ss << option_name << " requires an argument";
                throw Glib::OptionError(Glib::OptionError::BAD_VALUE, ss.str());
            }
            //first extract the name part
            int end = option_value.find(';');
            std::string name, arglist;
            if(end != std::string::npos)
            {
                name = option_value.substr(0, end);
                if(option_value.length() > end+1)
                    arglist = option_value.substr(end+1);
            }
            else
                name = option_value;
            //check that option_value is a valid Factory object, otherwise return false
            if(my_map.count(name) > 0)
            {
                my_option = name;
                if(!arglist.empty())
                    my_map.at(name)->configure(arglist);
                return true;
            }

            ss << "Unknown " << my_name << ": \'" << name << "\'";
            throw Glib::OptionError(Glib::OptionError::BAD_VALUE, ss.str());
        };


    protected:
        bool register_type(const std::string& name, P* cfg)
        {
            if(my_default.empty()) my_default = name;
            auto r = my_map.insert(make_pair(name, cfg));
            return r.second;
        };
    private:
        std::string my_name, my_desc, my_option, my_default;
        std::map<std::string, P*> my_map;
};
#endif //GIFAUTHOR_PROCESS_H

