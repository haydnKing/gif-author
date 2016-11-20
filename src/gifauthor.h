#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

#include <gtkmm.h>
#include <iostream>
#include <memory>

#include "encoder/gifencoder.h"

class GIFAuthor;
typedef std::shared_ptr<GIFAuthor> pGIFAuthor;

/**
 * Central class
 */
class GIFAuthor
{
    protected:
        GIFAuthor();
    public:
        virtual ~GIFAuthor();
    
        static pGIFAuthor create();

        virtual void from_files(const std::vector<std::string>& files);

        int get_output_width() const {return out_width;};
        int get_output_height() const {return out_height;};

        void set_size(int width, int height) {out_width=width; out_height=height;};
        void set_delay(int d) {delay = d;};
        void set_outfile(const std::string& outfile) {out_file = outfile;};

        void set_segmenter(pSegmenter s) {segmenter = s;};
        void set_ditherer(pDitherer d) {ditherer = d;};
        void set_colorquantizer(pColorQuantizer cq) {colorquantizer = cq;};


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
        void from_images(std::vector<std::string> fnames);
        
        int out_width, out_height, delay;
        
        std::vector<pVideoFrame> frames;
        GIF *out;
        std::string out_file;

        pSegmenter segmenter;
        pColorQuantizer colorquantizer;
        pDitherer ditherer;
};



#endif // GTKMM_GIFAUTHOR_H
