#include "colorquantizer.h"

class MMCQuantizer : public ColorQuantizer
{
    public:
        MMCQuantizer();
        virtual ~MMCQuantizer();

        void build_ct(std::vector<uint8_t*> colors);
        int map_to_ct(uint8_t* color);
        GIFColorTable get_ct();

    protected:

};

void MMCQuantizer::build_ct(std::vector<uint8_t*> colors)
{
};

int MMCQuantizer::map_to_ct(uint8_t* color)
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
