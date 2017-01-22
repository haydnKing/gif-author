#ifndef GTKMM_GIFAUTHOR_DITHERER_H
#define GTKMM_GIFAUTHOR_DITHERER_H

#include "../video/videoframe.h"
#include "../output/gif.h"
#include "colorquantizer.h"
#include "../util/bitset.h"
#include "../util/cline.h"

#include <string>
#include <memory>

class Ditherer;
typedef std::shared_ptr<Ditherer> pDitherer;

class Ditherer : public OptionGroup
{
    public:
        Ditherer(std::string name, std::string description);
        virtual ~Ditherer() {};


        pGIFImage dither_image(const pVideoFrame vf,
                               const pBitset mask,
                               const GIFColorTable *ct) const;

    protected:

        virtual void _dither_image(pGIFImage out,
                                   const pVideoFrame vf,
                                   const pBitset mask,
                                   const GIFColorTable *ct) const = 0;
};

/**
 * A factory for Ditherers
 */
class DithererFactory : public FactoryOption<Ditherer>
{
    public: 
        static pOption create(pDitherer& value);

    protected:
        DithererFactory(pDitherer& value);
};

#endif
