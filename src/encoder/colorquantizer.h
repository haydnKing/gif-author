#ifndef GTKMM_GIFAUTHOR_COLORQUANTIZER_H
#define GTKMM_GIFAUTHOR_COLORQUANTIZER_H

#include <vector>
#include <string>
#include "../output/gif.h"
#include "../video/imagesequence.h"

#include "../util/cline.h"

class ColorQuantizer;
typedef shared_ptr<ColorQuantizer> pColorQuantizer;

class ColorQuantizer : public OptionGroup
{
    public:
        ColorQuantizer(std::string name, std::string description);
        virtual ~ColorQuantizer();

        int get_num_colors() const {return num_colors;};

        pcGIFColorTable ct(pFrame fr, 
                           bool transparency, 
                           int quantized_colors=256);

    protected:
        virtual pcGIFColorTable build_ct(bool transparency, 
                                         int quantized_colors=256) = 0;
        void clear_colors();
        uint8_t* colors;
        int num_colors, max_colors;
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
