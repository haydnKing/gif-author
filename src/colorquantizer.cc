#include "colorquantizer.h"

class MMCQuantizer : public ColorQuantizer
{
    public:
        MMCQuantizer();
        virtual ~MMCQuantizer();

        void add_color(const uint8_t* color);
        void build_ct();
        int map_to_ct(const uint8_t* color) const;
        GIFColorTable get_ct();

    protected:
        unsigned int color_to_index(const uint8_t* color) const;

        uint32_t *hist;
};

MMCQuantizer::MMCQuantizer()
{
    //2^17, number of quantum spaces
    hist = new uint32_t[131072];
    std::memset(hist, 0, 131072*sizeof(uint32_t));
};

virtual MMCQuantizer::~MMCQuantizer()
{
    delete [] hist;
};

void MMCQuantizer::add_color(uint8_t* color)
{
    hist[color_to_index(color)]++;
}

void MMCQuantizer::build_ct()
{
};

int MMCQuantizer::map_to_ct(uint8_t* color) const
{
    return hist[color_to_index(color)];
};

GIFColorTable MMCQuantizer::get_ct()
{
};
        
unsigned int MMCQuantizer::color_to_index(const uint8_t* color) const
{
    //6 bits red, 6 bits green, 5 bits blue
    unsigned int r = ((color[0] & 252) << 9) + 
                     ((color[1] & 252) << 3) + 
                     (color[2] >> 3);
    return r;
}

pColorQuantizer ColorQuantizer::get_quantizer(QuantizerMethod m)
{
    ColorQuantizer *r;
    switch(m){
        case QUANT_MMC:
            r = new MMCQuantizer();
    };
    return pColorQuantizer(r);
};
