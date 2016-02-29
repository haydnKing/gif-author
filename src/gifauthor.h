#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

/**
 * Underlying application code, does not require UI
 */

#include "input/videoframe.h"
#include "output/gif.h"
#include "colorquantizer.h"

#include <stdint.h>
#include <vector>

#include <iostream>

/**
 * Which method to use when dithering an image
 */
enum DitherMethod {
    DITHER_NONE,
    DITHER_FLOYD_STEINBERG
};


/**
 * Central class. Contains all the information required to write out a GIF
 */
class GIFAuthor
{
    public:
        GIFAuthor();
        ~GIFAuthor();

        /**
         * get the method used for dithering
         */
        DitherMethod get_dm() const;
        /**
         * set the method used for dithering
         */
        void set_dm(DitherMethod _dm);

        /**
         * get the method used for quantizing
         */
        QuantizerMethod get_qm() const;
        /**
         * set the method used for quantizing
         */
        void set_dm(QuantizerMethod _qm);


        int get_output_width() const {return out_width;};
        int get_output_height() const {return out_height;};
        void set_output_size(int width=-1, int height=-1)
        {out_width=width; out_height=height;};


        /**
         * clear the frames
         */
        void clear_frames();
        /**
         * add a frame
         */
        void add_frame(pVideoFrame f);
        /**
         * get the list of frames
         */
        const std::vector<pVideoFrame> get_frames() const;
        /**
         * get the number of frames
         */
        int count_frames() const;


        /**
         * get the current output
         * only valid after update_output
         */
        const GIF *get_output() const;

        /**
         * (re)calculate the output
         */
        void update_output();

    protected:
        int out_width, out_height;
        //general settings
        DitherMethod dm;
        QuantizerMethod qm;
        
        std::vector<pVideoFrame> frames;
        GIF *out;


        pGIFImage dither_image(const pVideoFrame vf,
                               const pColorQuantizer cq) const;
        void dither_FS(const pVideoFrame vf,
                       GIFImage* out, 
                       const pColorQuantizer cq) const;
        void dither_none(const pVideoFrame vf,
                         GIFImage* out, 
                         const pColorQuantizer cq) const;
};



#endif // GTKMM_GIFAUTHOR_H
