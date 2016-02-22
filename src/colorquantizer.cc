#include "colorquantizer.h"

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
        num_colors++;
        for(int i = 0; i < 3; i++)
            colors[3*num_colors+i] = color[i];
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
                int partition(int start, int end, int pivot, int channel);
                int find_median(int channel);

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
    root(NULL)
{};

MMCQuantizer::~MMCQuantizer()
{
    delete root;
};

void MMCQuantizer::build_ct(int quantized_colors)
{
    root = new vbox(colors, num_colors);
    ct = new GIFColorTable();

    //split up the color space
    for(int i = 1; i < quantized_colors; i++)
    {
        root->get_largest(1.0,1.0)->split();
    }
    root->add_to_ct(ct);
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


int MMCQuantizer::vbox::find_median(int ch)
{
    int start = 0, 
        end = num_pixels, 
        median = num_pixels / 2, 
        mid, 
        pivot;

    while(end - start > 1)
    {
        mid = (start + end) / 2;
        // Choose a pivot - the median of the start, middle and end.
        // unlikely to be a rubbish pivot
        if(
            (px[3*start+ch] >= px[3*mid+ch] && px[3*start+ch] <= px[3*end+ch]) ||
            (px[3*start+ch] <= px[3*mid+ch] && px[3*start+ch] >= px[3*end+ch]))
            pivot = start;
        else if(
            (px[3*mid+ch] > px[3*start+ch] && px[3*mid+ch] < px[3*end+ch]) ||
            (px[3*mid+ch] < px[3*start+ch] && px[3*mid+ch] > px[3*end+ch]))
            pivot = mid;
        else
            pivot = end;


        //partition using that pivot
        pivot = partition(start, end, pivot, ch);

        //if the pivot is below the median, we only need to partition the upper part
        if(pivot < median)
            start = pivot;
        //if the pivot is above the median, we only need to partition the lower part
        else if(pivot > median)
            end = pivot-1;
        //special case where we happened to get the median
        else if(pivot == median)
            return pivot;

    }

    return start;
};


void MMCQuantizer::vbox::split()
{
    int ch = 0;
    uint8_t val = 0;
    //get largest dimension
    for(int i = 0; i < 3; i++)
    {
        std::cout << "channel " << i << ": range = [" << (unsigned int)min[i] << ", "<< (unsigned int)max[i] << "] ("<< (unsigned int)(max[i]-min[i])<<")" << std::endl;
        if(max[i] - min[i] > val)
        {
            val = max[i] - min[i];
            ch = i;
        }
    }

    //find the index of the median pixel in that channel
    int median = find_median(ch);
    split_channel = ch;
    split_value = px[3*median+ch];
    
    left = new vbox(px, median);
    right = new vbox(px+median, num_pixels - median);

    std::cout << "split vbox(vol = " << get_volume() << ", count = " << get_count() << ") ch = " << ch << " sv = " << (unsigned int)split_value   
        << "\n\tvbox(vol = " << left->get_volume() << ", count = " << left->get_count() << ")"
        << "\n\tvbox(vol = " <<right->get_volume() << ", count = " <<right->get_count() << ")" << std::endl;
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
