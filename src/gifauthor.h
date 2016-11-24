#ifndef GIFAUTHOR_H
#define GIFAUTHOR_H

#include <iostream>
#include <memory>

#include "encoder/gifencoder.h"
#include "util/cline.h"

class GIFAuthor;
typedef std::shared_ptr<GIFAuthor> pGIFAuthor;

/**
 * Central class
 */
class GIFAuthor
{
    protected:
        GIFAuthor(int argc, char* argv[]);
    public:
        virtual ~GIFAuthor();
    
        static pGIFAuthor create(int argc, char* argv[]);

        bool error() const {return is_error;};
        operator bool() const {return error();};

        /**
         * generate the GIF
         */
        virtual pGIF generate();

        int get_output_width() const {return out_width;};
        int get_output_height() const {return out_height;};

    protected:
        void from_images(std::vector<std::string> fnames);

        void load_files();
        
        int delay;
        Crop crop_opts;
        Scale scale_opts;
        std::string out_file;
        
        std::vector<pVideoFrame> frames;
        std::vector<std::string> filenames;

        pSegmenter segmenter;
        pColorQuantizer colorquantizer;
        pDitherer ditherer;

        bool is_error;
};



#endif // GTKMM_GIFAUTHOR_H
