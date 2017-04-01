#include "gifencoder.h"

//utility to compare pixels
bool px_equal(uint8_t *lhs, uint8_t *rhs)
{
    return (lhs[0]==rhs[0] && lhs[1]==rhs[1] && lhs[2] == rhs[2]);
};

GIFEncoder::GIFEncoder(int cw, int ch,
                       pSegmenter segmenter,
                       pDitherer ditherer,
                       pColorQuantizer colorquantizer,
                       int max_colors):
    canvas_width(cw),
    canvas_height(ch),
    segmenter(segmenter),
    ditherer(ditherer),
    colorquantizer(colorquantizer),
    sm_sigma(2.),
    sm_thresh(0.5),
    max_colors(max_colors)
{};

GIFEncoder::~GIFEncoder() {};

pGIF GIFEncoder::encode(pSequence frames)
{
    pcGIFColorTable ct;
    pGIFImage img;
    pFrame cropped;
    pGIF out = GIF::create(canvas_width, canvas_height);

    segmenter->segment(frames);

    for(auto fr : *frames)
    {
        ct = colorquantizer->quantize(fr, max_colors);
        if(!ct)
            throw("ColorQuantizer failed");
        img = ditherer->dither_image(fr, ct);
        if(!img)
            throw("Ditherer Failed");
        out->push_back(img);
    }
    
    return out;
};

 

