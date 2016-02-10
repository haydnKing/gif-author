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
        uint32_t *hist;
};

MMCQuantizerMMCQuantizer()
{
    //2^17, number of quantum spaces
    hist = new uint32_t[131072];
};

virtual MMCQuantizer~MMCQuantizer()
{
    delete [] hist;
};

void MMCQuantizer::add_color(uint8_t* color)
{
}

void MMCQuantizer::build_ct(std::vector<uint8_t*> colors)
{
};

int MMCQuantizer::map_to_ct(uint8_t* color) const
{
};

GIFColorTable MMCQuantizer::get_ct()
{
};

pColorQuantizer ColorQuantizer::get_quantizer(QuantizerMethod m)
{
    ColorQuantizer *r;
    switch(m){
        case QUANT_MMC:
            r = new MMCQuantizer();
    };
    return pColorQuantizer(r);
};
