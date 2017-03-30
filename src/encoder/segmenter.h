#ifndef GTKMM_IMAGESEGMENTER_H
#define GTKMM_IMAGESEGMENTER_H

/**
 * Given a sequence of frames, decide which pixels should be updated at each frame
 */

#include <stdint.h>
#include <vector>
#include <memory>

#include "../util/cline.h"

#include "../video/imagesequence.h"
#include "../util/bitset.h"

/**
 * A class to decide which pixels in each frame should be updated,
 * and what value they should be set to. Initialise via SegmenterFactory
 */
class Segmenter : public OptionGroup
{
    public:
        ~Segmenter() {};

        /**
         * Segment the frames
         * @param frames the sequence of frames to segment
         * @param out_frames the resulting frames. only pixels where the
         * corresponding bit in out_bits is high are valid. If out_frames is left empty, then the original frames should be used
         * @param out_bits bitfield indicating which pixels in each frame are 
         * to be updated.
         */
        virtual void segment(pSequence frames) = 0;

    protected:
        Segmenter(std::string name, std::string description) :
            OptionGroup(name, description)
        {};
};

typedef std::shared_ptr<Segmenter> pSegmenter;

/**
 * A factory for Segmenters
 */
class SegmenterFactory : public FactoryOption<Segmenter>
{
    public:
        static pOption create(pSegmenter& value);

    protected:        
        SegmenterFactory(pSegmenter& value);
};


#endif //GTKMM_IMAGESEGMENTER_H

