#ifndef GTKMM_IMAGESEGMENTER_H
#define GTKMM_IMAGESEGMENTER_H

/**
 * Given a sequence of frames, decide which pixels should be updated at each frame
 */

#include "input/videoframe.h"
#include "settings.h"
#include <stdint.h>
#include <vector>

class Bitset;
typedef Glib::RefPtr<Bitset> pBitset;

class Bitset : public Glib::Object
{
    public:
        ~Bitset();
        static pBitset create(int _width, int _height, bool initial=false);

        bool get(int x, int y) const;
        void set(int x, int y, bool s=true);

        void clear(bool v=false);

        //remove isolated pixels
        void remove_islands();

    private:
        uint8_t *data;
        int width, height;
        Bitset(int _width, int _height, bool initial);
};



enum SegmentationMethod {
    SM_SIMPLE_DELTA,
};

class Segmenter : public Configurable
{
    public:
        ~Segmenter() {};
        static Segmenter *get_segmenter(SegmentationMethod method);

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
        Segmenter() {};
};


#endif //GTKMM_IMAGESEGMENTER_H

