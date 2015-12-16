#ifndef GIF_AUTHOR_GIF_SETTINGS_H
#define GIF_AUTHOR_GIF_SETTINGS_H

#include <gtkmm/grid.h>

#include "videoplayer.h"

/**
 * Display gif frames and settings
 */
class GIFSettings : public Gtk::Grid
{
    public:
        GIFSettings();
        virtual ~GIFSettings();

    protected:

        VideoPlayer w_video_player;

};




#endif //GIF_AUTHOR_GIF_SETTINGS_H
