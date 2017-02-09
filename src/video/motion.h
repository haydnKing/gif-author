#ifndef GIFAUTHOR_MOTION_H
#define GIFAUTHOR_MOTION_H

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

using namespace std;

class MotionDetector;
typedef shared_ptr<MotionDetector> pMotionDetector;

/**
 * Calculate optical flow for a series of looping images
 */
class MotionDetector
{
    public:
        virtual ~MotionDetector();

        static pMotionDetector create(const vector<cv::Mat3b> &loop);

    private:
        MotionDetector(const vector<cv::Mat3b> &loop);
};

#endif
