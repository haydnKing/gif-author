#ifndef GIF_AUTHOR_GIF_H
#define GIF_AUTHOR_GIF_H

#include "LZW.h"

#include <cmath>
#include <cstring>
#include <list>
#include <stdint.h>
#include <ostream>

#include <iostream>

class RGBColor {
    public:
        RGBColor();
        RGBColor(uint8_t _r,
                 uint8_t _g,
                 uint8_t _b);
        RGBColor(const RGBColor& rhs);
        virtual ~RGBColor() {};

        const uint8_t& r() const {return c[0];};
        const uint8_t& g() const {return c[1];};
        const uint8_t& b() const {return c[2];};

        void r(uint8_t r) {c[0]=r;};
        void g(uint8_t g) {c[1]=g;};
        void b(uint8_t b) {c[2]=b;};
        
        void rgb(uint8_t _r, uint8_t _g, uint8_t _b);
        RGBColor& operator=(const RGBColor& rhs);
        
        const uint8_t* get_data() const;

    protected:
        uint8_t c[3];

};

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
        GIFColorTable(int _depth = 8, bool _sorted = false);
        ~GIFColorTable();

        /**
         * \returns the number of bits per image channel
         */
        int get_depth() const {return depth;};

        /**
         * \returns the number of colors
         */
        int num_colors() const {return colors;};

        /**
         * \returns log of the number of colours which will actually be 
         * written to file, which might be larger than num_colors
         */
        uint8_t log_colors() const;

        /**
         * is the colortable sorted
         */
        bool is_sorted() const {return sorted;};

        /**
         * get an RGBColor from the table
         * \param index the index of the color
         * \returns RGBColor
         */
        RGBColor& operator[](int index) {return data[index];};
        const RGBColor& operator[](int index) const {return data[index];};

        RGBColor& operator=(const RGBColor& rhs);
        /**
         * add a color to the colorscheme
         * \returns index on success, -1 on failure
         */
        int push_color(RGBColor col);

        /**
         * /returns the size of the saved color table in bytes
         */
        int get_size_bytes() const {return 3*colors;};

        /**
         * write the color table to the stringstream
         */
        void write(std::ostream& str) const;

        

    private:
        int depth, colors;
        bool sorted;
        RGBColor* data;
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
                 int delay_time=0, 
                 bool transparency=false,
                 GIFColorTable* ct=NULL);
        ~GIFImage();

        // accessors

        int get_left() const {return left;};
        int get_top() const {return top;};
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_delay_time() const {return delay_time;};
        void set_delay_time(int _dt) {delay_time=_dt;};

        bool is_interlaced() const {return flag_interlaced;};
        bool has_local_colortable() const {return ct!=NULL;};
        bool has_transparency() const {return flag_transparency;};
        bool is_user_input() const {return flag_user_input;};

        int transparent_index() const {return t_color_index;};

        // methods        
        void write(std::ostream& str, GIFColorTable* global_ct) const;

        uint8_t* get_data() {return data;};
        const uint8_t* get_data() const {return data;};

        const uint8_t& get_value(int x, int y) const;
        void set_value(int x, int y, uint8_t value);

        void clear_to(uint8_t code);

    private:
        uint16_t left, top, width, height, delay_time;
        bool flag_interlaced, flag_transparency, flag_user_input;
        uint8_t t_color_index;
        DisposalMethod disposal_method;
        GIFColorTable* ct;
        uint8_t* data;
};

/**
 * Store an entire GIF
 */
class GIF : public std::list<GIFImage>
{
    public:
        GIF(uint16_t _width, 
            uint16_t _height,
            GIFColorTable* _global_color_table=NULL,
            uint16_t _loop_count=0,
            uint8_t _background_color_index=0,
            uint8_t _pixel_aspect_ratio=0);
        virtual ~GIF();

        //accessors
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_par() const {return par;};
        uint16_t get_loop_count() const {return loop_count;};
        uint8_t get_bg_color_index() const {return bg_color_index;};
        GIFColorTable* get_global_colortable() {return global_ct;};
        const GIFColorTable* get_global_colortable() const {return global_ct;};


        void write(std::ostream& out) const;

    private:
        uint16_t width, height;
        uint8_t bg_color_index, par;
        uint16_t loop_count;
        GIFColorTable* global_ct;

        void write_animation_hdr(std::ostream& out) const;

};

#endif
