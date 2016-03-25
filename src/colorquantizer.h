#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include <vector>
#include <glibmm/refptr.h>
#include "output/gif.h"

#include <iostream>

enum QuantizerMethod {
    QUANT_MMC //Modified Median Cut
};

class ColorQuantizer;

typedef Glib::RefPtr<ColorQuantizer> pColorQuantizer;

class ColorQuantizer : public Glib::Object
{
    public:
        ColorQuantizer();
        virtual ~ColorQuantizer();

        static pColorQuantizer get_quantizer(QuantizerMethod m=QUANT_MMC);

        void set_max_colors(int max_colors);
        void add_color(const uint8_t* color);
        void add_colors(const uint8_t* color, int count);

        virtual void build_ct(int quantized_colors=256) = 0;
        virtual int map_to_ct(const uint8_t* color) const = 0;
        virtual const GIFColorTable *get_ct() const = 0;

    protected:
        uint8_t* colors;
        int max_colors, num_colors;
};


        

#endif
