#include "bitset.h"

Bitset::Bitset(int _width, int _height, bool initial):
    width(_width),
    height(_height)
{
    data = new uint8_t[(width*height+7)/8];
    clear(initial);
};

Bitset::~Bitset()
{
    delete [] data;
};

pBitset Bitset::create(int _width, int _height, bool initial)
{
    return pBitset(new Bitset(_width, _height, initial));
};

bool Bitset::get(int x, int y) const
{
    x = x+y*width;
    return (data[x/8] & (1<<(x%8)));
};

void Bitset::set(int x, int y, bool s)
{
    x = x+y*width;
    if(s)
        data[x/8] = data[x/8] | (1 << (x%8));
    else
        data[x/8] = data[x/8] & ~(1 << (x%8));
};

void Bitset::clear(bool v)
{
    if(v)
        std::memset(data, 255, (width*height+7)/8);
    else
        std::memset(data, 0, (width*height+7)/8);
};

void Bitset::remove_islands()
{
    bool t;
    int x,y;
    for(y=1; y < height-1; y++)
    {
        //left column, x=0
        if(get(0,y) &&
           !get(0,y-1) &&
           !get(0,y+1) &&
           !get(1,y  ) &&
           !get(1,y-1) &&
           !get(1,y-1))
        {
            set(0,y,false);
        }
        //right column, x=width-1
        if(get(width-1,y) &&
           !get(width-1,y-1) &&
           !get(width-1,y+1) &&
           !get(width-2,y  ) &&
           !get(width-2,y-1) &&
           !get(width-2,y-1))
        {
            set(0,y,false);
        }
        for(x=1; x < width-1; x++)
        {
            //general case
            //if pixel is high and surroundings are low
            if(get(x  ,y  ) &&
               !get(x  ,y+1) &&
               !get(x  ,y-1) &&
               !get(x-1,y  ) &&
               !get(x-1,y+1) &&
               !get(x-1,y-1) &&
               !get(x+1,y  ) &&
               !get(x+1,y+1) &&
               !get(x+1,y-1))
            {
                //set low
                set(x,y,false);
            }
        }
    }
    for(x=1; x < width-1; x++)
    {
        //top row, y=0
        if(get(x,0) &&
           !get(x-1,0) &&
           !get(x+1,0) &&
           !get(x-1,1) &&
           !get(x  ,1) &&
           !get(x+1,1))
        {
            set(x,0,false);
        }
        //bottom row, y=height-1
        if(get(x,height-1) &&
           !get(x-1,height-1) &&
           !get(x+1,height-1) &&
           !get(x-1,height-2) &&
           !get(x  ,height-2) &&
           !get(x+1,height-2))
        {
            set(x,height-1,false);
        }
    }
    //x==0, y==0
    if(get(0,0) &&
       !get(1,0) &&
       !get(1,1) &&
       !get(0,1))
    {
        set(0,0,false);
    }
    //x==width-1, y==0
    if(get(width-1,0) &&
       !get(width-1,1) &&
       !get(width-2,0) &&
       !get(width-2,1))
    {
        set(width-1,0,false);
    }
    //x==0, y==height-1
    if(get(0,height-1) &&
       !get(1,height-1) &&
       !get(1,height-2) &&
       !get(0,height-2))
    {
        set(0,height-1,false);
    }
    //x==width-1, y==height-1
    if(get(width-1,height-1) &&
       !get(width-2,height-1) &&
       !get(width-2,height-2) &&
       !get(width-1,height-2))
    {
        set(width-1,height-1,false);
    }
};

pBitset Bitset::smooth(const pBitset in, float sigma, float threshold)
{ 
    int kernel_center = int(1.5*sigma);
    int kernel_length = kernel_center * 2 + 1; 
    float *kernel = new float[kernel_length];
    float norm = 0;
    for(int k = 0; k < kernel_center*2+1; k++)
    {
        kernel[k] = std::exp(-(k-kernel_center)*(k-kernel_center)/(2*sigma*sigma));
        norm += kernel[k];
    }

    pBitset out = Bitset::create(in->get_width(), in->get_height(), false);
    int x,y,dy,ky,dx,kx;
    float h_norm, v_norm, h_sum, v_sum;
    for(y = 0; y < in->get_height(); y++)
    {
        for(x = 0; x < in->get_width(); x++)
        {
            v_norm = 0.;
            v_sum = 0.;
            for(ky=0; ky < kernel_length; ky++)
            {
                dy = y + ky - kernel_center;
                if(dy >= 0 && dy < in->get_height())
                {
                    h_sum = 0.;
                    h_norm = 0.;
                    for(kx=0; kx < kernel_length; kx++)
                    {
                        dx = x + kx - kernel_center;
                        if(dx >= 0 && dx < in->get_width())
                        {
                            h_norm += kernel[kx];
                            if(in->get(dx,dy))
                                h_sum += kernel[kx];
                        }
                    }
                    v_sum += kernel[ky] * (h_sum / h_norm);
                    v_norm += kernel[ky];
                }
            }

        if(v_sum / v_norm >= threshold)
            out->set(x,y);
        }
    }

    delete [] kernel;
    return out;
};

