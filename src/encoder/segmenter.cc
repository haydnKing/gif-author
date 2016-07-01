#include "segmenter.h"
#include <cstring>


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
    int x,y,z,start;
    float r,g,b,dr,dg,db;
    uint8_t *px_start, *px_this;


    //blur frames
    std::vector<pVideoFrame> bframes;
    for(auto it : frames)
    {
        bframes.push_back(it->blur(sigma));
    }

    //prepare output bits
    //No transparency in the first frame
    out_bits.push_back(Bitset::create(frames[0]->get_width(), frames[0]->get_height(), true));
    for(z=1; z < frames.size(); z++)
        out_bits.push_back(Bitset::create(frames[0]->get_width(), frames[0]->get_height(), false));

    //prepare output frames
    for(z=0; z < frames.size(); z++)
        out_frames.push_back(VideoFrame::create(frames[z]->get_width(), frames[z]->get_height()));
    

    //Do delta
    for(y=0; y < frames[0]->get_height(); y++)
    {
        for(x=0; x < frames[0]->get_width(); x++)
        {
            px_this = frames[0]->get_pixel(x,y);
            r = px_this[0];
            g = px_this[1];
            b = px_this[2];
            start = 0;
            px_start = px_this;
            for(z=1; z < frames.size(); z++)
            {
                px_this = frames[z]->get_pixel(x,y);
                //have we jumped more than delta?
                dr = float(px_this[0]) - r/(z-start);
                dg = float(px_this[1]) - g/(z-start);
                db = float(px_this[2]) - b/(z-start);
                if(dr*dr + dg*dg + db*db > delta*delta)
                {
                    out_bits[start]->set(x,y);
                    px_start[0] = uint8_t(0.5+r/(z-start));
                    px_start[1] = uint8_t(0.5+g/(z-start));
                    px_start[2] = uint8_t(0.5+b/(z-start));
                    px_start = px_this;
                }
                r += px_this[0];
                g += px_this[1];
                b += px_this[2];
            }
            out_bits[start]->set(x,y);
            px_start[0] = uint8_t(0.5+r/(z-start));
            px_start[1] = uint8_t(0.5+g/(z-start));
            px_start[2] = uint8_t(0.5+b/(z-start));
        }
    }
};

SegmenterFactory::SegmenterFactory() : 
    ProcessFactory("segmenter", "The segmenter decides which pixels in successive frames should be updated and which should be set to transparency. Setting more of the image to transparency improves the compressibility of the stream")
{
    register_type("SimpleDelta", new DeltaSegmenter());
};

SegmenterFactory segmenterFactory;
