#ifndef GTKMM_GIFAUTHOR_COLORTABLELOOKUP_H
#define GTKMM_GIFAUTHOR_COLORTABLELOOKUP_H

#include <vector>
#include <string>
#include "../output/gif.h"

/**
 * ColorTableLookup - given an RGB value, find the closest value in the colortable
 * currently, this is just brute force, but an octree or something would be useful
 */
class ColorTableLookup
{
    public:
        ColorTableLookup(const GIFColorTable *ct);
        ~ColorTableLookup();

        int get_closest_to(const uint8_t *rhs);

    private:
        float distance(const uint8_t *lhs, const uint8_t *rhs);

        const GIFColorTable *ct;
};

#endif
