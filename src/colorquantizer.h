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

        virtual void add_color(const uint8_t* color) = 0;
        virtual void build_ct() = 0;
        virtual int map_to_ct(const uint8_t* color) const = 0;
        virtual GIFColorTable get_ct() const = 0;
};


        

#endif
