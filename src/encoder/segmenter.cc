#include "segmenter.h"
#include <cstring>

//#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

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
                        "Amount to spacially blur by when calculating deltas", 
                        2.0));
            add_setting(new PositiveFloatSetting("sigmaT", 
                        "Amount to temporally blur by when calculating deltas", 
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
    float sigmaT= get_setting("sigmaT")->get_float();

    int x,y,z,start;
    float r,g,b,dr,dg,db;
    uint8_t *px_start, *px_this;


    //blur frames
    std::vector<pVideoFrame> bframes;
    for(auto it : frames)
    {
        bframes.push_back(it->blur(sigma));
    }
    bframes = VideoFrame::blur(bframes, sigmaT);

    //prepare output bits
    //No transparency in the first frame
    out_bits.push_back(pBitset());//Bitset::create(frames[0]->get_width(), frames[0]->get_height(), true));
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
            for(z=1; z < frames.size(); z++)
            {
                px_this = frames[z]->get_pixel(x,y);
                //have we jumped more than delta?
                dr = float(px_this[0]) - r/(z-start);
                dg = float(px_this[1]) - g/(z-start);
                db = float(px_this[2]) - b/(z-start);
                if(dr*dr + dg*dg + db*db > delta*delta)
                {
                    if(start != 0) out_bits[start]->set(x,y);
                    out_frames[start]->set_pixel(x,y,
                        uint8_t(0.5+r/(z-start)),
                        uint8_t(0.5+g/(z-start)),
                        uint8_t(0.5+b/(z-start)));
                    start = z;
                    r = g = b = 0.;
                }
                r += px_this[0];
                g += px_this[1];
                b += px_this[2];
            }
            if(start != 0) out_bits[start]->set(x,y);
            out_frames[start]->set_pixel(x,y,
                uint8_t(0.5+r/(z-start)),
                uint8_t(0.5+g/(z-start)),
                uint8_t(0.5+b/(z-start)));
        }
    }
};

/*******************************************************************
 *************************************************** NULL SEGMENTER
 *******************************************************************/

class NullSegmenter : public Segmenter
{
    public:
        NullSegmenter() :
            Segmenter("NullSegmenter", "Update every pixel in every frame")
        {};
        ~NullSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits)
        {
            for(auto fr : frames)
            {
                out_frames.push_back(fr->copy());
                out_bits.push_back(Bitset::create(fr->get_width(), fr->get_height(), true));
            }
        };
};


/*******************************************************************
 *************************************************** MOTION SEGMENTER
 *******************************************************************/

class MotionSegmenter : public Segmenter
{
    public:
        MotionSegmenter() :
            Segmenter("MotionSegmenter", "Update pixels when they move")
        {
        };
        ~MotionSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits)
        {
            if(frames.size() == 0) return;
            out_frames.push_back(frames[0]->copy());
            out_bits.push_back(pBitset());
            
            cv::Ptr<cv::DualTVL1OpticalFlow> tvl1 = cv::createOptFlow_DualTVL1();
            cv::Mat_<cv::Point2f> flow;
            cv::Mat prev, next;
            cv::cvtColor(*(frames[0]->get_mat()), prev, cv::COLOR_RGB2GRAY);
            pBitset b;
            int x,y,z,dx,dy;

            int count;
            
            for(z = 1; z < frames.size(); z++) {
                std::cout << "z = " << z << std::endl;
                count  = 0;
                out_frames.push_back(frames[z]->copy());
                cv::cvtColor(*(frames[z]->get_mat()), next, cv::COLOR_RGB2GRAY);
                tvl1->calc(prev, next, flow);
                cv::swap(prev, next);

                //create a blank bitset
                b = Bitset::create(frames[z]->get_width(), frames[z]->get_height(), false);
                //set bits where the pixel moved more than a pixel
                for(y = 0; y < frames[z]->get_height(); y++)
                {
                    for(x = 0; x < frames[z]->get_width(); x++)
                    {
                        dx = int(flow.at<cv::Point2f>(y,x).x+0.5);
                        dy = int(flow.at<cv::Point2f>(y,x).y+0.5);
                        if(dx > 0 || dy > 0)
                        {
                            b->set(x,y);
                            count++;
                            dx += x;
                            dy += y;
                            if(dx >= 0 && dx < frames[z]->get_width() &&
                               dy >= 0 && dy < frames[z]->get_height())
                            {
                                b->set(dx,dy);
                            }
                        }
                    }
                }
                out_bits.push_back(b);
                std::cout << "  count = " << count << std::endl;
            }

        };
};

SegmenterFactory::SegmenterFactory() : 
    ProcessFactory("segmenter", "The segmenter decides which pixels in successive frames should be updated and which should be set to transparency. Setting more of the image to transparency improves the compressibility of the stream")
{
    register_type("SimpleDelta", new DeltaSegmenter());
    register_type("NullSegmenter", new NullSegmenter());
    register_type("MotionSegmenter", new MotionSegmenter());
};

SegmenterFactory segmenterFactory;
