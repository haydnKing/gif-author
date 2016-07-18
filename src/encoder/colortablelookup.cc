#include "colortablelookup.h"

ColorTableLookup::ColorTableLookup(const GIFColorTable *ct) :
    ct(ct)
{};

ColorTableLookup::~ColorTableLookup() {};

int ColorTableLookup::get_closest_to(const uint8_t *rhs)
{
    float min_v = std::numeric_limits<float>::max(),
          cur_v;
    int min_i = 0;
    for(int i = 0; i < ct->num_colors(); i++)
    {
        cur_v = distance(ct->get_index(i), rhs);
        if(cur_v < min_v)
        {
            min_v = cur_v;
            min_i = i;
        }
    }
    return min_i;
};

float ColorTableLookup::distance(const uint8_t *lhs, const uint8_t *rhs)
{
    float r = float(lhs[0]) - float(rhs[0]);
    float g = float(lhs[1]) - float(rhs[1]);
    float b = float(lhs[2]) - float(rhs[2]);
    return r*r+g*g+b*b;
}


