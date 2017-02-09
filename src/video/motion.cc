#include "motion.h"

MotionDetector::MotionDetector(const vector<cv::Mat3b> &loop)
{
    //Hard stuff goes here
};

MotionDetector::~MotionDetector() {};


pMotionDetector MotionDetector::create(const vector<cv::Mat3b> &loop)
{
    return pMotionDetector(new MotionDetector(loop));
};

