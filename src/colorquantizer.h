#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include "output/gif.h"

enum QuantizerMethod {
    QUANT_WEB_COLORS,
    QUANT_MMC
};

ColorQuantizer* get_quantizer(QuantizerMethod m);

class ColorQuantizer
{
    public:
        ColorQuantizer();
        virtual ~ColorQuantizer();

        virtual void build_ct(uint8_t** colors) = 0;
        virtual int map_to_ct(uint8_t* color) const = 0;
        virtual GIFColorTable get_ct() const = 0;
};


        

#endif
