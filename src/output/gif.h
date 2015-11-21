#ifndef GIF_AUTHOR_GIF_H
#define GIF_AUTHOR_GIF_H

#include "../input/videoframe.h"

#include <cmath>
#include <ostream>

enum ColorIndex {RED = 0, GREEN=1, BLUE=2};

/**
 * Store a GIF Color Table
 *
 * TOTO: Expand colors to nearest power of two
 */
class GIFColorTable
{
    public:
        /**
         * \param _depth number of bits per color channel
         * \param _colors log_2 of the number of colors
         * \param _sorted is the color table ordered according to priority
         */
        GIFColorTable(int _depth = 8,
                      int _colors = 256,
                      bool _sorted = false, 
                      uint8_t* _data = NULL);
        ~GifColorTable();

        /**
         * \returns the number of bits per image channel
         */
        int get_depth() const {return depth;};

        /**
         * \returns the number of colors
         */
        int num_colors() const {return colors;};

        /**
         * \returns log-2(number of colors)
         */
        uint8_t log_colors() const {return uint8_t(std::log(colors)/std::log(2));};

        /**
         * is the colortable sorted
         */
        bool is_sorted() const {return sorted;};

        /**
         * get an RGB tuple of a color from the table
         * \param index the index of the color
         * \returns [Red,Green,Blue]
         */
        uint8_t* operator[](int index) {return data+3*index;};
        const uint8_t* operator[](int index) const {return data+3*index;};

        /**
         * set a color in the color table
         * \param index the index of the color to set
         * \param r the red channel
         * \param g the green channel
         * \param b the blue channel
         */
        void set_color(int index, uint8_t r, uint8_t g, uint8_t b);

        /**
         * /returns the size of the saved color table in bytes
         */
        int get_size_bytes() const {return 3*colors;};

        /**
         * write the color table to the stringstream
         * /returns the number of bytes written
         */
        int write(std::ostream& str) const;
        

    private:
        int depth, colors;
        bool sorted;
        uint8_t data;
};


enum DisposalMethod {
    DISPOSAL_METHOD_NOT_SPECIFIED = 0,
    DISPOSAL_METHOD_NONE = 1,
    DISPOSAL_METHOD_RESTORE_BACKGROUND = 2,
    DISPOSAL_METHOD_RESTORE_PREVIOUS = 3};

/**
 * Store an individual GIF Image
 */
class GIFImage
{
    public:
        GIFImage(int left, 
                 int top,
                 int width, 
                 int height, 
                 uint8_t* data,
                 int delay_time=0, 
                 ColorTable* ct=NULL);
        ~GIFImage();

        // accessors

        int get_left() const {return left;};
        int get_top() const {return top;};
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_delay_time() const {return delay_time;};

        bool is_interlaced() const {return flag_interlaced;};
        bool has_local_colortable() const {return ct!=NULL;};
        bool has_transparency() const {return flag_transparency;};
        bool is_user_input() const {return flag_user_input;};

        int transparent_index() const {return t_color_index;};

        // methods        
        void write(std::ostream& str) const;

    private:
        uint16_t left, top, width, height, delay_time;
        bool flag_interlace, flag_transparency, flag_user_input;
        uint8_t t_color_index;
        DisposalMethod disposal_method;
        ColorTable* ct;
        uint8_t* data;
};

/**
 * Store an entire GIF
 */
class GIF : public std::list<GIFImage>
{
    public:
        GIF(int _width, 
            int _height,
            ColorTable* _global_color_table=NULL,
            uint8_t _background_color_index=0,
            uint8_t _pixel_aspect_ratio=0);
        virtual ~GIF();

        //accessors
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_par() const {return par;};
        uint8_t get_bg_color_index() const {return bg_color_index;};
        ColorTable* get_global_colortable() {return global_ct;};
        const ColorTable* get_global_colortable() const {return global_ct;};


        void write(std::ostream& out) const;

    private:
        int width, height;
        uint8_t bg_color_index, par;
        ColorTable* global_ct;

};

#endif
