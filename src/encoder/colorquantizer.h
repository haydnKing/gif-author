#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include <vector>
#include <string>
#include "../output/gif.h"

#include "../util/cline.h"

class ColorQuantizer;
typedef shared_ptr<ColorQuantizer> pColorQuantizer;

class ColorQuantizer : public OptionGroup
{
    public:
        ColorQuantizer(std::string name, std::string description);
        virtual ~ColorQuantizer();

        void set_max_colors(int max_colors);
        void add_color(const uint8_t* color);
        void add_colors(const uint8_t* color, int count);
        int get_num_colors() const {return num_colors;};

        virtual void build_ct(bool transparency, int quantized_colors=256) = 0;
        virtual const GIFColorTable *get_ct() const = 0;

    protected:
        uint8_t* colors;
        int max_colors, num_colors;
};

/**
 * A factory for ColorQuantizers
 */
class QuantizerFactory : public FactoryOption<ColorQuantizer>
{
    public: 
        static pOption create(pColorQuantizer& value);

    protected:
        QuantizerFactory(pColorQuantizer& value);
};

#endif
