#include "imagesequence.h"

Frame::Frame(const std::string& filename, int delay):
    the_delay(delay)
{
    cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
    if(!image.data) {
        throw "No image data loaded from \"" + filename + "\"";
    }
    cv::cvtColor(image, *this, CV_BGR2RGBA);
};
        
Frame::Frame(int width, int height, int delay, int depth):
    cv::Mat(height, width, depth),
    the_delay(delay)
{};

Frame::Frame(const cv::Mat& mat, int delay):
    cv::Mat(mat),
    the_delay(delay)
{};

pFrame Frame::create_from_file(const std::string& filename, int delay)
{
    return pFrame(new Frame(filename, delay));
};

pFrame Frame::create(int width, int height, int delay, int depth)
{
    return pFrame(new Frame(width, height, delay, depth));
};

pFrame Frame::create(const cv::Mat& mat, int delay)
{
    return pFrame(new Frame(mat, delay));
};

pFrame Frame::create(const pFrame& rhs, const cv::Rect& roi)
{
    return pFrame(new Frame((*rhs)(roi), rhs->delay()));
};

int Frame::delay() const
{
    return the_delay;
};

void Frame::delay(int d)
{
    the_delay = d;
};
        
cv::Rect Frame::get_bounds() const
{
    int l = cols,
        r = 0,
        t = rows,
        b = 0;
    const uint8_t* row;
    bool o;
    for(int y = 0; y < rows; y++)
    {
        row = ptr(y);
        o = false;
        for(int x = 3; x < 4*cols; x+=4)
        {
            if(row[x] > 0)
            {
                o = true;
                if(x < l) l = x;
                if(x > r) r = x;
            }
        }
        if(o)
        {
            if(y < t) t = y;
            if(y > b) b = y;
        }
    }
    if(l > r) return cv::Rect(0,0,0,0);
    return cv::Rect(l/4,t,(r-l)/4+1,b-t+1);
};
        
bool Frame::has_transparency() const
{
    for(auto px = begin<cv::Vec4b>();
             px < end<cv::Vec4b>();
             px++)
        if((*px)[3] != 255) return true;
    return false;
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

int Sequence::width() const
{
    if(size() < 1) return -1;
    return at(0)->cols;
}

int Sequence::height() const
{
    if(size() < 1) return -1;
    return at(0)->rows;
}
        
pSequence Sequence::resize(int width, int height, int interpolation) const
{
    pSequence r = Sequence::create();
    cv::Size s(width, height);
    pFrame out;
    for(auto it : *this)
    {
        out = Frame::create(width, height, it->delay());
        cv::resize(*it, *out, s, 0, 0, interpolation);
        r->push_back(out);
    }
    return r;
};
        
pSequence Sequence::crop(int x, int y, int width, int height) const
{
    pSequence r = Sequence::create();
    cv::Rect roi(x,y,width,height);
    for(auto it : *this)
    {
        r->push_back(Frame::create(it, roi));
    }
    return r;
};

pSequence Sequence::blur(float sigma) const
{
    pSequence r = Sequence::create();
    cv::Size ksize(2*int(1.5*sigma+0.5)+1, 2*int(1.5*sigma+0.5)+1);
    pFrame out;
    for(auto it : *this)
    {
        out = Frame::create(it->cols, it->rows, it->depth());
        cv::GaussianBlur(*it, *out, ksize, sigma);
        r->push_back(out);
    }
    return r;
};

pSequence Sequence::time_blur(float sigma) const
{
    pSequence r = Sequence::create();
    int kc = int(1.5*sigma+0.5);
    float *K = get_kernel(sigma, kc);
    pFrame out, sum;

    //create an array of higher bit-depth copies
    vector<pFrame> images;
    for(auto it = begin(); it < end(); it++)
    {
        out = Frame::create((*it)->cols, (*it)->rows, (*it)->delay(), CV_16UC4);
        (*it)->convertTo(*out, CV_16U, 256.);
        images.push_back(out);
    }

    //for each position
    sum = Frame::create(images[0]->cols, images[0]->rows, 0, CV_16UC4);
    out = Frame::create(images[0]->cols, images[0]->rows);
    for(int f = 0; f < images.size(); f++)
    {
        sum->setTo(cv::Scalar(0,0,0,255));
        //  sum output
        for(int k = 1-kc; k < kc+2; k++)
        {
            int s = f+k;
            if(s < 0)
            {
                s = -s;
                if(s > images.size()) s = images.size()-1;
            } else if(s >= images.size())
            {
                s = 2*images.size() - (s+1);
                if(s < 0) s = 0;
            }
            cv::addWeighted(*sum, 1.0, *images[s], K[k+kc-1], 0, *sum);
        }
        //  convert output back to standard bit-depth & store

        sum->convertTo(*out, CV_8U, 1./256.);
        r->push_back(out);
    }
    delete [] K;
    //return stored images
    return r;
};

float *Sequence::get_kernel(float sigma, int kernel_center)
{
    float *kernel = new float[kernel_center*2+1];
    float n = 0;
    for(int k = 0; k < kernel_center*2+1; k++)
    {
        kernel[k] = std::exp(-(k-kernel_center)*(k-kernel_center)/(2*sigma*sigma));
        n += kernel[k];
    }
    for(int k = 0; k < kernel_center*2+1; k++)
        kernel[k] = kernel[k] / n;

    return kernel;
};
