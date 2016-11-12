#include "gifencoder.h"

//utility to compare pixels
bool px_equal(uint8_t *lhs, uint8_t *rhs)
{
    return (lhs[0]==rhs[0] && lhs[1]==rhs[1] && lhs[2] == rhs[2]);
};

GIFEncoder::GIFEncoder(int cw, int ch):
    canvas_width(cw),
    canvas_height(ch),
    sm_sigma(2.),
    sm_thresh(0.5)
{};

GIFEncoder::~GIFEncoder() {};

void GIFEncoder::push_frame(pVideoFrame fr)
{
    frames.push_back(fr);
};

GIF *GIFEncoder::get_output()
{
    GIF *out = new GIF(canvas_width, canvas_height);
    pVideoFrame fr;
    pBitset fr_mask;
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    const uint8_t *px;
    int64_t last_timestamp, delay = 4;

    std::vector<pBitset> masks;
    std::vector<pVideoFrame> sframes;
    Segmenter *sm = segmenterFactory.get_selected();
    sm->segment(frames,  masks);
    smooth_transparency(frames, sframes, masks);

    std::stringstream ss;
    //VideoFrame::write_ppm(frames, "dbg/s_input");
    //VideoFrame::write_ppm(sframes, "dbg/s_output");

    std::cout << "Background detection done" << std::endl;
    std::cout << "frames: " << frames.size() << std::endl;
    std::cout << "sframes: " << sframes.size() << std::endl;
    std::cout << "masks: " << masks.size() << std::endl;

    Ditherer *ditherer = dithererFactory.get_selected();

    for(int i = 0; i < sframes.size(); i++)
    {
        std::cout << "Frame " << i << " of " << frames.size() << std::endl;
        fr = sframes[i];
        fr_mask = masks[i];

        //get the quantizer
        ColorQuantizer *cq = quantizerFactory.get_selected();
        cq->set_max_colors(fr->get_height() * fr->get_width());
        
        if(fr_mask)
        {
            for(y = 0; y < fr->get_height(); y++)
                for(x = 0; x < fr->get_width(); x++)
                    if(fr_mask->get(x,y)) 
                    {
                        cq->add_color(fr->get_pixel(x,y));
                    }
        }
        else
        {
            for(y = 0; y < fr->get_height(); y++)
                for(x = 0; x < fr->get_width(); x++)
                    cq->add_color(fr->get_pixel(x,y));
        }

        if(cq->get_num_colors() > 0)
        {
            //currently just using 256 colours for everything
            cq->build_ct((bool)fr_mask, 255);

            //autocrop
            pGIFImage img;
            if(fr_mask)
            {
                int left   = fr_mask->get_left(),
                    width  = fr_mask->get_right() - left,
                    top    = fr_mask->get_top(),
                    height = fr_mask->get_bottom() - top;
                std::cout << "img[" << i <<"] = "<<left<<","<<top<<"+"<<width<<"x"<<height << std::endl;
                fr_mask = Bitset::crop(fr_mask, left, top, width, height);
                fr = fr->crop(left, top, width, height);
                img = ditherer->dither_image(fr, fr_mask, cq->get_ct());
                img->set_left(left);
                img->set_top(top);
            }
            else {
                //Dither the image
                img = ditherer->dither_image(fr, fr_mask, cq->get_ct());
            }
            img->set_disposal_method(DISPOSAL_METHOD_NONE);
            //debug_ct(img, cq->get_ct());
            ss.str("");
            ss << "dbg/quantized" << std::setw(5) << std::setfill('0') << i << ".ppm";
            img->write_ppm(ss.str().c_str());
            //ss.str("");
            //ss << "dbg/colortable" << i << ".ppm";
            //cq->get_ct()->write_ppm(ss.str().c_str());
            
            //set delay_time
            if(i > 0)
            {
                delay = (frames[i]->get_timestamp() - last_timestamp)/10;
                out->back()->set_delay_time(delay);
            }
            last_timestamp = frames[i]->get_timestamp();
            
            std::cout << "push_back() i = " << i << " transparency : " << (bool)fr_mask << std::endl;
            out->push_back(img);
        }
    }
    //set delay for last frame
    if(sframes.size() > 0) out->back()->set_delay_time(delay);

    std::cout << "Length of returned GIF: " << out->size() << std::endl;

    return out;
};

void GIFEncoder::smooth_transparency(const std::vector<pVideoFrame> frames, 
                                     std::vector<pVideoFrame>& out_frames,
                                     std::vector<pBitset>& bits) {
    int x,y,z,start;
    double r,g,b;
    uint8_t *px;

    //smooth the out bits
    for(z = 0; z < frames.size(); z++)
    {
        if(bits[z])
            bits[z] = Bitset::smooth(bits[z], sm_sigma, sm_thresh);
    }

    //calculate the value for each out frame
    for(z = 0; z < frames.size(); z++)
    {
        out_frames.push_back(VideoFrame::create(frames[z]->get_width(), frames[z]->get_height()));
    }
    for(y = 0; y < frames[0]->get_height(); y++)
    {
        for(x = 0; x < frames[0]->get_width(); x++)
        {
            start = 0;
            px = frames[0]->get_pixel(x,y);
            r = px[0];
            g = px[1];
            b = px[2];

            for(z = 1; z < frames.size(); z++)
            {
                if(!bits[z] || bits[z]->get(x,y)) {
                    r /= z-start;
                    g /= z-start;
                    b /= z-start;
                    out_frames[start]->set_pixel(x,y,uint8_t(r+0.5),uint8_t(g+0.5),uint8_t(b+0.5));
                    r = g = b = 0.;
                    start = z;
                }
                px = frames[z]->get_pixel(x,y);
                r += px[0];
                g += px[1];
                b += px[2];
            }
            r /= z-start;
            g /= z-start;
            b /= z-start;
            out_frames[start]->set_pixel(x,y,uint8_t(r+0.5),uint8_t(g+0.5),uint8_t(b+0.5));
        }
    }
}
        
void GIFEncoder::dbg_save_POI(int x, int y, const char* name) const
{
    std::stringstream ss;
    ss << "./POI/POI_" << x << "_" << y << "_" << name << ".csv";
    std::ofstream f(ss.str().c_str());
    f << "r, g, b" << std::endl;
    const uint8_t* px;

    for(int i = 0; i < frames.size(); i++)
    {
        px = frames[i]->get_pixel(x,y);
        f << int(px[0]) << ", " << int(px[1]) << ", " << int(px[2]) << std::endl;
    }
};
        
void GIFEncoder::dbg_thresholding(int len, uint8_t *px, float *fpx, const char *name) const
{
    std::stringstream ss;
    ss << "./POI/thresh_" << name << ".csv";
    std::ofstream f(ss.str().c_str());
    f << "r, g, b, r, g, b" << std::endl;

    for(int i = 0; i < len; i++)
    {
        f << int(px[3*i+0]) << ", "
          << int(px[3*i+1]) << ", "
          << int(px[3*i+2]) << ", "
          << fpx[3*i+0] << ", "
          << fpx[3*i+1] << ", "
          << fpx[3*i+2] << std::endl;
    }
};

pGIFImage GIFEncoder::create_gif_image(int left, int top, int width, int height) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(left,
                                 top, 
                                 width, 
                                 height);

    return pGIFImage(ret);
};
 

