#ifndef GTKMM_GIFAUTHOR_DEBLOCKING_H
#define GTKMM_GIFAUTHOR_DEBLOCKING_H

#include "videoframe.h"

#include "../util/process.h"

class Deblocker : public Process
{
    public:
        Deblocker(int width, int height);
        virtual ~Deblocker();

        void process(std::vector<pVideoFrame> frames);
        void deblock(pVideoFrame vf);

    private:
        float vscore(const pVideoFrame vf, int x, int y_min, int y_max);
        float hscore(const pVideoFrame vf, int y, int x_min, int x_max);
        int width, height;
        pVideoFrame buff;
};


#endif
