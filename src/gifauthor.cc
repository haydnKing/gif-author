#include "gifauthor.h"

GAFrame::GAFrame(pVideoFrame& _pVF, int _delay_t) 
{
};

GAFrame::~GAFrame()
{
};

std::vector<pGIFImage> GAFrame::process(int w, int h, InterpolationMethod im)
{
};

bool GAFrame::get_color_table(GIFColorTable *& ct, pVideoFrame scaled_vf) const
{
};

bool GAFrame::dither_image(GIFImage& out, GIFColorTable& ct, pVideoFrame scaled_vf) const
{
};
