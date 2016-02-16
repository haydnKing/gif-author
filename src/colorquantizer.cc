#include "colorquantizer.h"

ColorQuantizer::ColorQuantizer() :
    colors(NULL),
    max_colors(0),
    num_color(0)
{};


ColorQuantizer::~ColorQuantizer()
{
    if(colors != NULL)
    {
        delete [] colors;
    }
};

void ColorQuantizer::set_max_colors(int _max_colors)
{
    if(colors != NULL)
    {
        delete [] colors;
    }
    max_colors = _max_colors;
    num_colors = 0;
    colors = new uint8_t[max_colors*3];
};

void ColorQuantizer::add_color(const uint8_t* color)
{
    if(num_colors < max_colors)
    {
        num_colors++;
        for(int i = 0; i < 3; i++)
            colors[num_colors+i] = color[i];
    }
};



/* *************************************************************
 *                                         MMCQuantizer
 * *************************************************************/
class MMCQuantizer : public ColorQuantizer
{
    public:
        MMCQuantizer();
        virtual ~MMCQuantizer();

        void set_max_colors(int max_colors);
        void add_color(const uint8_t* color);
        
        virtual void build_ct();
        virtual int map_to_ct(const uint8_t* color) const;
        virtual GIFColorTable *get_ct();

    protected:

        class vbox
        {
            public:
                vbox(uint8_t* px, int num_pixels);
                ~vbox() {};

                bool is_leaf() const;
                vbox *get_left() {return left;};
                vbox *get_right() {return right;};

                unsigned int get_volume() const;
                int get_count() const;

            private:
                void swap_px(const int& a, const int& b);
                int partition(int start, int end, int pivot, int channel);

                vbox *left, *right;
                uint8_t *px;
                int num_pixels;
                uint8_t min[3], max[3];
        };



        uint32_t *hist;
};

        
void MMCQuantizer::build_ct();
int MMCQuantizer::map_to_ct(const uint8_t* color) const;
GIFColorTable *MMCQuantizer::get_ct();

/* *************************************************************
 *                                         MMCQuantizer::vbox
 * *************************************************************/



MMCQuantizer::vbox::vbox(uint8_t* _px, int np):
    px(_px),
    num_pixels(np),
    left(NULL),
    right(NULL)
{
    //find the minimum and maximum in each dimension
    min[0] = min[1] = min[2] = 255;
    max[0] = max[1] = max[2] = 0;
    int p, i;
    for(p = 0; p < num_pixels; p++)
    {
        for(i = 0; i < 3; i++)
        {
            if(px[3*p+i] > max[i]) max[i] = px[3*p+i];
            if(px[3*p+i] < min[i]) min[i] = px[3*p+i];
        }
    }

};

MMCQuantizer::vbox::~vbox() {};

bool MMCQuantizer::vbox::is_leaf() const
{
    return (left == NULL && right ==NULL);
};

unsigned int MMCQuantizer::vbox::get_volume() const
{
    return (max[0] - min[0]) * 
           (max[1] - min[1]) * 
           (max[2] - min[2]);
};

int MMCQuantizer::vbox::get_count() const
{
    return num_colors;
};


void MMCQuantizer::vbox::swap_px(const int& a, const int& b)
{
    uint8_t t, i;
    for(i=0; i < 3; i++)
    {
        t = px[3*b+i];
        px[3*b+i] = px[3*a+i];
        px[3*a+i] = t;
    }
};

int MMCQuantizer::vbox::partition(int start, int end, int pivot, int channel)
{
    //partition for Quicksort
    uint8_t pivot_val = px[3*pivot+channel];
    int i = start;
    //move pivot to end
    swap_px(pivot, end-1);
    //scan along the range, swapping if value is less than pivot
    for(int j = start; j < end; j++)
    {
        if(px[3*j+channel] < pivot_val)
        {
            swap_px(i,j);
            i++;
        }
    }
    //move the pivot to the middle
    swap_px(i, end-1);

    return i+1;
};
