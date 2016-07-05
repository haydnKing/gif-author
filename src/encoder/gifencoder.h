#ifndef GTKMM_GIFENCODER_H
#define GTKMM_GIFENCODER_H

/**
 * Turn a sequence of pre-processed images into an efficient GIF
 */

#include "../video/videoframe.h"
#include "../output/gif.h"
#include "colorquantizer.h"
#include "segmenter.h"
#include "ditherer.h"

#include <stdint.h>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

/**
 * Central class. Contains all the information required to write out a GIF
 */
class GIFEncoder
{
    public:
        GIFEncoder(int canvas_width,
                   int canvas_height);
        ~GIFEncoder();

        void push_frame(pVideoFrame fr);

        GIF *get_output();


    protected:
        int canvas_width, canvas_height;
        //general settings
        
        std::vector<pVideoFrame> frames;

        void dbg_save_POI(int x, int y, const char* name) const;
        void dbg_thresholding(int len, uint8_t *px, float *fpx, const char *name) const;

        pGIFImage create_gif_image(int left, int top, int width, int height) const;

};



#endif // GTKMM_GIFAUTHOR_H
