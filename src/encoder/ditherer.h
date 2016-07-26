#ifndef GTKMM_GIFAUTHOR_DITHERER_H
#define GTKMM_GIFAUTHOR_DITHERER_H

#include "../video/videoframe.h"
#include "../output/gif.h"
#include "colorquantizer.h"
#include "../util/bitset.h"
#include "../util/process.h"

#include <string>

class Ditherer : public Process
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
class DithererFactory : public ProcessFactory<Ditherer>
{
    public: 
        DithererFactory();
};

extern DithererFactory dithererFactory;

#endif
