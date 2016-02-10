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

        virtual void build_ct(std::vector<uint8_t*> colors) = 0;
        virtual int map_to_ct(uint8_t* color) const = 0;
        virtual GIFColorTable get_ct() const = 0;
};


        

#endif
