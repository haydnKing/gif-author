#ifndef GTKMM_GIFAUTHOR_H
#define GTKMM_GIFAUTHOR_H

#include <gtkmm.h>
#include <iostream>
#include <memory>

#include "encoder/gifencoder.h"

class GIFAuthor;
typedef std::shared_ptr<GIFAuthor> pGIFAuthor;

/**
 * Central class
 */
class GIFAuthor : public Gtk::Application
{
    protected:
        GIFAuthor();
    public:
        virtual ~GIFAuthor();
    
        static pGIFAuthor create();

        virtual void on_open(const type_vec_files &files, const Glib::ustring &hint);

        int get_output_width() const {return out_width;};
        int get_output_height() const {return out_height;};


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
        void from_images(std::vector<std::string> fnames);
        bool parse_width_height(const Glib::ustring& name, const Glib::ustring& value, bool has_value);
        bool parse_filename(const Glib::ustring& name, const Glib::ustring& value, bool has_value);
        
        int out_width, out_height, delay;
        std::string out_file;
        
        std::vector<pVideoFrame> frames;
        GIF *out;
        
        SegmenterFactory segmenterFactory;
        QuantizerFactory quantizerFactory;
        DithererFactory dithererFactory;
};



#endif // GTKMM_GIFAUTHOR_H
