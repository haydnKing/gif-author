#include "segmenter.h"
#include <cstring>

void Segmenter::output_average(const std::vector<pVideoFrame> frames,
                               std::vector<pVideoFrame>& out_frames,
                               std::vector<pBitset>& out_bits)
{
    double r,g,b;
    int x,y,z,last_update;
    uint8_t *px;

    //create output data
    for(z = 0; z < frames.size(); z++)
    {
        out_frames.push_back(VideoFrame::create(frames[z]->get_width(), frames[z]->get_height()));
    }

    for(y = 0; y < frames[0]->get_height(); y++)
    {
        for(x = 0; x < frames[0]->get_width(); x++)
        {
            px = frames[0]->get_pixel(x,y);
            r = px[0];
            g = px[1];
            b = px[2];
            last_update = 0;
            for(z = 1; z < frames.size(); z++)
            {
                if(out_bits[z]->get(x,y))
                {
                    px = out_frames[last_update]->get_pixel(x,y);
                    px[0] = uint8_t(0.5 + r / (z-last_update));
                    px[1] = uint8_t(0.5 + g / (z-last_update));
                    px[2] = uint8_t(0.5 + b / (z-last_update));
                    r = g = b = 0.;
                    last_update = z;
                }   
                px = frames[z]->get_pixel(x,y);
                r += px[0];
                g += px[1];
                b += px[2];
            }
            px = out_frames[last_update]->get_pixel(x,y);
            px[0] = uint8_t(0.5 + r / (z-last_update));
            px[1] = uint8_t(0.5 + g / (z-last_update));
            px[2] = uint8_t(0.5 + b / (z-last_update));
        }
    }


};

/*******************************************************************
 *************************************************** DELTA SEGMENTER
 *******************************************************************/

class DeltaSegmenter : public Segmenter
{
    public:
        DeltaSegmenter() :
            Segmenter("DeltaSegmenter", "Update pixels when the next pixel changes by greater than delta")
        {
            add_setting(new PositiveFloatSetting("delta", 
                "Don't update pixels that change by less than this value", 
                4.0));
            add_setting(new PositiveFloatSetting("sigma", 
                        "Amount to pre-blur by when calculating deltas", 
                        1.0));
        };
        ~DeltaSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);
};

void DeltaSegmenter::segment(const std::vector<pVideoFrame> frames, 
                             std::vector<pVideoFrame>& out_frames,
                             std::vector<pBitset>& out_bits)
{
    float delta = get_setting("delta")->get_float();
    float sigma = get_setting("sigma")->get_float();

    //No transparency in the first frame
    out_bits.push_back(pBitset());

    std::vector<pVideoFrame> bframes;
    for(auto it : frames)
    {
        bframes.push_back(it->blur(sigma));
    }

    //cycle through each frame
    int i, x,y;
    pBitset out;
    uint8_t *px_this, *px_prev;
    for(i=1; i < frames.size(); i++)
    {
        out = Bitset::create(frames[i]->get_width(), frames[i]->get_height(), false);
        for(y=0; y<frames[i]->get_height(); y++)
        {
            for(x=0; x < frames[i]->get_width(); x++)
            {
                px_this = bframes[i]->get_pixel(x,y);
                px_prev = bframes[i-1]->get_pixel(x,y);
                if(((px_this[0] - px_prev[0])*(px_this[0] - px_prev[0])+ 
                    (px_this[1] - px_prev[1])*(px_this[1] - px_prev[1])+ 
                    (px_this[2] - px_prev[2])*(px_this[2] - px_prev[2])) > delta*delta)
                    out->set(x,y,true);
            }
        }
        out_bits.push_back(out);
    }

    output_average(frames, out_frames, out_bits);
};

    

SegmenterFactory::SegmenterFactory() : 
    ProcessFactory("segmenter", "The segmenter decides which pixels in successive frames should be updated and which should be set to transparency. Setting more of the image to transparency improves the compressibility of the stream")
{
    register_type("SimpleDelta", new DeltaSegmenter());
};

SegmenterFactory segmenterFactory;
