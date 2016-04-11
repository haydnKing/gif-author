/* Code to store algorithm settings and display them to a UI */
#ifndef GIFAUTHOR_SETTINGS_H
#define GIFAUTHOR_SETTINGS_H

#include <string>
#include <map>
#include <typeinfo>

using namespace std;

/**
 * An individual setting object
 */
template <class T> class Setting
{
    public:
        Setting(const T& default_value, string description) :
            value(default_value),
            description(description),
            bounds(false)
        {};
        Setting(const T& default_value, const T& min_value, const T& max_value, string description) :
            value(default_value),
            min_value(min_value),
            max_value(max_value),
            description(description),
            bounds(true)
        {};
        Setting(const Setting<T>& rhs) :
            value(rhs.get_value()),
            min_value(rhs.get_minimum()),
            max_value(rhs.get_maximum()),
            description(rhs.get_description()),
            bounds(rhs.is_bounded())
        {};
        ~Setting() {};

        const T& get_value() const {return value;}
        const T& get_minimum() const {return min_value;}
        const T& get_maximum() const {return max_value;}

        const string& get_description() const {return description;}
        bool is_bounded() const {return bounds;}

        bool set_value(const T& new_value)
        {
            if(bounds)
            {
                if(new_value > max_value || new_value < min_value)
                    return false;
            }
            value = new_value;
            return true;
        };



    private:
        T value, min_value, max_value;
        string description;
        bool bounds;
};


/**
 * An object which stores settings
 */
class Configurable
{
    public:
        typedef pair<const type_info&, void*> setting_type;

        /**
         * Add a new setting, without bounds
         */
        template <class T>
            bool add_setting(const string& name, const T& default_value, const string& description)
            {
                Setting<T> s = Setting<T>(name, default_value, description);
                return add_setting(s);
            };

        /**
         * Add a new setting, with bounds
         */
        template <class T>
            bool add_setting(const string& name, const T& default_value, const T& min_value, const T& max_value, const string& description)
            {
                Setting<T> s = Setting<T>(name, default_value, min_value, max_value, description);
                return add_setting(s);
            };

        /**
         * Directly add a new setting
         */
        template <class T>
            bool add_setting(const string& name, const Setting<T> setting)
            {
                auto it = my_map.insert(make_pair(name, setting_type(typeid(T), nullptr)));
                //if we failed
                if(!it.second) return false;
                //otherwise set the object
                it.first->second.second = new Setting<T>(setting);
                return true;
            };

        template <class T>
            const Setting<T>& get_setting(const string& name) const
            {
                auto it = my_map.at(name);
                if(it.first != typeid(T)) throw bad_cast();
                return *(T*)it.second;
            };


    private:
        map<string, setting_type> my_map;
};

#endif //GIFAUTHOR_SETTINGS_H

