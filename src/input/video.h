#ifndef GTKMM_GIFAUTHOR_VIDEO_H
#define GTKMM_GIFAUTHOR_VIDEO_H

#include <glibmm/main.h>
#include "videoframe.h"

/**
 * Abstract base class for VideoFile and VideoSequence
 */
class Video : public Glib::Object
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
         * @returns RefPtr to the fetched frame, points to NULL on failure
         */
        virtual pVideoFrame get_frame() = 0;

};
         

#endif // GTKMM_GIFAUTHOR_VIDEO_H
