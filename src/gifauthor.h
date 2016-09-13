#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

/**
 * Underlying application code, does not require UI
 */
#include <gtkmm.h>
#include <iostream>

#include "encoder/gifencoder.h"


/**
 * Central class
 */
class GIFAuthor : public Gtk::Application
{
    protected:
        GIFAuthor();
        ~GIFAuthor();
    public:
    
        static Glib::RefPtr<GIFAuthor> create();

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
        void register_command_line();
        int on_handle_local_options(const Glib::RefPtr<Glib::VariantDict>& options);
        void from_images(std::vector<std::string> fnames, int delay, int width);
        bool parse_width_height(const Glib::ustring& name, const Glib::ustring& value, bool has_value);
        
        int out_width, out_height;
        
        std::vector<pVideoFrame> frames;
        GIF *out;
        
        SegmenterFactory segmenterFactory;
        QuantizerFactory quantizerFactory;
        DithererFactory dithererFactory;
};



#endif // GTKMM_GIFAUTHOR_H
