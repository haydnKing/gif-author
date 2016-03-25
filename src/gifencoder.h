#ifndef GTKMM_GIFENCODER_H
#define GTKMM_GIFENCODER_H

/**
 * Turn a sequence of pre-processed images into an efficient GIF
 */

#include "input/videoframe.h"
#include "output/gif.h"
#include "colorquantizer.h"

#include <stdint.h>
#include <vector>

#include <iostream>
#include <fstream>

#include <opencv2/video.hpp>

/**
 * Which method to use when dithering an image
 */
enum DitherMethod {
    DITHER_NONE,
    DITHER_FLOYD_STEINBERG
};


/**
 * A bit for every pixel in an image
 */
class ImageBitset
{
    public:
        ImageBitset(int width, int height, bool initial=false);
        ~ImageBitset();

        bool get(int x, int y) const;
        void set(int x, int y, bool v);
        void set(bool v);

    private:
        uint8_t *bitset;
        int w, h;
};


/**
 * Central class. Contains all the information required to write out a GIF
 */
class GIFEncoder
{
    public:
        GIFEncoder(int canvas_width,
                   int canvas_height,
                   QuantizerMethod qm = QUANT_MMC,
                   DitherMethod dm = DITHER_FLOYD_STEINBERG);
        ~GIFEncoder();

        void push_frame(pVideoFrame fr);

        GIF *get_output();


    protected:
        int canvas_width, canvas_height;
        //general settings
        DitherMethod dm;
        QuantizerMethod qm;
        
        std::vector<pVideoFrame> frames;

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
