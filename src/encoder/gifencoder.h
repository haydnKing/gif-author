#ifndef GTKMM_GIFENCODER_H
#define GTKMM_GIFENCODER_H

/**
 * Turn a sequence of pre-processed images into an efficient GIF
 */

#include "input/videoframe.h"
#include "output/gif.h"
#include "colorquantizer.h"
#include "segmenter.h"

#include <stdint.h>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>

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

        void dither_image(pGIFImage out,
                          const pVideoFrame vf,
                          const pBitset mask,
                          const pColorQuantizer cq,
                          uint8_t colors) const;
        void dither_FS(const pVideoFrame vf,
                       const pBitset mask,
                       pGIFImage out, 
                       const pColorQuantizer cq) const;
        void dither_none(const pVideoFrame vf,
                         const pBitset mask,
                         pGIFImage out, 
                         const pColorQuantizer cq) const;


        void dbg_save_POI(int x, int y, const char* name) const;
        void dbg_thresholding(int len, uint8_t *px, float *fpx, const char *name) const;

        pGIFImage create_gif_image(int left, int top, int width, int height) const;

};



#endif // GTKMM_GIFAUTHOR_H
