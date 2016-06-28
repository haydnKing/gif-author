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


template<typename ... Args>
string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

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
                Setting<T> s = Setting<T>(default_value, description);
                return add_setting(name, s);
            };

        /**
         * Add a new setting, with bounds
         */
        template <class T>
            bool add_setting(const string& name, const T& default_value, const T& min_value, const T& max_value, const string& description)
            {
                Setting<T> s = Setting<T>(default_value, min_value, max_value, description);
                return add_setting(name, s);
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
                return *(Setting<T>*)it.second;
            };

        template <class T>
            Setting<T>& get_setting(const string& name)
            {
                auto it = my_map.at(name);
                if(it.first != typeid(T)) throw bad_cast();
                return *(Setting<T>*)it.second;
            };

        string get_help_string() const
        {
            stringstream out;
            for(auto it : my_map) // what is this magic??
            {
                out << it.first;
                //*looks shifty*
                Setting<T> setting = get_setting(it.first);
                if(setting->is_bounded())
                {
                    out << " [" << setting.get_minimum() << ", " 
                        << setting.get_maximum() << "]";
                }
                out << ": " << setting->get_description() << endl;
            }
            return out.str();
        };

        bool parse(const string& cmd)
        {
            int pos = 0, end = 0, equal = 0;
            string sub_cmd, lv, rv;
            while(pos < cmd.length())
            {
                end = cmd.find(';', pos);
                //last substring
                if(pos < 0) pos = cmd.length();
                sub_cmd = cmd.substr(pos, end-pos);
                
                //parse the sub command
                //is there an equals?
                equal = sub_cmd.find('=');
                //no
                if(equal == string::npos)
                {
                    //infer: setting is a bool, set to true
                    get_setting<bool>(sub_cmd).set_value(true);
                }
                //yes
                else
                {
                    lv = sub_cmd.substr(pos, equal-pos);
                    rv = sub_cmd.substr(equal+1, end-equal-1);
                    //boolean
                    if(rv == "true")
                    {
                        get_setting<bool>(sub_cmd).set_value(true);
                    }
                    else if(rv == "false")
                    {
                        get_setting<bool>(sub_cmd).set_value(false);
                    }
                    else //int, float, or string
                    {
                        try
                        {
                            int v = stoi(rv);
                            if(!get_setting<int>(lv).set_value(v))
                                throw invalid_argument(string_format("Argument \'%s\' outside bounds", lv));
                        }
                        catch(invalid_argument e)
                        {
                            try
                            {
                                float v = stof(rv);
                                if(!get_setting<float>(lv).set_value(v))
                                    throw invalid_argument(string_format("Argument \'%s\' outside bounds", lv));
                            }
                            catch(invalid_argument f)
                            {
                                get_setting<string>(lv).set_value(rv);
                            }
                        }
                    }
                }
                pos = end;
            }
        };


    private:
        map<string, setting_type> my_map;
};

#endif //GIFAUTHOR_SETTINGS_H

