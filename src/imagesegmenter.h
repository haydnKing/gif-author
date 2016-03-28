#ifndef GTKMM_IMAGESEGMENTER_H
#define GTKMM_IMAGESEGMENTER_H

/**
 * Given a sequence of frames, decide which pixels should be updated at each frame
 */

#include "input/videoframe.h"
#include <stdint.h>
#include <vector>

enum SegmentationMethod {
    SM_SIMPLE_DELTA,
    SM_SMOOTH_DELTA,
};

class Segmenter
{
    public:
        ~Segmenter() {};
        static Segmenter *get_segmentor(SegmentationMethod method);

        /**
         * Segment the frames
         * @param frames the sequence of frames to segment
         * @param out_frames the resulting frames. only pixels where the
         * corresponding bit in out_bits is high are valid. If out_frames is left empty, then the original frames should be used
         * @param out_bits bitfield indicating which pixels in each frame are 
         * to be updated.
         */
        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits) = 0;

    protected:
        Segmenter() {};
};


#endif //GTKMM_IMAGESEGMENTER_H

