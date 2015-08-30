#ifndef GTKMM_GIFAUTHOR_VIDEO_H
#define GTKMM_GIFAUTHOR_VIDEO_H


extern "C" {
#include <libavformat/avformat.h>
}

/**
 * Abstract base class for VideoFile and VideoClip
 */
class Video 
{
    public:
        /**
         * Is the video ok to read from
         */
        virtual bool is_ok() = 0;

        /**
         * The width of the video frames
         */
        virtual int width() = 0;

        /**
         * height of the video frames
         */
        virtual int height() = 0;

        /**
         * Get the index of the current frame
         * @returns index
         */
        virtual int64_t position() = 0;

        /**
         * Get the length of the sequence in frames (this may not be completely
         * accurate in all cases as FFMPEG only estimates durations for some
         * codecs)
         * @returns number of frames
         */
        virtual int64_t length() = 0;

        /**
         * Get the duration of each frame in milliseconds (assume equal length frames for
         * simplicity)
         * @return frame duration
         */
        virtual int64_t frame_duration_ms() = 0;

        /**
         * Seek to the given index
         * @param index The index of the frame to seek to
         * @param wrap whether to wrap when index is out of range (e.g.
         * index=-1 seeks to the last frame)
         * @returns true on success
         */
        virtual bool seek_to(int64_t index, bool wrap=true) = 0;

        /**
         * Get the current frame in the sequence and increment the position by
         * one
         * @param out The frame to decode into, passing a NULL value will
         * create a new frame
         * @returns true on success
         */
        virtual bool get_frame(AVFrame **out) = 0;

};
         

#endif // GTKMM_GIFAUTHOR_VIDEO_H
