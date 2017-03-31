#ifndef GTKMM_GIFENCODER_H
#define GTKMM_GIFENCODER_H

/**
 * Turn a sequence of pre-processed images into an efficient GIF
 */

#include "../video/imagesequence.h"
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
                   int canvas_height,
                   pSegmenter segmenter,
                   pDitherer ditherer,
                   pColorQuantizer colorquantizer,
                   int max_colors=256);
        ~GIFEncoder();

        pGIF encode(pSequence frames);

        void set_sigma(float s) {sm_sigma = s;};
        void set_threshold(float t) {sm_thresh = t;};

        void set_segmenter(pSegmenter s) {segmenter = s;};
        void set_ditherer(pDitherer d) {ditherer = d;};
        void set_colorquantizer(pColorQuantizer cq) {colorquantizer = cq;};

    protected:
        int canvas_width, canvas_height, max_colors;
        float sm_sigma, sm_thresh;
        pSegmenter segmenter;
        pColorQuantizer colorquantizer;
        pDitherer ditherer;
};



#endif // GTKMM_GIFAUTHOR_H
