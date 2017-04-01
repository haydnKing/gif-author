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
        ~DeltaSegmenter() {};

        static pSegmenter create() {
            return pSegmenter(new DeltaSegmenter());
        };

        void segment(pSequence frames);

        void name() const;
    protected:
        DeltaSegmenter() :
            Segmenter("delta", "Update pixels when only the next pixel changes by greater than delta"),
            delta(4.0),
            sigma(2.0),
            sigmaT(1.0)
        {
            add_option("delta", 
                "Don't update pixels that change by less than this value", 
                delta);
            add_option("sigma", 
                      "Amount to spacially blur by when calculating deltas", 
                      sigma);
            add_option("sigmaT", 
                        "Amount to temporally blur by when calculating deltas", 
                        sigmaT);
        };

        float delta, sigma, sigmaT;
};

void DeltaSegmenter::segment(pSequence frames)
{
    if(frames->size()==0) return;

    int x,y,z,start;
    float r,g,b,dr,dg,db;
    cv::Vec4b px_this;


    //blur frames
    pSequence bframes;
    bframes = frames->blur(sigma)->time_blur(sigmaT);

    //prepare output bits
    //No transparency in the first frame
    for(auto px = frames->at(0)->begin<cv::Vec4b>();
             px < frames->at(0)->end<cv::Vec4b>();
             px++)
    {
        (*px)[3] = 255;
    }
    //set full transparency in subsequent frames
    for(auto fr = ++frames->begin(); fr != frames->end(); fr++)
    {
        for(auto px = (*fr)->begin<cv::Vec4b>();
                 px < (*fr)->end<cv::Vec4b>();
                 px++)
        {
            (*px)[3] = 0;
        }
    }

    //Do delta
    for(y=0; y < frames->height(); y++)
    {
        for(x=0; x < frames->width(); x++)
        {
            px_this = frames->at(0)->at<cv::Vec4b>(y,x);
            r = px_this[0];
            g = px_this[1];
            b = px_this[2];
            start = 0;
            for(z=1; z < frames->size(); z++)
            {
                px_this = frames->at(z)->at<cv::Vec4b>(y,x);
                //have we jumped more than delta?
                dr = float(px_this[0]) - r/(z-start);
                dg = float(px_this[1]) - g/(z-start);
                db = float(px_this[2]) - b/(z-start);
                if(dr*dr + dg*dg + db*db > delta*delta)
                {
                    frames->at(start)->at<cv::Vec4b>(y,x)[3] = 255;
                    start = z;
                    r = g = b = 0.;
                }
                r += px_this[0];
                g += px_this[1];
                b += px_this[2];
            }
            frames->at(start)->at<cv::Vec4b>(y,x)[3] = 255;
        }
    }
};

/*******************************************************************
 *************************************************** NULL SEGMENTER
 *******************************************************************/

class NullSegmenter : public Segmenter
{
    public:
        ~NullSegmenter() {};

        static pSegmenter create() {
            return pSegmenter(new NullSegmenter());
        };

        void segment(pSequence frames)
        {
            //set each pixel in each frame to full opacity
            for(auto fr : *frames)
            {
                for(auto px = fr->begin<cv::Vec4b>();
                    px < fr->end<cv::Vec4b>();
                    px++)
                {
                    px[3] = 255;
                }
            }
        };
    protected:
        NullSegmenter() :
            Segmenter("null", "Update every pixel in every frame")
        {};
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
SegmenterFactory::SegmenterFactory(pSegmenter& value) : 
    FactoryOption("segmenter", "The segmenter decides which pixels in successive frames should be updated and which should be set to transparency. Setting more of the image to transparency improves the compressibility of the stream", value)
{
    auto def = DeltaSegmenter::create();
    add_group(def);
    add_group(NullSegmenter::create());
    value = def;
};

pOption SegmenterFactory::create(pSegmenter& value)
{
    return pOption(new SegmenterFactory(value));
};

