#include "colorquantizer.h"

class MMCQuantizer : public ColorQuantizer
{
    public:
        MMCQuantizer();
        virtual ~MMCQuantizer();

        void add_color(const uint8_t* color);
        void build_ct();
        int map_to_ct(const uint8_t* color) const;
        GIFColorTable get_ct();

    protected:
        static unsigned int color_to_index(const uint8_t* color) const;
        static unsigned int color_to_index(const uint8_t& r, 
                                           const uint8_t& g, 
                                           const uint8_t& b) const;

        class vbox
        {
            public:
                vbox(uint32_t *_hist,
                     int start_r,
                     int start_g,
                     int start_b,
                     int size_r, 
                     int size_g, 
                     int size_b);
                ~vbox() {};

                int get_volume() const;
                int get_count() const;
                bool is_splittable() const;

                void split(vbox &*lhs, vbox &*rhs);

                void apply_color(int color);

            private:
                uint8_t size[3];
                uint8_t minimum[3], maximum[3];
                uint32_t *hist;
                uint32_t count;
        };



        uint32_t *hist;
};

/* *************************************************************
 *                                         MMCQuantizer
 * *************************************************************/

MMCQuantizer::MMCQuantizer()
{
    //2^17, number of quantum spaces
    hist = new uint32_t[131072];
    std::memset(hist, 0, 131072*sizeof(uint32_t));
};

virtual MMCQuantizer::~MMCQuantizer()
{
    delete [] hist;
};

void MMCQuantizer::add_color(uint8_t* color)
{
    hist[color_to_index(color)]++;
}

void MMCQuantizer::build_ct()
{
};

int MMCQuantizer::map_to_ct(uint8_t* color) const
{
    return hist[color_to_index(color)];
};

GIFColorTable MMCQuantizer::get_ct()
{
};
        
unsigned int MMCQuantizer::color_to_index(const uint8_t* color) const
{
    //6 bits red, 6 bits green, 5 bits blue
    return ((color[0] & 252) << 9) + 
           ((color[1] & 252) << 3) + 
           (color[2] >> 3);
};

unsigned int MMCQuantizer::color_to_index(const uint8_t& r, 
                                          const uint8_t& g, 
                                          const uint8_t& b) const
{
    //6 bits red, 6 bits green, 5 bits blue
    return ((r & 252) << 9) + 
           ((g & 252) << 3) + 
           (b >> 3);
};

pColorQuantizer ColorQuantizer::get_quantizer(QuantizerMethod m)
{
    ColorQuantizer *r;
    switch(m){
        case QUANT_MMC:
            r = new MMCQuantizer();
    };
    return pColorQuantizer(r);
};

/* *************************************************************
 *                                         MMCQuantizer::vbox
 * *************************************************************/


MMCQuantizer::vbox::vbox(uint32_t *_hist,
                         int start_r,
                         int start_g,
                         int start_b,
                         int size_r, 
                         int size_g, 
                         int size_b):
    hist(_hist)
{   
    start[0] = start_r;
    start[1] = start_g;
    start[2] = start_b;
    size[0] = size_r;
    size[1] = size_g;
    size[2] = size_b;

    //find range
    minimum[0] = size_r;
    minimum[1] = size_g;
    minimum[2] = size_b;
    maximum[0] = start_r;
    maximum[1] = start_g;
    maximum[2] = start_b;

    int r,g,b;
    uint32_t v;
    count = 0;

    for(r = start[0]; r < (start[0]+size[0]); r++)
    {
        for(g = start[1]; g < (start[1]+size[1]); g++)
        {
            for(b = start[2]; b < (start[2]+size[2]); b++)
            {
                v = hist[color_to_index(r,g,b)];
                if(v > 0)
                {
                    count += v;
                    if(r < minimum[0]) minimum[0] = r;
                    if(r > maximum[0]) maximum[0] = r;
                    if(g < minimum[1]) minimum[1] = g;
                    if(g > maximum[1]) maximum[1] = g;
                    if(b < minimum[2]) minimum[2] = b;
                    if(b > maximum[2]) maximum[2] = b;
                }
            }
        }
    }
};

int MMCQuantizer::vbox::get_volume() const
{
    return (maximum[0] - minimum[0]) * 
           (maximum[1] - minimum[1]) * 
           (maximum[2] - minimum[2]);
};

int MMCQuantizer::vbox::get_count() const
{
    return count;
};

bool MMCQuantizer::vbox::is_splittable() const
{
    return ((maximum[0] - minimum[0]) > 1) &&
           ((maximum[1] - minimum[1]) > 1) &&
           ((maximum[2] - minimum[2]) > 1);
};

void MMCQuantizer::vbox::split(vbox &*lhs, vbox &*rhs);

void MMCQuantizer::vbox::apply_color(int color)
{
    int r,g,b;

    for(r = start[0]; r < (start[0]+size[0]); r++)
    {
        for(g = start[1]; g < (start[1]+size[1]); g++)
        {
            for(b = start[2]; b < (start[2]+size[2]); b++)
            {
                hist[color_to_index(r,g,b)] = color;
            }
        }
    }
};
