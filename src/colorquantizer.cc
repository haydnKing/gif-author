#include "colorquantizer.h"

#include <fstream>
#include <unistd.h>

/* *************************************************************
 *                                         ColorQuantizer
 * *************************************************************/

ColorQuantizer::ColorQuantizer() :
    colors(NULL),
    max_colors(0),
    num_colors(0)
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
    colors = new uint8_t[3*max_colors];
};

void ColorQuantizer::add_color(const uint8_t* color)
{
    if(num_colors < max_colors)
    {
        for(int i = 0; i < 3; i++)
            colors[3*num_colors+i] = color[i];
        num_colors++;
    }
};

void ColorQuantizer::add_colors(const uint8_t* color, int count)
{
    for(int i = 0; i < count; i++)
    {
        add_color(color+3*i);
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

        virtual void build_ct(int quantized_colors=256);
        virtual int map_to_ct(const uint8_t* color) const;
        virtual const GIFColorTable *get_ct() const;

    protected:

        class vbox
        {
            public:
                vbox(uint8_t* px, int num_pixels);
                ~vbox();

                bool is_leaf() const;
                vbox *get_left() {return left;};
                vbox *get_right() {return right;};
                vbox *get_largest(float volume_coef, float count_coef); 
                void add_to_ct(GIFColorTable *ct);

                unsigned int get_volume() const;
                int get_count() const;

                int quantize(const uint8_t *color);

                void split();

            private:
                void swap_px(const int& a, const int& b);
                //int partition(int start, int end, int pivot, int channel);
                int partition_by_value(int start, int end, float value, int channel);
                int partition_channel(int channel);
                void set_front_back(int start, int end, int channel);

                vbox *left, *right;
                int split_channel, ct_index;
                float split_value;
                uint8_t *px;
                int num_pixels;
                uint8_t min[3], max[3];
        };


        GIFColorTable *ct;
        vbox *root;
};

        
MMCQuantizer::MMCQuantizer() :
    root(NULL)
{};

MMCQuantizer::~MMCQuantizer()
{
    delete root;
};

void MMCQuantizer::build_ct(int quantized_colors)
{
    std::cout << "MMCQuantizer::build_ct(quantized_colors = "<< quantized_colors<<")" << std::endl;
    root = new vbox(colors, num_colors);
    ct = new GIFColorTable();
    float f = 0.5;

    //split up the color space
    for(int i = 1; i < quantized_colors; i++)
    {
        std::cout << "\ti = " << i << std::endl;
        if(i < quantized_colors * f)
            root->get_largest(1.0,0.0)->split();
        else
            root->get_largest(1.0,1.0)->split();
    }
    root->add_to_ct(ct);
    std::cout << "MMCQuantizer::~build_ct(...)" << std::endl;
};

int MMCQuantizer::map_to_ct(const uint8_t* color) const
{
    return root->quantize(color);
};

const GIFColorTable *MMCQuantizer::get_ct() const
{
    return ct;
};

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

MMCQuantizer::vbox::~vbox() 
{
    if(left != NULL)
        delete left;
    if(right != NULL)
        delete right;
};

bool MMCQuantizer::vbox::is_leaf() const
{
    return (left == NULL && right ==NULL);
};

MMCQuantizer::vbox *MMCQuantizer::vbox::get_largest(float volume_coef, float count_coef)
{
    if(is_leaf())
        return this;
    vbox *l = left->get_largest(volume_coef, count_coef), 
         *r =right->get_largest(volume_coef, count_coef);

    if((volume_coef * l->get_volume() + count_coef * l->get_count()) > 
       (volume_coef * r->get_volume() + count_coef * r->get_count()))
        return l;
    return r;
};

unsigned int MMCQuantizer::vbox::get_volume() const
{
    return (max[0] - min[0]) * 
           (max[1] - min[1]) * 
           (max[2] - min[2]);
};

int MMCQuantizer::vbox::get_count() const
{
    return num_pixels;
};

int MMCQuantizer::vbox::quantize(const uint8_t *color)
{
    if(is_leaf())
    {
        return ct_index;
    }

    if(color[split_channel] <= split_value)
        return left->quantize(color);
    return right->quantize(color);
};
                
void MMCQuantizer::vbox::add_to_ct(GIFColorTable *ct)
{
    if(is_leaf())
    {
        //find the average pixel value
        uint32_t sum[3] = {0,0,0};
        uint8_t value[3];
        int i,j;
        for(i=0; i < num_pixels; i++)
        {
            for(j=0; j < 3; j++)
            {
                sum[j] += px[3*i+j];
            }
        }
        for(j=0; j<3; j++)
        {
            value[j] = sum[j] / num_pixels;
        }
        ct_index = ct->push_color(value);
    }
    else
    {
        left->add_to_ct(ct);
        right->add_to_ct(ct);
    }
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
/*
int MMCQuantizer::vbox::partition(int start, int end, int pivot, int channel)
{
    //partition from Quicksort
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
*/
int MMCQuantizer::vbox::partition_by_value(int start, int end, float value, int channel)
{
    std::cout << "\t\tMMCQuantizer::vbox::partition_by_value("<<start<<", "<<end<<", "<<value<<", "<<channel<<")"<<std::endl;
    int i = start, j;
    //check if they're all equal
    bool all_equal = true;
    uint8_t v = px[3*start+channel];


    /**/
    unsigned long hist[256];
    for(int k = 0; k < 256; k++)
        hist[k] = 0;
    /**/


    for(j = start; j < end; j++)
    {
        hist[int(px[3*j+channel])]++;
        if(all_equal)
            all_equal = (v == px[3*j+channel]);
        if(px[3*j+channel] < value)
        {
            swap_px(i,j);
            i++;
        }
    }

    /**/
    for(int k = 0; k < 256; k++)
        if(hist[k] != 0)
            std::cout << "\t\t\thist[" << k << "] = " << hist[k] << std::endl;
    /**/

    std::cout << "\t\t\tall_equal = " << all_equal << std::endl;
    
    if(all_equal)
        return -1;
    set_front_back(i, end, channel);
    set_front_back(start, i-1, channel);
    return i;
};

int MMCQuantizer::vbox::partition_channel(int ch)
{
    std::cout << "\tMMCQuantizer::vbox::partition_channel(ch = " << ch << ")" << std::endl;
    int first = 0, 
        last = num_pixels-1, 
        median = num_pixels / 2, 
        pivot;
    float l_bound = float(min[ch]),
          u_bound = float(max[ch]);

    while(last != first)
    {
        std::cout << "bounds = (" << l_bound << ", " << u_bound << ")" << std::endl;
        //partition around a value half way between the extreems
        pivot = partition_by_value(first, last+1, (l_bound+u_bound)/2., ch);
        std::cout << "\t\t-> " << pivot << std::endl;

        //if all the values we're left with are the same
        if(pivot < 0)
        {
            //return either the pivot before or the pivot after the run of
            //identical values
            //choose the one furthest from its original position as the better
            //median
            if((first - 0) > (num_pixels-1 - last))
                return first;
            else
                return last+1;
        }

        //if the pivot is below the median, we only need to partition the upper part
        if(pivot < median)
        {
            l_bound = px[3*pivot+ch];
            first = pivot;
        }
        //if the pivot is above the median, we only need to partition the lower part
        else if(pivot > median)
        {
            u_bound = px[3*(pivot-1)+ch];
            last = pivot-1;
        }
        //special case where we happened to get the median
        else if(pivot == median)
            return pivot;

    }

    return first;
};

void MMCQuantizer::vbox::set_front_back(int start, int end, int channel)
{    
    uint8_t s = px[3*start+channel],
            l  = px[3*start+channel];
    int s_i = start,
        l_i = start;
    for(int j = start; j < end; j++)
    {
        if(px[3*j+channel] < s)
        {
            s = px[3*j+channel];
            s_i = j;
        }
        else if(px[3*j+channel] > l)
        {
            l = px[3*j+channel];
            l_i = j;
        }
    }
    swap_px(start, s_i);
    swap_px(end, l_i);
};

void MMCQuantizer::vbox::split()
{
    std::cout << "MMCQuantizer::vbox::split()" << std::endl;
    std::cout << "\tnum_pixels = " << num_pixels << std::endl;
    if(num_pixels <= 0)
        usleep(10000000);
    int ch = 0, i;
    uint8_t val = 0;
    //get largest dimension
    for(i = 0; i < 3; i++)
    {
        std::cout << "\t(min["<< i << "], max[" << i<<"]) = (" << int(min[i]) << ", " << int(max[i]) << ")"<<std::endl;
        if(max[i] - min[i] > val)
        {
            val = max[i] - min[i];
            ch = i;
        }
    }
    std::cout << "\tchannel = " << ch << std::endl;

    //find the index of the median pixel in that channel
    int split_index = partition_channel(ch);
    std::cout << "\tsplit_index = " << split_index << std::endl;
    int median = split_index;
    split_channel = ch;
    split_value = float(px[3*split_index+ch]);

    //if left is the largest distance
    std::cout << "if(split_value - min[ch] > max[ch] - split_value)" << std::endl; 
    std::cout << "if(" << split_value<<" - "<<int(min[ch])<<" > "<<int(max[ch])<<" - "<<split_value<<")" << std::endl; 
    std::cout << "if("<<(split_value - min[ch])<<" > "<<(max[ch] - split_value)<<")" << std::endl; 
    if(split_value - min[ch] > max[ch] - split_value)
    {
        std::cout << "\tLHS split" << std::endl;
        //split half way along the left
        split_value = ((float)split_value+(float)min[ch])/2.;
        split_index = partition_by_value(0, split_index, split_value, ch);
    }
    //if right is the largest distance
    else
    {
        std::cout << "\tRHS split" << std::endl;
        //split half way along the right
        split_value = ((float)split_value+(float)max[ch])/2.;
        split_index = partition_by_value(split_index, num_pixels, split_value, ch);
    }
    
    std::cout << "\tsplit_index = " << split_index << std::endl;
    left = new vbox(px, split_index);
    right = new vbox(px+3*split_index, num_pixels - split_index);
    std::cout << "\t(left,right)->get_count() = (" << left->get_count() << ", " << right->get_count() << ")" << std::endl; 
    std::cout << "MMCQuantizer::vbox::~split()" << std::endl;
};


pColorQuantizer ColorQuantizer::get_quantizer(QuantizerMethod m)
{
    ColorQuantizer *cq;
    switch(m)
    {
        case QUANT_MMC:
            cq = new MMCQuantizer();
            break;
    }
    return pColorQuantizer(cq);
};
