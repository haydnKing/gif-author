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
                               ColorQuantizer *cq,
                               uint8_t colors) const;

    protected:

        virtual void _dither_image(pGIFImage out,
                                   const pVideoFrame vf,
                                   const pBitset mask,
                                   ColorQuantizer *cq,
                                   uint8_t colors) const = 0;
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
