#ifndef GTKMM_GIFAUTHOR_IMAGESEQUENCE_H
#define GTKMM_GIFAUTHOR_IMAGESEQUENCE_H

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class Frame;
typedef std::shared_ptr<Frame> pFrame;

class Frame 
{
    public:
        virtual ~Frame() {};

        static pFrame create(int width, int height, int delay=0);
        static pFrame create(const cv::Mat& mat, int delay=0);
        static pFrame create_from_file(const std::string& filename, int delay=0);

        int get_delay() const;
        void set_delay(int d);

        const cv::Mat mat() const {return my_mat;}; 

        pFrame resize(int width, int height, int interpolation=cv::INTER_AREA) const;
        pFrame crop(int x, int y, int width, int height) const;

    private:
        Frame(const std::string& filename, int delay);
        Frame(int width, int height, int delay);
        Frame(const cv::Mat& mat, int delay);

    protected:
        int delay;
        cv::Mat my_mat;
};


class Sequence;
typedef std::shared_ptr<Sequence> pSequence;


class Sequence : public std::vector<pFrame>
{
    public:
        virtual ~Sequence() {};

        /**
         * Initiate the image sequence from files
         */
        static pSequence create_from_filenames(const std::vector<std::string>& fnames, int delay);
        static pSequence create_from_filenames(const std::vector<std::string>& fnames, 
                                               const vector<int>& delay);
        static pSequence create_from_frames(const std::vector<pFrame>& frames);
        static pSequence create();

        /*
         * batch resize
         */
        pSequence resize(int width, int height) const;
        /*
         * batch crop
         */
        pSequence crop(int x, int y, int width, int height) const;
        
    private:
        Sequence(const std::vector<pFrame>& frames);
        Sequence(){};
};



#endif // GTKMM_GIFAUTHOR_IMAGESEQUENCE_H
