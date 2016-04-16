#ifndef GIFAUTHOR_FACTORY_H
#define GIFAUTHOR_FACTORY_H

#include <map>

using std::map;

/**
 * A factory class template for Configureables
 *
 * The idea is that this will also help building command line options etc
 */
template<class E, class T> class Factory
{
    public:
        static T *get(const E& type)
        {
            return my_map.at(type);
        };

        static bool register_type(const E& type, T* cfg)
        {
            auto r = my_map.insert(make_pair(type, cfg));
            return r.second;
        };

    private:
        static std::map<E, T*> my_map;
};

template<class E, class T>
std::map<E,T*> Factory<E,T>::my_map = std::map<E,T*>();

#endif // GIFAUTHOR_FACTORY_H
