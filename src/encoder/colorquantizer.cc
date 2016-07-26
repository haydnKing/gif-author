#include "colorquantizer.h"

/* *************************************************************
 *                                         ColorQuantizer
 * *************************************************************/

ColorQuantizer::ColorQuantizer(std::string name, std::string description) :
    Process(name, description),
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
                void add_to_ct(int index, GIFColorTable *ct);

                unsigned int get_volume() const;
                int get_count() const;

                void split();

            private:
                void swap_px(const int& a, const int& b);
                int partition(int start, int end, float value, int channel,
                              uint8_t& left_high, uint8_t& right_low);
                uint8_t get_split_value(int channel);

                vbox *left, *right;
                int split_channel, ct_index;
                uint8_t split_value;
                uint8_t *px;
                int num_pixels;
                uint8_t min[3], max[3];
        };


        GIFColorTable *ct;
        vbox *root;
};

        
MMCQuantizer::MMCQuantizer() :
    ColorQuantizer("MMC", "A Modified Median Cut quantizer"),
    root(NULL)
{};

MMCQuantizer::~MMCQuantizer()
{
    delete root;
};

void MMCQuantizer::build_ct(int quantized_colors)
{
    if(num_colors < 2)
        return;

    //first two colours are just lightest and darkest
    ct = new GIFColorTable();
    uint8_t max_r[3] = {0,0,0},
            min_c[3] = {255,255,255};
    for(int i = 0; i < num_colors; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(colors[i+j] > max_c[j]) max_c[j] = colors[i+j];
            if(colors[i+j] < min_c[j]) min_c[j] = colors[i+j];
        }
    }
    ct->add_color(max_c);
    ct->add_color(min_c);

    //rest of the colours are allocated using MMC
    root = new vbox(colors, num_colors);
    float f = 0.5;
    vbox* box;

    //split up the color space
    for(int i = 1; i < quantized_colors; i++)
    {
        if(i < quantized_colors * f)
            box = root->get_largest(1.0,0.0);
        else
            box = root->get_largest(1.0,1.0);
        if(box == NULL)
            break;
        box->split();
    }
    root->add_to_ct(ct);
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
    {
        //check that I contain at least 2 colours
        if((max[0] == min[0]) && (max[1] == min[1]) && (max[2] == min[2]))
            return NULL;
        return this;
    }
    vbox *l = left->get_largest(volume_coef, count_coef), 
         *r =right->get_largest(volume_coef, count_coef);

    if(l!=NULL && r!=NULL)
    {
        if((volume_coef * l->get_volume() + count_coef * l->get_count()) > 
           (volume_coef * r->get_volume() + count_coef * r->get_count()))
            return l;
        return r;
    }
    if(l!=NULL) return l;
    if(r!=NULL) return r;
    return NULL;
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

int MMCQuantizer::vbox::partition(int start, int end, float value, int channel, 
                                  uint8_t& left_high, uint8_t& right_low)
{
    int i = start, j;
    left_high = min[channel];
    right_low = max[channel];
    uint8_t v, lowest = max[channel], highest = min[channel];

    //quicksort esq, keep track of lowest and highest in each side
    for(j = start; j < end; j++)
    {
        v = px[3*j+channel];
        if(v < value)
        {
            if(v > left_high) left_high = v;
            swap_px(i,j);
            i++;
        }
        else
        {
            if(v < right_low) right_low = v;
        }

        if(v < lowest) lowest = v;
        if(v > highest) highest = v;
    }

    //if all values are the same
    if(lowest == highest)
        return -1;

    return i;
};

uint8_t MMCQuantizer::vbox::get_split_value(int ch)
{
    int start = 0,
        end   = num_pixels,
        idx;
    uint8_t low   = min[ch],
            high  = max[ch],
            lhigh = max[ch],
            rlow  = min[ch],
            last_partition;

    while(end-start > 1)
    {
        //average then ceil
        last_partition = int(0.5*(float(low)+float(high))+0.5);
        //partition on the mid value
        idx = partition(start, end, float(last_partition), ch,
                        lhigh, rlow);

        //if all values between start & end are equal
        if(idx < 0)
        {
            //Return the value which will get us closest to the median
            if((start) >= (num_pixels - end))
            {
                //start is larger, put repeated values in rhs
                return last_partition;
            }
            else
            {
                //put repeated values in lhs
                return last_partition + 1;
            }

        }

        //if index is below middle
        if(idx < num_pixels/2)
        {
            //partition the rhs
            start = idx;
            low = rlow;
        }
        //if index above middle
        else if(idx > num_pixels/2)
        {
            //partition lhs
            end = idx;
            high = lhigh;
        }
        //idx == middle
        else
        {
            start = idx;
            end = idx+1;
        }
    }

    return last_partition;
};

void MMCQuantizer::vbox::split()
{
    int ch = 0, i;
    uint8_t val = 0;
    //get largest dimension
    for(i = 0; i < 3; i++)
    {
        if(max[i] - min[i] > val)
        {
            val = max[i] - min[i];
            ch = i;
        }
    }
    split_channel = ch;

    //get split value
    split_value = get_split_value(ch);
    uint8_t lhigh, rlow;
    int split_index;

    //if left is the largest distance
    if(split_value - min[ch] > max[ch] - split_value)
    {
        //split half way along the left
        split_value = uint8_t(0.5+((float)split_value+(float)min[ch])/2.);
        split_index = partition(0, num_pixels, split_value, ch, lhigh, rlow);
    }
    //if right is the largest distance
    else
    {
        //split half way along the right
        split_value = uint8_t(0.5+((float)split_value+(float)max[ch])/2.);
        split_index = partition(0, num_pixels, split_value, ch, lhigh, rlow);
    }
    
    left = new vbox(px, split_index);
    right = new vbox(px+3*split_index, num_pixels - split_index);
};

/*
 * Factory
 */

QuantizerFactory::QuantizerFactory() :
    ProcessFactory("quantizer", "The quantizer takes the full colour spectrum of the image and chooses a palette of colours to represent it in the GIF")
{
    register_type("MMC", new MMCQuantizer());
};

QuantizerFactory quantizerFactory;

