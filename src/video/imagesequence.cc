#include "imagesequence.h"

Frame::Frame(const std::string& filename, int delay):
    delay(delay)
{
    cv::Mat image;
    image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    if(!image.data) {
        throw "No image data loaded from \"" + filename + "\"";
    }
    cv::cvtColor(image, &this, CV_BGR2RGBA);
};

pFrame Frame::from_file(const std::string& filename, int delay)
{
    return pFrame(new Frame(filename, delay));
};

int Frame::get_delay() const
{
    return delay;
};

void Frame::set_delay(int d)
{
    delay = d;
};

ImageSequence::ImageSequence(const std::vector<pFrame>& frames)
    std::vector<pFrame>(frames)
{};

pImageSequence ImageSequence::from_filenames(const std::vector<std::string>& fnames, int delay)
{
    pIS = pImageSequence(new ImageSequence());
    for(auto f: fnames)
    {
        pIS->push_back(Frame::from_file(f, delay));
    }
    return pIS;
};

pImageSequence ImageSequence::from_filenames(const std::vector<std::string>& fnames, const vector<int>& delays)
{
    pIS = pImageSequence(new ImageSequence());
    for(int i = 0; i < fnames.size(); i++)
    {
        pIS->push_back(Frame::from_file(fnames[i], delays[i]));
    }
    return pIS;
};

pImageSequence ImageSequence::from_frames(const std::vector<pFrame>& frames)
{
    return pImageSequence(new ImageSequence(frames));
}
