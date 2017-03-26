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


class Sequence;
typedef std::shared_ptr<Sequence> pSequence;


class Sequence : public std::vector<pFrame>
{
    public:
        virtual ~Sequence() {};

        /**
         * Initiate the image sequence from files
         */
        static pSequence from_filenames(const std::vector<std::string>& fnames, int delay);
        static pSequence from_filenames(const std::vector<std::string>& fnames, const vector<int>& delay);
        static pSequence from_frames(const std::vector<pFrame>& frames);
        
    private:
        Sequence(const std::vector<pFrame>& frames);
        Sequence(){};
};



#endif // GTKMM_GIFAUTHOR_IMAGESEQUENCE_H
