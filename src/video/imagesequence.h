#ifndef GTKMM_GIFAUTHOR_IMAGESEQUENCE_H
#define GTKMM_GIFAUTHOR_IMAGESEQUENCE_H

#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

class Frame;
typedef std::shared_ptr<Frame> pFrame;

class Frame : public cv::Mat
{
    public:
        virtual ~Frame() {};

        static pFrame from_file(const std::string& filename, int delay=0);

        int get_delay() const;
        void set_delay(int d);

    private:
        Frame(const std::string& filename, int delay);

    protected:
        int delay;
};


class ImageSequence;
typedef std::shared_ptr<ImageSequence> pImageSequence;


class ImageSequence : public std::vector<pFrame>
{
    public:
        virtual ~ImageSequence() {};

        /**
         * Initiate the image sequence from files
         */
        static pImageSequence from_filenames(const std::vector<std::string>& fnames, int delay);
        static pImageSequence from_filenames(const std::vector<std::string>& fnames, const vector<int>& delay);
        static pImageSequence from_frames(const std::vector<pFrame>& frames);
        
    private:
        ImageSequence(const std::vector<pFrame>& frames);
        ImageSequence(){};
};



#endif // GTKMM_GIFAUTHOR_IMAGESEQUENCE_H
