#ifndef GIF_AUTHOR_GIF_H
#define GIF_AUTHOR_GIF_H

#include "../input/videoframe.h"

/**
 * Store a GIF Color Table
 */
class GIFColorTable
{
    public:
        /**
         * \param _bits number of bits per color channel
         * \param _size number of colors in the color table
         * \param _sorted is the color table ordered according to priority
         */
        GIFColorTable(int _bits, int _size, bool _sorted = false);
        ~GifColorTable();

        /**
         * \returns the number of bits per image channel
         */
        int get_bits() const;

        /**
         * \returns the number of colors in the table
         */
        int get_size() const;

        /**
         * get an RGB tuple of a color from the table
         * \param index the index of the color
         * \returns [Red,Green,Blue]
         */
        uint8_t[3] operator[](int index);
        const uint8_t[3] operator[](int index) const;

        /**
         * set a color in the color table
         * \param index the index of the color to set
         * \param r the red channel
         * \param g the green channel
         * \param b the blue channel
         * \returns true on success
         */
        bool set_color(int index, uint8_t r, uint8_t g, uint8_t b);

    private:
        int bits, size;
        uint8_t data;
};

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
                 ColorTable* ct=NULL);
        ~GIFImage();

        // accessors

        int get_left() const;
        int get_top() const;
        int get_width() const;
        int get_height() const;
        int get_delay_time() const;

        bool is_interlaced() const;
        bool has_local_colortable() const;
        bool has_transparency() const;
        bool is_user_input() const;

        int transparent_index() const;

        // methods

    private:
        int left, top, width, height, delay_time;
        bool flag_interlace, flag_transparency, flag_user_input;
        int t_color_index;
        int disposal_method; // should be an enum?
        ColorTable* ct;
        uint8_t* data;
};

/**
 * Store an entire GIF
 */
class GIF
{
};

#endif
