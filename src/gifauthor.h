#ifndef GIFAUTHOR_H
#define GIFAUTHOR_H

#include <iostream>
#include <memory>

#include "encoder/gifencoder.h"
#include "util/cline.h"
#include "encoder/colorquantizer.h"
#include "encoder/ditherer.h"
#include "encoder/segmenter.h"

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

    protected:
        void from_images(std::vector<std::string> fnames);

        void load_files();
        
        int delay;
        Crop crop_opts;
        Size scale_opts;
        std::string out_file_name;
        
        std::vector<pVideoFrame> frames;
        std::vector<std::string> in_file_names;

        pSegmenter segmenter;
        pColorQuantizer colorquantizer;
        pDitherer ditherer;

        bool is_error;
};



#endif // GTKMM_GIFAUTHOR_H
