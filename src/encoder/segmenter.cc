#include "segmenter.h"
#include <cstring>

//#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

void Segmenter::segment(const std::vector<pVideoFrame> frames, 
                             std::vector<pVideoFrame>& out_frames,
                             std::vector<pBitset>& out_bits) {
    get_update_bits(frames, out_bits);
    int x,y,z,start;
    double r,g,b;
    uint8_t *px;

    //smooth the out bits
    for(z = 0; z < frames.size(); z++)
    {
        if(out_bits[z])
            out_bits[z] = Bitset::smooth(out_bits[z], 2, 0.5);
    }

    //calculate the value for each out frame
    for(z = 0; z < frames.size(); z++)
    {
        out_frames.push_back(VideoFrame::create(frames[z]->get_width(), frames[z]->get_height()));
    }
    for(y = 0; y < frames[0]->get_height(); y++)
    {
        for(x = 0; x < frames[0]->get_width(); x++)
        {
            start = 0;
            px = frames[0]->get_pixel(x,y);
            r = px[0];
            g = px[1];
            b = px[2];

            for(z = 1; z < frames.size(); z++)
            {
                if(!out_bits[z] || out_bits[z]->get(x,y)) {
                    r /= z-start;
                    g /= z-start;
                    b /= z-start;
                    out_frames[start]->set_pixel(x,y,uint8_t(r+0.5),uint8_t(g+0.5),uint8_t(b+0.5));
                    r = g = b = 0.;
                    start = z;
                }
                px = frames[z]->get_pixel(x,y);
                r += px[0];
                g += px[1];
                b += px[2];
            }
            r /= z-start;
            g /= z-start;
            b /= z-start;
            out_frames[start]->set_pixel(x,y,uint8_t(r+0.5),uint8_t(g+0.5),uint8_t(b+0.5));
        }
    }
}

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

        void get_update_bits(const std::vector<pVideoFrame> frames, 
                     std::vector<pBitset>& out_bits);
};

void DeltaSegmenter::get_update_bits(const std::vector<pVideoFrame> frames, 
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
    out_bits.push_back(pBitset());
    for(z=1; z < frames.size(); z++)
        out_bits.push_back(Bitset::create(frames[0]->get_width(), frames[0]->get_height(), false));


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
                    if(out_bits[start]) out_bits[start]->set(x,y);
                    start = z;
                    r = g = b = 0.;
                }
                r += px_this[0];
                g += px_this[1];
                b += px_this[2];
            }
            if(out_bits[start]) out_bits[start]->set(x,y);
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

        void get_update_bits(const std::vector<pVideoFrame> frames, 
                     std::vector<pBitset>& out_bits)
        {
            for(auto fr : frames)
            {
                out_bits.push_back(Bitset::create(fr->get_width(), fr->get_height(), true));
            }
        };
};


/*******************************************************************
 *************************************************** MOTION SEGMENTER
 *******************************************************************/
/*
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
            
            cv::Ptr<cv::DualTVL1OpticalFlow> tvl1 = cv::createOptFlow_DualTVL1();
            cv::Mat_<cv::Point2f> flow;
            cv::Mat prev, next;
            cv::cvtColor(*(frames[0]->get_mat()), prev, cv::COLOR_RGB2GRAY);
            pBitset b;
            int x,y,z,dx,dy;

            out_frames.push_back(frames[0]->copy());
            out_bits.push_back(Bitset::create(frames[z]->get_width(), frames[z]->get_height(), true));
            for(z = 1; z < frames.size(); z++) 
                out_bits.push_back(Bitset::create(frames[z]->get_width(), frames[z]->get_height(), false));
            int count;

            pVideoFrame dbg = VideoFrame::create(frames[0]->get_width(), frames[0]->get_height());
            std::stringstream ss;
            
            for(z = 1; z < frames.size(); z++) {
                std::cout << "z = " << z << std::endl;
                count  = 0;
                float max_x = 0., max_y = 0.;
                out_frames.push_back(frames[z]->copy());
                cv::cvtColor(*(frames[z]->get_mat()), next, cv::COLOR_RGB2GRAY);
                tvl1->calc(prev, next, flow);
                cv::swap(prev, next);

                //set bits where the pixel moved more than a pixel
                for(y = 0; y < frames[z]->get_height(); y++)
                {
                    for(x = 0; x < frames[z]->get_width(); x++)
                    {
                        if(flow.at<cv::Point2f>(y,x).x > max_x) max_x = flow.at<cv::Point2f>(y,x).x;
                        if(flow.at<cv::Point2f>(y,x).y > max_y) max_y = flow.at<cv::Point2f>(y,x).y;
                        dx = int(flow.at<cv::Point2f>(y,x).x+0.5);
                        dy = int(flow.at<cv::Point2f>(y,x).y+0.5);
                        dbg->set_pixel(x,y, uint8_t(std::abs(dx)), uint8_t(std::abs(dy)), 0);
                        if(dx != 0 || dy != 0)
                        {
                            out_bits[z-1]->set(x,y);
                            count++;
                            dx += x;
                            dy += y;
                            if(dx >= 0 && dx < frames[z]->get_width() &&
                               dy >= 0 && dy < frames[z]->get_height())
                            {
                                out_bits[z]->set(dx,dy);
                                //if(z +1 < frames.size()) out_bits[z+1]->set(dx,dy);
                            }
                        }
                    }
                }
                std::cout << "  count = " << count << std::endl;
                std::cout << "  max = (" << max_x << ", " << max_y << ")" << std::endl;
                
                ss.str("");
                ss << "dbg/motion_" << std::setw(4) << std::setfill('0') << z << ".ppm";
                dbg->write_ppm(ss.str().c_str());
            }

        };
};
*/
SegmenterFactory::SegmenterFactory() : 
    ProcessFactory("segmenter", "The segmenter decides which pixels in successive frames should be updated and which should be set to transparency. Setting more of the image to transparency improves the compressibility of the stream")
{
    register_type("SimpleDelta", new DeltaSegmenter());
    register_type("NullSegmenter", new NullSegmenter());
 //   register_type("MotionSegmenter", new MotionSegmenter());
};

SegmenterFactory segmenterFactory;
