#ifndef GIF_AUTHOR_GIF_H
#define GIF_AUTHOR_GIF_H

#include "LZW.h"

#include <cmath>
#include <cstring>
#include <list>
#include <stdint.h>
#include <ostream>
#include <fstream>
#include <memory>
#include <limits>

#include <iostream>

/**
 * Store a GIF Color Table
 *
 * TODO: Expand colors to nearest power of two
 */
class GIFColorTable
{
    public:
        /**
         * \param _depth number of bits per color channel
         * \param _sorted is the color table ordered according to priority
         */
        GIFColorTable(int depth = 8, bool sorted = false);
        ~GIFColorTable();

        /**
         * \returns the number of bits per image channel
         */
        int get_depth() const {return depth;};

        /**
         * \returns the number of colors
         */
        int get_size() const {return size;};

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
         * get a colour from the table
         * \param index the index of the color
         * \returns uint8_t[3]
         */
        uint8_t *get_index(int index) {return data+3*index;};
        const uint8_t *get_index(int index) const {return data+3*index;};

        /**
         * add a colour
         */
        void add_color(const uint8_t *c);

        /**
         * number of colours added
         */
        int num_colors() const {return colors;};

        /**
         * indicate that no more colours will be added. Allows creation of fast
         * colour lookup trees
         */
        void finalize();

        /**
         * Get the closest palette colour to v - currently L-2 norm in RGB space
         */
        int get_closest(const uint8_t *v) const;
        
        /**
         * get the transparent index
         */
        int get_transparent_index() const {return transparent_index;};

        /**
         * does the colour table have a transparent index?
         */
        bool is_transparent() const {return transparent_index >= 0;};

        /**
         * set the transparent index
         */
        void set_transparent_index(int idx) {transparent_index = idx;};

        /**
         * /returns the size of the saved color table in bytes
         */
        int get_size_bytes() const {return 3*colors;};

        /**
         * write the color table to the stringstream
         */
        void write(std::ostream& str) const;

        /**
         * write to a ppm for debugging
         */
        void write_ppm(const char *fname) const;

    private:
        int depth, size, colors, transparent_index;
        bool sorted;
        uint8_t *data;
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
                 const GIFColorTable* ct=NULL);
        ~GIFImage();

        // accessors

        int get_left() const {return left;};
        void set_left(int l) {left = l;};
        int get_top() const {return top;};
        void set_top(int t) {top = t;};
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_delay_time() const {return delay_time;};
        void set_delay_time(int _dt) {delay_time=_dt;};

        bool is_interlaced() const {return flag_interlaced;};
        bool has_local_colortable() const {return ct!=NULL;};
        void set_local_colortable(const GIFColorTable* _ct) {ct = _ct;};
        const GIFColorTable* get_local_colortable() const {return ct;};
        bool is_user_input() const {return flag_user_input;};

        DisposalMethod get_disposal_method() const {return disposal_method;};
        void set_disposal_method(DisposalMethod dm) {disposal_method = dm;};

        // methods        
        void write(std::ostream& str, GIFColorTable* global_ct) const;
        void write_ppm(const char *fname, const GIFColorTable *global_ct=NULL) const;

        uint8_t* get_data() {return data;};
        const uint8_t* get_data() const {return data;};

        const uint8_t& get_value(int x, int y) const;
        void set_value(int x, int y, uint8_t value);

        void clear_to(uint8_t code);

    private:
        uint16_t left, top, width, height, delay_time;
        bool flag_interlaced, flag_user_input;
        DisposalMethod disposal_method;
        const GIFColorTable* ct;
        uint8_t* data;
};

typedef std::shared_ptr<GIFImage> pGIFImage;

/**
 * Store an entire GIF
 */
class GIF : public std::list<pGIFImage>
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

typedef std::shared_ptr<GIF> pGIF;

#endif
