#include "imagesegmenter.h"

class DeltaSegmenter;
class SmoothDeltaSegmenter;

Segmenter *Segmenter::get_segmentor(SegmentationMethod method)
{
    Segmenter *ret;
    switch(method)
    {
        case SM_SIMPLE_DELTA:
            ret = new DeltaSegmenter();
    }
    return ret;
};

/*******************************************************************
 *************************************************** DELTA SEGMENTER
 *******************************************************************/

class DeltaSegmenter : public Segmenter
{
    public:
        DeltaSegmenter() {};
        ~DeltaSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);
};

void DeltaSegmenter::segment(const std::vector<pVideoFrame> frames, 
                             std::vector<pVideoFrame>& out_frames,
                             std::vector<pBitset>& out_bits)
{
    //worry about where to get settings later
    float delta = 2.0;

    //No transparency in the first frame
    out_bits.push_back(pBitset());

    //cycle through each frame
    int i, x,y;
    pBitset out;
    uint8_t *px_this, *px_prev;
    for(i=1; i < frames.size(); i++)
    {
        out = pBitset::create(frames[i]->get_width(), frames[i]->get_height(), false);
        for(y=0; y<frames[i]->get_height(); y++)
        {
            for(x=0; x < frames[i]->get_width(); x++)
            {
                px_this = frames[i]->get_pixel(x,y);
                px_prev = frames[i-1]->get_pixel(x,y);
                if(std::abs(px_this[0] - px_prev[0]) > delta ||
                   std::abs(px_this[1] - px_prev[1]) > delta ||
                   std::abs(px_this[2] - px_prev[2]) > delta)
                    out->set(x,y,true);
            }
        }
        out_bits.push_back(out);
    }
};
    
/*******************************************************************
 ******************************************** SMOOTH DELTA SEGMENTER
 *******************************************************************/

class SmoothDeltaSegmenter : public Segmenter
{
    public:
        SmoothDeltaSegmenter() {};
        ~SmoothDeltaSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);

    protected:
        float get_num_changed(pVideoFrame lhs, pVideoFrame rhs, delta);
};
