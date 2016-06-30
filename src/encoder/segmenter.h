#ifndef GTKMM_IMAGESEGMENTER_H
#define GTKMM_IMAGESEGMENTER_H

/**
 * Given a sequence of frames, decide which pixels should be updated at each frame
 */

#include <stdint.h>
#include <vector>

#include "../util/process.h"

#include "../video/videoframe.h"
#include "../util/bitset.h"

/**
 * A class to decide which pixels in each frame should be updated,
 * and what value they should be set to. Initialise via SegmenterFactory
 */
class Segmenter : public Process
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
        virtual void segment(const std::vector<pVideoFrame> frames, 
                             std::vector<pVideoFrame>& out_frames,
                             std::vector<pBitset>& out_bits) = 0;

    protected:
        Segmenter(std::string name, std::string description) :
            Process(name, description)
        {};
        void output_average(const std::vector<pVideoFrame> frames,
                            std::vector<pVideoFrame>& out_frames,
                            std::vector<pBitset>& out_bits);
};

/**
 * A factory for Segmenters
 */
class SegmenterFactory : public ProcessFactory<Segmenter>
{
    public:
        SegmenterFactory();
};

extern SegmenterFactory segmenterFactory;

#endif //GTKMM_IMAGESEGMENTER_H

