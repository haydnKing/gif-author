#include "segmenter.h"

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
 ******************************************** PER-PIXEL SEGMENTER
 *******************************************************************/

/*
 * Extract the timeline for individual pixels
 */
class PerPixelSegmenter : public Segmenter
{
    public:
        PerPixelSegmenter() {};
        ~PerPixelSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);

    protected:
        virtual void process_pixel(int length, uint8_t *in, uint8_t *out, bool *changed) = 0;

};

void PerPixelSegmenter::segment(const std::vector<pVideoFrame> frames, 
                                std::vector<pVideoFrame>& out_frames,
                                std::vector<pBitset>& out_bits)
{
    int x, y, z, w = frames[0]->get_width(), h = frames[0]->get_height();
    uint8_t *i_px = new uint8_t[3*frames.size()],
            *o_px = new uint8_t[3*frames.size()];
    bool *changed = new bool[frames.size()];
    const uint8_t *px;

    for(z = 0; z < frames.size(); z++)
    {
        out_frames.push_back(VideoFrame::create(w,h));
        out_bits.push_back(BitSet::create(w,h));
    }

    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            //get the original pixels
            for(z=0; z < frames.size(); z++)
            {
                px = frames[z]->get_pixel(x,y);
                i_px[3*z  ] = px[0];
                i_px[3*z+1] = px[1];
                i_px[3*z+2] = px[2];
            }

            process_pixel(frames.size(), i_px, o_px, changed);

            //set the processed pixels and bits
            for(z = 0; z < frames.size(); z++)
            {
                px = out_frames[z]->get_pixel(x,y);
                px[0] = o_px[3*z  ];
                px[1] = o_px[3*z+1];
                px[2] = o_px[3*z+2];
                out_bits[z]->set_bit(x,y,changed[z]);
            }

        }
    }

    delete [] i_px;
    delete [] o_px;
    delete [] changed;
};
