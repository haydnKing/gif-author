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
            my_option()
        {};
        virtual ~Factory() {};

        C *get(const std::string& name)
        {
            return my_map.at(name);
        };

        bool register_type(const std::string& name, C* cfg)
        {
            auto r = my_map.insert(make_pair(name, cfg));
            return r.second;
        };

        Glib::OptionGroup get_option_group()
        {
            Glib::OptionGroup og(my_name, my_desc);
            Glib::OptionEntry oe;
            oe.set_long_name(my_name);
            oe.set_flags(Glib::OptionEntry::FLAG_OPTIONAL_ARG);

            std::stringstream ss;
            ss << "Select the " << my_name << " to use. Valid values are:\n";
            for(auto it : my_map)
            {
                ss << "\"" << it.first << "\": Description here later\n";
            }
            oe.set_description(ss.str());
            og.add_entry(oe, sigc::mem_fun(*this, &Factory::on_option_name));
            return og;
        };

        /**
         * return the name of the chosen item
         */
        const std::string& get_chosen_name() const
        {
            return my_option;
        };

        /**
         * return the chosen item itself
         */
        C* get_chosen_item() 
        {
            return my_map.find(my_option);
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
        std::string my_name, my_desc, my_option;
        std::map<std::string, C*> my_map;
};


#endif // GIFAUTHOR_FACTORY_H
