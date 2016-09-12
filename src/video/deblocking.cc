#include "deblocking.h"

Deblocker::Deblocker(int width, int height) :
    width(width),
    height(height),
    Process("Deblocker", "Reduce the appearance of visual blocks from video compression")
{
    add_setting(new PositiveIntSetting("size", "the size of the blocks in the input", 8));
    buff = VideoFrame::create(width, height);
}

Deblocker::~Deblocker()
{};

void Deblocker::process(std::vector<pVideoFrame> frames)
{
    for(int i = 0; i < frames.size(); i++) 
    {
        ret.push_back(deblock(frames[i]));
    }
}


void Deblocker::deblock(pVideoFrame vf)
{
    //blur weights - linear for now
    float weights[block_size];
    for(int i = 0; i < block_size/2; i++) 
    {
        weights[i] = i / (block_size/2-1);
    }
    for(int i = block_size/2; i < block_size; i++)
    {
        weights[i] = weights[block_size-1-i];
    }

    //build the kernel
    int kernel_center = (block_size+1)/2;
    float sigma = float(kernel_center) / 1.5;
    int kernel_length = kernel_center * 2 + 1; 
    float *kernel = new float[kernel_length];
    for(int k = 0; k < kernel_length; k++)
        kernel[k] = std::exp(-(k-kernel_center)*(k-kernel_center)/(2*sigma*sigma));
    float norm, val[3];

    int block_start, block_end, block_pos;



    delete [] kernel;


}


float Deblocker::vscore(const pVideoFrame vf, int x, int y_min, int y_max)
{
    float s = 0;
    uint8_t *l, *r;
    for(int y = y_min; y < y_max; y++)
    {
        l = vf->get_pixel(x,y);
        r = vf->get_pixel(x+1,y);
        for(int i= 0; i < 3; i++)
            s += float(l[i]) - float(r[i]);
        s /= 3.;
    }
    return s / (y_max - y_min);
};

float Deblocker::hscore(const pVideoFrame vf, int y, int x_min, int x_max)
{
    float s = 0;
    uint8_t *l, *r;
    for(int x = x_min; x < x_max; x++)
    {
        l = vf->get_pixel(x,y);
        r = vf->get_pixel(x,y+1);
        for(int i= 0; i < 3; i++)
            s += float(l[i]) - float(r[i]);
        s /= 3.;
    }
    return s / (x_max - x_min);
};

#endif
