#ifndef GIF_AUTHOR_GIF_SETTINGS_H
#define GIF_AUTHOR_GIF_SETTINGS_H

#include "../input/videoframe.h"

/**
 * Hold settings for an arbitrary operation
 */
class GIFPreProcess {
    public:
        virtual bool apply(pVideoFrame in, pVideoFrame& out) = 0;
        virtual bool preview(pVideoFrame in, pVideoFrame& out) = 0;
};
/**
 * Class to hold settings relating to cropping the image
 */
class Crop : public GIFPreProcess 
{
    public:
        Crop();
        Crop(int _left, int _right, int _width, int _height);
        virtual ~Crop() {};

        int get_left() const {return left;}
        int get_right() const {return right;}
        int get_width() const {return width;}
        int get_height() const {return height;}

        void set_left(int l) {left = l;}
        void set_right(int r) {right = r;}
        void set_width(int w) {width = w;}
        void set_height(int h) {height = h;}

        virtual bool apply(pVideoFrame in, pVideoFrame& out);
        virtual bool preview(pVideoFrame in, pVideoFrame& out);

    private:
        int left, right, width, height;
};

/**
 * Class to hold image scaling settings
 */
class Scale : public GIFPreProcess {
    public:
        Scale();
        Scale(int _width, int _height);
        virtual ~Scale() {};

        int get_width() const {return width;};
        int get_height() const {return height;};

        void set_width(int w) {width = w;}
        void set_height(int h) {height = h;}

        virtual bool apply(pVideoFrame in, pVideoFrame& out);
        virtual bool preview(pVideoFrame in, pVideoFrame& out);
    private:
        int width, height;
};

/**
 * Hold a VideoFrame and all the settings necessary to convert it into a
 * GIFImage
 */
class GIFImagePreProcess {
    public:
        GIFImageSettings(pVideoFrame frame);
        virtual ~GIFImageSettings();

        /**
         * @returns whether or not the frame is enabled in the final GIF
         */
        bool is_enabled() const;
        void set_enabled(bool _enabled);

        void add_preprocess(GIFPreProcess* pp);
        void clear_preprocess();

        pVideoFrame get_frame();

    protected:
        pVideoFrame frame;
        bool enabled;

        Crop* crop;
        Scale* scale;
};



#endif //GIF_AUTHOR_GIF_SETTINGS_H
