#ifndef GIF_AUTHOR_GIF_H
#define GIF_AUTHOR_GIF_H

#include "LZW.h"
#include "../util/out.h"

#include <cmath>
#include <cstring>
#include <string>
#include <list>
#include <stdint.h>
#include <ostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <limits>

#include <iostream>

class GIFColorTable;
typedef std::shared_ptr<GIFColorTable> pGIFColorTable;
typedef std::shared_ptr<const GIFColorTable> pcGIFColorTable;
/**
 * Store a GIF Color Table
 *
 * TODO: Expand colors to nearest power of two
 */
class GIFColorTable
{
    public:
        ~GIFColorTable();
        /**
         * \param _depth number of bits per color channel
         * \param _sorted is the color table ordered according to priority
         */
        static pGIFColorTable create(int depth = 8, bool sorted = false);

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
        void set_transparent();

        /**
         * /returns the size of the saved color table in bytes
         */
        int get_size_bytes() const {return 3*colors;};

        /**
         * write the color table to the stringstream
         */
        int write(std::ostream& str) const;

        /**
         * write to a ppm for debugging
         */
        void write_ppm(const char *fname) const;

    protected:
        GIFColorTable(int depth = 8, bool sorted = false);

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

class GIFImage;
typedef std::shared_ptr<GIFImage> pGIFImage;
/**
 * Store an individual GIF Image
 */
class GIFImage
{
    public:
        ~GIFImage();
        static pGIFImage create(int left, 
                                int top,
                                int width, 
                                int height, 
                                int delay_time=0, 
                                pcGIFColorTable ct=NULL);

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
        void set_local_colortable(pcGIFColorTable _ct) {ct = _ct;};
        pcGIFColorTable get_local_colortable() const {return ct;};
        bool is_user_input() const {return flag_user_input;};

        DisposalMethod get_disposal_method() const {return disposal_method;};
        void set_disposal_method(DisposalMethod dm) {disposal_method = dm;};

        // methods        
        int write(std::ostream& str, pGIFColorTable global_ct);
        void write_ppm(const char *fname, pcGIFColorTable global_ct=NULL) const;

        uint8_t* get_data() {return data;};
        const uint8_t* get_data() const {return data;};

        const uint8_t& get_value(int x, int y) const;
        void set_value(int x, int y, uint8_t value);

        void clear_to(uint8_t code);

        std::string as_string() const;

    protected:
        GIFImage(int left, 
                 int top,
                 int width, 
                 int height, 
                 int delay_time=0, 
                 pcGIFColorTable ct=NULL);

    private:
        uint16_t left, top, width, height, delay_time;
        int bytes;
        bool flag_interlaced, flag_user_input;
        DisposalMethod disposal_method;
        pcGIFColorTable ct;
        uint8_t* data;
};


class GIF;
typedef std::shared_ptr<GIF> pGIF;
/**
 * Store an entire GIF
 */
class GIF : public std::list<pGIFImage>
{
    public:
        virtual ~GIF();
        
        static pGIF create(uint16_t _width, 
            uint16_t _height,
            pGIFColorTable _global_color_table=NULL,
            uint16_t _loop_count=0,
            uint8_t _background_color_index=0,
            uint8_t _pixel_aspect_ratio=0);

        //accessors
        int get_width() const {return width;};
        int get_height() const {return height;};
        int get_par() const {return par;};
        uint16_t get_loop_count() const {return loop_count;};
        uint8_t get_bg_color_index() const {return bg_color_index;};
        pGIFColorTable get_global_colortable() {return global_ct;};
        pcGIFColorTable get_global_colortable() const {return global_ct;};

        int write(std::ostream& out) const;

        int save(const std::string filename) const;

        std::string as_string() const;

    protected:
        GIF(uint16_t _width, 
            uint16_t _height,
            pGIFColorTable _global_color_table=NULL,
            uint16_t _loop_count=0,
            uint8_t _background_color_index=0,
            uint8_t _pixel_aspect_ratio=0);

    private:
        uint16_t width, height;
        uint8_t bg_color_index, par;
        uint16_t loop_count;
        pGIFColorTable global_ct;

        int write_animation_hdr(std::ostream& out) const;

};

#endif
