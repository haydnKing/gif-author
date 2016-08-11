#ifndef GTKMM_GIFAUTHOR_DEBLOCKING_H
#define GTKMM_GIFAUTHOR_DEBLOCKING_H

#include "videoframe.h"

#include "../util/process.h"

class Deblocker : public Process
{
    public:
        Deblocker();
        virtual ~Deblocker();

        pVideoFrame deblock(const pVideoFrame vf);

    private:
        int block_size;
}


#endif
