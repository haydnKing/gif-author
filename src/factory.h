#ifndef GIFAUTHOR_FACTORY_H
#define GIFAUTHOR_FACTORY_H

#include <map>
#include <string>

#include "settings.h"

using std::map;

/**
 * A factory class template for Configureables
 * C: a Configureable
 */
template<class C> class Factory
{
    public:

        static C *get(const std::string& name)
        {
            return my_map.at(type);
        };

        static bool register_type(const std::string& name, C* cfg)
        {
            auto r = my_map.insert(make_pair(name, cfg));
            return r.second;
        };

    private:
        static std::map<std::string, C*> my_map;
};

template<class C>
std::map<std::string,C*> Factory<std::string,C>::my_map = std::map<std::string,C*>();

#endif // GIFAUTHOR_FACTORY_H
