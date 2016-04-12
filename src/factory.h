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
            auto it = my_map.at(type);
            return *it;
        };

        static bool register(const E& type, T* cfg)
        {
            auto r = my_map.insert(make_pair(type, cfg));
            return r.second;
        };

    private:
        static std::map<E, T*> my_map;
};




#endif // GIFAUTHOR_FACTORY_H
