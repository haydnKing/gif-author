#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

/**
 * Underlying application code, does not require UI
 */

#include "encoder/gifencoder.h"


/**
 * Central class. Contains all the information required to write out a GIF
 */
class GIFAuthor
{
    public:
        GIFAuthor();
        ~GIFAuthor();

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
        
        std::vector<pVideoFrame> frames;
        GIF *out;

};



#endif // GTKMM_GIFAUTHOR_H
