#ifndef GIFAUTHOR_BITSET_H
#define GIFAUTHOR_BITSET_H

#include <glibmm/refptr.h>

class Bitset;
typedef Glib::RefPtr<Bitset> pBitset;

class Bitset : public Glib::Object
{
    public:
        ~Bitset();
        static pBitset create(int _width, int _height, bool initial=false);

        bool get(int x, int y) const;
        void set(int x, int y, bool s=true);

        void clear(bool v=false);

        //remove isolated pixels
        void remove_islands();

    private:
        uint8_t *data;
        int width, height;
        Bitset(int _width, int _height, bool initial);
};

#endif


