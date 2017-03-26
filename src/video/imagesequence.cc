#include "imagesequence.h"

Frame::Frame(const std::string& filename, int delay):
    delay(delay)
{
    cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    if(!image.data) {
        throw "No image data loaded from \"" + filename + "\"";
    }
    cv::cvtColor(image, my_mat, CV_BGR2RGBA);
};
        
Frame::Frame(int width, int height, int delay):
    my_mat(height, width, CV_8UC4),
    delay(delay)
{};

Frame::Frame(const cv::Mat& mat, int delay):
    my_mat(mat),
    delay(delay)
{};

pFrame Frame::create_from_file(const std::string& filename, int delay)
{
    return pFrame(new Frame(filename, delay));
};

pFrame Frame::create(int width, int height, int delay)
{
    return pFrame(new Frame(width, height, delay));
};

pFrame Frame::create(const cv::Mat& mat, int delay)
{
    return pFrame(new Frame(mat, delay));
};

int Frame::get_delay() const
{
    return delay;
};

void Frame::set_delay(int d)
{
    delay = d;
};
        
pFrame Frame::resize(int width, int height, int interpolation) const
{
    pFrame out = Frame::create(width, height, get_delay());
    cv::resize(mat(), out->mat(), out->size(), 0, 0, interpolation);
    return out;
}

pFrame Frame::crop(int x, int y, int width, int height) const
{
    return Frame::create(mat()(cv::Rect(x,y,width,height)), get_delay());
};

Sequence::Sequence(const std::vector<pFrame>& frames):
    std::vector<pFrame>(frames)
{};

pSequence Sequence::create_from_filenames(const std::vector<std::string>& fnames, int delay)
{
    pS = pSequence(new Sequence());
    for(auto f: fnames)
    {
        pS->push_back(Frame::from_file(f, delay));
    }
    return pS;
};

pSequence Sequence::create_from_filenames(const std::vector<std::string>& fnames, const vector<int>& delays)
{
    pS = pSequence(new Sequence());
    for(int i = 0; i < fnames.size(); i++)
    {
        pS->push_back(Frame::from_file(fnames[i], delays[i]));
    }
    return pS;
};

pSequence Sequence::create_from_frames(const std::vector<pFrame>& frames)
{
    return pSequence(new Sequence(frames));
};

pSequence Sequence::create()
{
    return pSequence(new Sequence());
};
        
pSequence Sequence::resize(int width, int height) const
{
    pSequence r = Sequence::create();
    for(auto it = begin(); it < end(); it++)
    {
        r->push_back((*it)->resize(width, height));
    }
    return r;
};
        
pSequence Sequence::crop(int x, int y, int width, int height) const
{
    pSequence r = Sequence::create();
    for(auto it = begin(); it < end(); it++)
    {
        r->push_back((*it)->crop(x, y, width, height));
    }
    return r;
};

