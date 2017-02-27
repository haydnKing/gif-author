#include "motion.h"

MotionDetector::MotionDetector(const vector<cv::Mat3b> &loop)
{
    if(loop.size() < 2)
        return;
    do_motion_detection(loop);
};

MotionDetector::~MotionDetector() {};


pMotionDetector MotionDetector::create(const vector<cv::Mat3b> &loop)
{
    return pMotionDetector(new MotionDetector(loop));
};

void MotionDetector::do_motion_detection(const vector<cv::Mat3b> &loop)
{
    cv::Ptr<cv::DualTVL1OpticalFlow> tvl1 = cv::createOptFlow_DualTVL1();

    cv::Mat3b prev, next;
    cv::cvtColor(loop.front(), prev, cv::COLOR_RGB2GRAY);

    cv::Mat_<cv::Point2f> the_flow;

    for(int i = 1; i < loop.size(); i++)
    {
        cv::cvtColor(loop[i], next, cv::COLOR_RGB2GRAY);
        tvl1->calc(prev, next, the_flow);
        cv::swap(prev, next);
        flow.push_back(the_flow);
    }
    //flow for final frame
    cv::cvtColor(loop[0], next, cv::COLOR_RGB2GRAY);
    tvl1->calc(prev, next, the_flow);
    flow.push_back(the_flow);
};
