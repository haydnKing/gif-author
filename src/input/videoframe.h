#ifndef GTKMM_GIFAUTHOR_VIDEOFRAME_H
#define GTKMM_GIFAUTHOR_VIDEOFRAME_H

#include <stdint.h>
#include <glibmm/main.h>
#include <glibmm/object.h>

/**
 * Affine 2D Transform
 */
class Affine2D
{
    public:
        Affine2D(const Affine2D& rhs);
        virtual ~Affine2D();

        /**
         * Create an Identity
         */
        static Affine2D I();
        /**
         * Create a scale
         * @param x_ratio final_x / original_x
         * @param y_ratio final_y / original_y
         */
        static Affine2D Scale(double x_ratio, double y_ratio);
        /**
         * Create a translation
         * @param x x position of the new origin
         * @param y y position of the new origin
         */
        static Affine2D Transform(double x, double y);
        /**
         * Create a rotation
         * @param a angle of rotation (radians)
         * @param x center of rotation 
         * @param y center of rotation
         */
        static Affine2D Rotation(double a, double x, double y);
        /**
         * Create a rotation
         * @param a angle of rotation (degrees)
         * @param x center of rotation
         * @param y center of rotation
         */
        static Affine2D RotationDegrees(double a, double x, double y);
        /**
         * Create the product of two Affine2Ds
         * @param left
         * @param right
         * @returns left * right
         */
        static Affine2D Product(const Affine2D& left, const Affine2D& right);

        /**
         * Multiply two affine transformations
         * @param rhs the other transform
         * @returns a new Affine2D
         */
        Affine2D operator*(const Affine2D& rhs) const;

        /**
         * Invert the transformation
         * @returns M^-1
         */
        Affine2D invert() const;

        /**
         * get the result of applying the transformation in the X direction
         * @returns the new x
         */
        double get_x(const double& x, const double& y) const;

        /**
         * get the result of applying the transformation in the Y direction
         * @returns the new y
         */
        double get_y(const double& x, const double& y) const;

        const double* get_A() const {return A;};
        const double* get_b() const {return b;};

    protected:
        Affine2D();
        double A[4];
        double b[2];
};

//forward declaration
class VideoFrame;
typedef Glib::RefPtr<VideoFrame> pVideoFrame;

/**
 * Extrapolation Method
 */
enum ExtrapolationMethod{
    EXTRAPOLATION_NONE,
    EXTRAPOLATION_CONST,
    EXTRAPOLATION_LINEAR
};

/**
 * Interpolation Method
 */
enum InterpolationMethod {
    INTERPOLATION_NEAREST,
    INTERPOLATION_BILINEAR,
    INTERPOLATION_BICUBIC,
    INTERPOLATION_LANCZOS};

/**
 * Hold a frame of video
 */
class VideoFrame : public Glib::Object
{
    public:
        virtual ~VideoFrame();
        /*
         * Create an empty Frame
         */
        static pVideoFrame create();

        /*
         * Create a frame from raw data
         * @param data The raw data to use, 24bpp RGB format
         * @param width image width
         * @param height image height
         * @param rowstride image width plus padding
         * @param copy if true, inisialise a copy of the data leaving the
         * caller responsible for the original. If false, responsibility to
         * delete the data is passed to the VideoFrame
         * @param timestamp the timestamp of the frame, defaults to none
         */
        static pVideoFrame create_from_data(
                uint8_t* data,
                int width,
                int height,
                int rowstride,
                bool copy=true,
                int64_t _timestamp=-1,
                int64_t _position=-1);

        /*
         * frame height
         */
        int get_height() const;
        /*
         * frame width
         */
        int get_width() const;
        /*
         * size of each padded row
         */
        int get_rowstride() const;
        /*
         * data size
         */
        int get_datasize() const;
        /*
         * when to display the frame in ms
         */
        int64_t get_timestamp() const; 
        /*
         * the position of the frame in the stream
         */
        int64_t get_position() const; 

        /*
         * get a const pointer to the data
         */
        const uint8_t* get_data() const;

        /*
         * get a pointer to the data
         */
        uint8_t* get_data();

        /*
         * Check if the VideoFrame is initiated
         */
        bool is_ok() const;


        // ############################################## Operations

        /**
         * Return a crop of the VideoFrame (don't duplicate data)
         * @param left 
         * @param right
         * @param width pass -1 to preserve image
         * @param height pass -1 to preserve image
         * @returns the new frame, a subset of the current frame, or NULL if 
         *          parameters were out of bounds
         */
        pVideoFrame crop(int left, int right, int width, int height) const;

        /**
         * Return a scaled version of the VideoFrame
         * @param width target width
         * @param height target height
         * @returns the new frame
         */
        pVideoFrame scale(int width, int height) const;

        /**
         * Return a transformed version of the VideoFrame
         * @param transform The transform to apply
         * @returns the new frame
         */
        pVideoFrame transform(const Affine2D& tr);

        /**
         * Retrieve the color at (x,y)
         * @param x
         * @param y
         * @param out will point to the data
         * @param mode how to handle (x,y) outside image. 
         *  INTERPOLATION_NONE: return black
         *  INTERPOLATION_CONST: return the value of the nearest pixel in the image
         *  INTERPOLATION_LINEAR: linearly extrapolate from the last two pixels
         * @returns whether or not out should be deleted after use
         */
        bool extrapolate(int x, 
                         int y, 
                         uint8_t*& out,
                         ExtrapolationMode mode=EXTRAPOLATION_CONST);
         

        /**
         * Interpolate to estimate a value at an arbitrary position
         * @param x x position
         * @param y y position
         * @param out location to store output, just return it if NULL
         * @param mode the Interpolation mode to use
         * @returns interpolated color. black if (x,y) is outside image 
         */
        uint8_t* value_at(double x, 
                          double y,
                          uint8_t* out,
                          InterpolationMode mode=INTERPOLATION_BILINEAR) const;

    protected:
        VideoFrame();

    private:
        void init(uint8_t* _data, int w, int h, int r, int64_t t, int64_t p); 
        void interpolate_nearest(double x, double y, uint8_t* out) const;
        void interpolate_bilinear(double x, double y, uint8_t* out) const;
        void interpolate_bicubic(double x, double y, uint8_t* out) const;
        //void interpolate_lanczos(double x, double y, uint8_t* out) const;

        void extrapolate_linear(int x, int y, uint8_t* out) const;

        uint8_t offset(int x, int y) const;

        int height, width, rowstride;
        int64_t timestamp, position;
        uint8_t* data;
        //keep a pointer to the data parent (if any) so that the data isn't deleted
        pVideoFrame data_parent;
};


#endif // GTKMM_GIFAUTHOR_VIDEOFRAME_H
