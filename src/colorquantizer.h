#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include <vector>
#include <glibmm/refptr.h>
#include "output/gif.h"

enum QuantizerMethod {
    QUANT_MMC //Modified Median Cut
};

class ColorQuantizer;

typedef Glib::RefPtr<ColorQuantizer> pColorQuantizer;

class ColorQuantizer
{
    public:
        ColorQuantizer();
        virtual ~ColorQuantizer();

        static pColorQuantizer get_quantizer(QuantizerMethod m);

        void set_max_colors(int max_colors);
        void add_color(const uint8_t* color);

        virtual void build_ct() = 0;
        virtual int map_to_ct(const uint8_t* color) const = 0;
        virtual GIFColorTable *get_ct() const = 0;

    protected:
        uint8_t* colors;
        int max_colors, num_colors;
};


        

#endif
