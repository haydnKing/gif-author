#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

/**
 * Underlying application code, does not require UI
 */

#include "input/videoframe.h"
#include "output/gif.h"

#include <stdint.h>
#include <vector>

/**
 * Which method to use when dithering an image
 */
enum DitherMethod {
    DITHER_NONE,
    DITHER_FLOYD_STEINBERG
};


/**
 * Hold information on an intermediary video frame.
 * Once settings are applied, can the produce the final GIFImage
 */
class GAFrame
{
    public:
        /**
         * @param _pVF RefPtr to the VideoFrame
         * @param _delay_t delay until next frame in microseconds -ve values
         * are snapped to zero
         */
        GAFrame(pVideoFrame& _pVF, int _delay_t);
        virtual ~GAFrame();

        /**
         * get the time until the next frame is shown
         */
        int get_delay() const {return delay_t;};
        /**
         * set the time until the next frame is shown
         * @param t milliseconds
         */
        void set_delay(int t) {delay_t = t;};

        /**
         * Get the method used for dithering
         */
        DitherMethod get_dither_method() const {return dither_method;};
        /**
         * Set the method used for dithering
         * @param m
         */
        void set_dither_method(DitherMethod m) {dither_method = m;};

        /**
         * Set the method for creating a local colour table. Ignored if a
         * global color table is set.
         */
        ColorTableMethod get_ct_method() const {return ct_method;};
        /**
         * Set the method for choosing a colour table. Ignored if a global
         * colour table is set
         * @param m
         */
        void set_ct_method(ColorTableMethod m) {ct_method = m;};


        /**
         * Force the frame to use the global colour table rather than making
         * its own. Set ct==NULL to disable the global table.
         * @param gct The global ct
         */
        void use_global_ct(GIFColorTable* ct) {global_ct = ct;};

        /**
         * Get whether or not the global colour table will be used
         */
        bool has_global_ct() {return global_ct != NULL;};

        /**
         * Do all processing and return the final GIFImage for writing
         * @param w final width
         * @param h final height
         * @param im interpolation method to use for scaling
         */
        std::vector<pGIFImage> process(int w, 
                                       int h, 
                                       InterpolationMethod im=INTERPOLATION_BICUBIC) const;


    protected:
        /*
         * Either generate of return the global table
         */
        bool get_color_table(GIFColorTable *& ct, pVideoFrame scaled_vf) const;

        /*
         * Use colour table to produce a dithered image
         */
        Glib::RefPtr<GIFImage> dither_image(GIFColorTable& ct, pVideoFrame scaled_vf) const;

        void dither_FS(GIFImage* out, GIFColorTable& ct, pVideoFrame scaled_vf) const;
        void dither_none(GIFImage* out, GIFColorTable& ct, pVideoFrame scaled_vf) const;

        pVideoFrame pVF;
        int delay_t;
        DitherMethod dither_method;
        GIFColorTable *global_ct;
        bool use_gct;
};

#endif // GTKMM_GIFAUTHOR_H
