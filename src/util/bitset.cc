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

