#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include <vector>
#include "output/gif.h"

enum QuantizerMethod {
    QUANT_WEB_COLORS, //Just uses standard 256 colours - mainly for debug
    QUANT_MMC //Modified Median Cut
};

class ColorQuantizer
{
    public:
        ColorQuantizer();
        virtual ~ColorQuantizer();

        static ColorQuantizer *get_quantizer(QuantizerMethod m);

        virtual void build_ct(std::vector<uint8_t*> colors) = 0;
        virtual int map_to_ct(uint8_t* color) const = 0;
        virtual GIFColorTable get_ct() const = 0;
};


        

#endif
