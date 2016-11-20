//#include "gifauthor.h"
#include <iostream>

#include "util/cline.h"
#include "encoder/colorquantizer.h"
#include "encoder/ditherer.h"
#include "encoder/segmenter.h"

int main (int argc, char *argv[])
{
    std::cout << "Hello, world" << std::endl;
    pOptionGroup og = OptionGroup::create("mainGroup");

    int intopt = 5;
    float floatopt = 4.0;
    bool boolopt = false;
    pColorQuantizer cq;
    pDitherer dither;
    pSegmenter segmenter;

    og->add_option<int>("intopt", "an option that's an integer", intopt);
    og->add_option<float>("floatopt", "an option that's a float", floatopt);
    og->add_option<bool>("boolopt", "an option that's a bool", boolopt);
    og->add_option(QuantizerFactory::create(cq));
    og->add_option(DithererFactory::create(dither));
    og->add_option(SegmenterFactory::create(segmenter));

    std::cout << og->help() << std::endl;
    std::cout << "parsing..." << std::endl;

    std::vector<string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
    }
    std::vector<string> uk = og->parse(args);

    if(uk.size() > 0)
    {
        for(auto it : uk)
        {
            std::cout << "Unknown argument: " << it << std::endl;
        }
    }

    cout << "\n-----\nParsed values: " << endl;
    cout << "  intopt: " << intopt << endl;
    cout << "  floatopt: " << floatopt << endl;
    cout << "  boolopt: " << boolopt << endl;

    return 0;
}
