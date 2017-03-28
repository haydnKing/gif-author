#include "imagesequence.h"

Frame::Frame(const std::string& filename, int delay):
    the_delay(delay)
{
    cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    if(!image.data) {
        throw "No image data loaded from \"" + filename + "\"";
    }
    cv::cvtColor(image, my_mat, CV_BGR2RGBA);
};
        
Frame::Frame(int width, int height, int delay):
    my_mat(height, width, CV_8UC4),
    the_delay(delay)
{};

Frame::Frame(const cv::Mat& mat, int delay):
    my_mat(mat),
    the_delay(delay)
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

int Frame::delay() const
{
    return the_delay;
};

void Frame::delay(int d)
{
    the_delay = d;
};

int Frame::width() const
{
    return mat().cols;
};

int Frame::height() const
{
    return mat().rows;
};
        
pFrame Frame::resize(int width, int height, int interpolation) const
{
    pFrame out = Frame::create(width, height, delay());
    cv::resize(mat(), out->mat(), out->mat().size(), 0, 0, interpolation);
    return out;
}

pFrame Frame::crop(int x, int y, int width, int height) const
{
    return Frame::create(mat()(cv::Rect(x,y,width,height)), delay());
};

pFrame Frame::blur(float sigma) const
{
    pFrame out = Frame::create(width(), height(), delay());
    int ks = int(3.0*sigma+1.0);
    if(ks % 2 == 0) ks++;
    cv::GaussianBlur(mat(), out->mat(), cv::Size(ks,ks), sigma);
    return out;
};

Sequence::Sequence(const std::vector<pFrame>& frames):
    std::vector<pFrame>(frames)
{};

pSequence Sequence::create_from_filenames(const std::vector<std::string>& fnames, int delay)
{
    pSequence pS = Sequence::create();
    for(auto f: fnames)
    {
        pS->push_back(Frame::create_from_file(f, delay));
    }
    return pS;
};

pSequence Sequence::create_from_filenames(const std::vector<std::string>& fnames, const vector<int>& delays)
{
    pSequence pS = Sequence::create();
    for(int i = 0; i < fnames.size(); i++)
    {
        pS->push_back(Frame::create_from_file(fnames[i], delays[i]));
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

pSequence Sequence::blur(float sigma) const
{
    pSequence r = Sequence::create();
    for(auto it : *this)
    {
        r->push_back(it->blur(sigma));
    }
    return r;
};

pSequence Sequence::time_blur(float sigma) const
{
    pSequence r = Sequence::create();
    //create an array of higher bit-depth copies
    //for each position
    //  calculate normalised kernel
    //  sum output
    //  convert output back to standard bit-depth & store
    //return stored images
    return r;
};

float *Sequence::get_kernel(float sigma, int kernel_center)
{
    float *kernel = new float[kernel_center*2+1];
    for(int k = 0; k < kernel_center*2+1; k++)
        kernel[k] = std::exp(-(k-kernel_center)*(k-kernel_center)/(2*sigma*sigma));

    return kernel;
};
