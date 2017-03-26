#ifndef GTKMM_GIFAUTHOR_VIDEOFRAME_H
#define GTKMM_GIFAUTHOR_VIDEOFRAME_H

#include <stdint.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <memory>

#include <opencv2/opencv.hpp>

#include <iostream>


//forward declaration
class VideoFrame;
typedef std::shared_ptr<VideoFrame> pVideoFrame;

/**
 * Hold a frame of video, a wrapper for cv::Mat
 */
class VideoFrame 
{
    public:
        virtual ~VideoFrame();
        /*
         * Create an empty Frame
         */
        static pVideoFrame create();

        /**
         * Initiate to a copy of the given mat
         */
        static pVideoFrame create_from_mat(const cv::Mat& mat, int64_t delay=0);
        static pVideoFrame create_from_file(const std::string& fname, int64_t delay=0);
        static pVideoFrame create(int width, int height, uint8_t initial=0, int64_t delay=0);

        /**
         * create a copy of the data
         */
        pVideoFrame copy() const;

        /**
         * Extract the underlying cv::Mat
         */
        cv::Mat get_mat();

        /**
         * Extract the underlying cv::Mat
         */
        const cv::Mat& get_mat() const;

        /*
         * frame height
         */
        int get_height() const;
        /*
         * frame width
         */
        int get_width() const;

        /*
         * how long to display the frame in ms
         */
        int64_t get_delay() const;

        bool is_ok() const;

        /*
         * Write a debug PPM image to file
         */
        void write_ppm(const char *fname) const;

        /*
         * Write a series of debug PPM images to file
         */
        static void write_ppm(const std::vector<pVideoFrame> frames, const char *head);


        // ############################################## Operations

        /**
         * Return a crop of the VideoFrame 
         * @param left 
         * @param right
         * @param width pass -1 to preserve image
         * @param height pass -1 to preserve image
         * @returns the new frame, a subset of the current frame, or NULL if 
         *          parameters were out of bounds
         */
        pVideoFrame crop(int left, int top, int width, int height);

        /**
         * Return a scaled version of the VideoFrame
         * @param width target width
         * @param height target height
         * @returns the new frame
         */
        pVideoFrame scale_to(int width, int height) const;

        pVideoFrame blur(float sigma) const;

        static std::vector<pVideoFrame> blur(const std::vector<pVideoFrame> &rhs, float sigma);

    protected:
        VideoFrame();

    private:
        cv::Mat image;
        int64_t delay;
};


#endif // GTKMM_GIFAUTHOR_VIDEOFRAME_H
