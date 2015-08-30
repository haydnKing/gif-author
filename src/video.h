#ifndef GTKMM_GIFAUTHOR_VIDEO_H
#define GTKMM_GIFAUTHOR_VIDEO_H

/**
 * Abstract base class for VideoFile and VideoClip
 */
class Video 
{
    public:
        /**
         * Abstract constructor
         */
        Video() = NULL;
        virtual ~Video() {};

        /**
         * Is the video ok to read from
         */
        virtual bool is_ok() = NULL;

        /**
         * The width of the video frames
         */
        virtual int width() = NULL;

        /**
         * height of the video frames
         */
        virtual int height() = NULL;

        /**
         * Get the index of the current frame
         * @returns index
         */
        int64_t get_position() = NULL;

        /**
         * Get the length of the sequence in frames (this may not be completely
         * accurate in all cases as FFMPEG only estimates durations for some
         * codecs)
         * @returns number of frames
         */
        int64_t get_length() = NULL;

        /**
         * Seek to the given index
         * @param index The index of the frame to seek to
         * @param wrap whether to wrap when index is out of range (e.g.
         * index=-1 seeks to the last frame)
         * @returns true on success
         */
        virtual bool seek_to(int64_t index, bool wrap=true) = NULL;

        /**
         * Get the current frame in the sequence - returns the same as the last
         * call to get_next_frame or get_prev_frame
         * @param out The frame to decode into, passing a NULL value will
         * create a new frame
         * @returns true on success
         */
        virtual bool get_frame(AVFrame **out) = NULL;

};
         

#endif // GTKMM_GIFAUTHOR_VIDEO_H
