#ifndef GIFAUTHOR_BITSET_H
#define GIFAUTHOR_BITSET_H

#include <cstring>
#include <cmath>
#include <memory>

class Bitset;
typedef std::shared_ptr<Bitset> pBitset;

class Bitset
{
    public:
        ~Bitset();
        static pBitset create(int _width, int _height, bool initial=false);
        static pBitset smooth(pBitset in, float sigma, float threshold);
        static pBitset crop(pBitset in, int x, int y, int width, int height);

        bool get(int x, int y) const;
        void set(int x, int y, bool s=true);

        int get_width() const {return width;};
        int get_height() const {return height;};

        void clear(bool v=false);

        //remove isolated pixels
        void remove_islands();

        //get the position of the highest set pixel
        int get_top() const;
        int get_bottom() const;
        int get_left() const;
        int get_right() const;

    private:
        uint8_t *data;
        int width, height;
        Bitset(int _width, int _height, bool initial);
};

#endif


