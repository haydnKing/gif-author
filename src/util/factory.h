#ifndef GIFAUTHOR_FACTORY_H
#define GIFAUTHOR_FACTORY_H

#include <map>
#include <string>
#include <glibmm/optiongroup.h>
#include <glibmm/optionentry.h>
#include <sigc++/sigc++.h>
#include <sstream>

#include "settings.h"

using std::map;

/**
 * A factory class template for Configureables
 * C: a Configureable
 */
template<class C> class Factory
{
    public:
        Factory(std::string factory_name, std::string factory_description) : 
            my_name(factory_name),
            my_desc(factory_description),
            my_option(),
            my_default()
        {};
        virtual ~Factory() {};

        C *get(const std::string& name)
        {
            return my_map.at(name);
        };

        bool register_type(const std::string& name, C* cfg)
        {
            if(my_default.empty()) my_default = name;
            auto r = my_map.insert(make_pair(name, cfg));
            return r.second;
        };

        Glib::OptionGroup get_option_group()
        {
            Glib::OptionGroup og(my_name, my_desc);
            
            //select
            Glib::OptionEntry select_entry;
            select_entry.set_long_name(my_name);
            select_entry.set_flags(Glib::OptionEntry::FLAG_OPTIONAL_ARG);

            std::stringstream ss;
            ss << "Select the " << my_name << " to use. Valid values are: ";
            for(auto it : my_map)
            {
                ss << "\"" << it.first << "\"";
                if(it.first == my_default) ss << " (default)";
                if(it != *--my_map.end()) ss << ",";
            }
            select_entry.set_description(ss.str());
            og.add_entry(select_entry, 
                         sigc::mem_fun(*this, &Factory::on_option_name));
            
            //settings
            Glib::OptionEntry settings_entry;
            settings_entry.set_long_name(my_name + "-settings");
            settings_entry.set_flags(Glib::OptionEntry::FLAG_OPTIONAL_ARG);
            ss.str("");
            ss << "Settings for the chosen " << my_name << " as listed below, in the format NAME1:VALUE1;NAME2:VALUE2;....\n";
            for(auto f_type = my_map.begin(); f_type != my_map.end(); f_type++)
            {
                ss << "\t\t\t\t--" << my_name << " " << f_type->first << ": ";
                std::vector<std::string> help_strings = f_type->second->get_help_strings();
                if(!help_strings.empty())
                {
                    ss << std::endl;
                    for(auto help_string = help_strings.begin(); help_string != help_strings.end(); help_string++)
                    {
                        ss << "\t\t\t\t\t" << *help_string;
                        if(help_string != --help_strings.end()) ss << std::endl;
                    }
                }
                if(f_type != --my_map.end()) ss << std::endl;
            }
            settings_entry.set_description(ss.str());
            //should probably hook into post parse for this
            og.add_entry(settings_entry, settings_string);
            return og;
        };

        /**
         * return the name of the chosen item
         */
        const std::string& get_chosen_name() const
        {
            if(my_option.empty()) return my_default;
            return my_option;
        };

        /**
         * return the chosen item itself
         */
        C* get_chosen_item() 
        {
            if(my_option.empty()) return my_map.at(my_default);
            return my_map.at(my_option);
        };


    private:
        bool on_option_name(const Glib::ustring& option_name, 
                            const Glib::ustring& option_value,
                            bool has_value)
        {
            //check that option_value is a valid Factory object, otherwise return false
            if(my_map.count(std::string(option_value)) > 0)
            {
                my_option = std::string(option_value);
                return true;
            }
            return false;
        };
        std::string my_name, my_desc, my_option, my_default;
        Glib::ustring settings_string;
        std::map<std::string, C*> my_map;
};


#endif // GIFAUTHOR_FACTORY_H
