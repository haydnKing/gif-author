/* Code to store algorithm settings and display them to a UI */
#ifndef GIFAUTHOR_SETTINGS_H
#define GIFAUTHOR_SETTINGS_H

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <iostream>
#include <cstdlib>
#include <new>

using namespace std;

class Settings 
{
    public:
        typedef pair<const type_info&,void*> value_type;
        typedef unordered_map<string,value_type> map;
        typedef map::const_iterator const_iterator;

        template <class T>
          bool add_setting(const string& name, const T& value) 
          {
            auto it = data.insert(make_pair(
              name, value_type(typeid(T), nullptr)));
            if (!it.second) return false;
            it.first->second.second = new(malloc(sizeof(T))) T(value);
            return true; 
          }

        template <class T>
          const T& get_setting(const string& name) const 
          {
            auto it = data.at(name);
            if (it.first != typeid(T)) throw bad_cast();
            return *(T*)it.second; 
          }

        const_iterator begin() const {return data.begin(); }
        const_iterator end() const {return data.end(); }

        void erase_setting(const_iterator it) 
        {
            free(it->second.second);
            data.erase(it); 
        }

        bool remove_setting(const string& name) 
        {
            auto it = data.find(name);
            if (it == data.end()) return false;
            free(it->second.second);
            data.erase(it);
            return true; 
        }
    private:
        map data;
};

#endif //GIFAUTHOR_SETTINGS_H

