#ifndef GIFAUTHOR_BITSET_H
#define GIFAUTHOR_BITSET_H

#include <glibmm/refptr.h>
#include <glibmm/object.h>
#include <cstring>

class Bitset;
typedef Glib::RefPtr<Bitset> pBitset;

class Bitset : public Glib::Object
{
    public:
        ~Bitset();
        static pBitset create(int _width, int _height, bool initial=false);
        static pBitset smooth(pBitset in, float sigma, float threshold);

        bool get(int x, int y) const;
        void set(int x, int y, bool s=true);

        int get_width() const {return width;};
        int get_height() const {return height;};

        void clear(bool v=false);

        //remove isolated pixels
        void remove_islands();

        //smooth 

    private:
        uint8_t *data;
        int width, height;
        Bitset(int _width, int _height, bool initial);
};

#endif


