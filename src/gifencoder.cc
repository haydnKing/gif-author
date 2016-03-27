#include "gifencoder.h"

//utility to compare pixels
bool px_equal(uint8_t *lhs, uint8_t *rhs)
{
    return (lhs[0]==rhs[0] && lhs[1]==rhs[1] && lhs[2] == rhs[2]);
};

GIFEncoder::GIFEncoder(int cw, int ch, QuantizerMethod _qm, DitherMethod _dm):
    canvas_width(cw),
    canvas_height(ch),
    qm(_qm),
    dm(_dm)
{};

GIFEncoder::~GIFEncoder() {};

void GIFEncoder::push_frame(pVideoFrame fr)
{
    frames.push_back(fr);
};

GIF *GIFEncoder::get_output()
{
    dbg_save_POI(220, 105, "elbow");
    dbg_save_POI(170, 88, "light");
    dbg_save_POI(104, 97, "wall1");
    dbg_save_POI(205, 95, "wall2");
    dbg_save_POI(272,166, "hip");

    GIF *out = new GIF(canvas_width, canvas_height);
    pVideoFrame fr, prev_fr;
    std::vector<pVideoFrame>::iterator it;
    int x, y;
    const uint8_t *px, *prev_px;
    int64_t last_timestamp, delay = 4;
    int64_t frame_no = 0;

    std::vector<pVideoFrame> sframes = simplify();
    std::cout << "Background detection done" << std::endl;

    for(int i = 0; i < sframes.size(); i++)
    {
        std::cout << "Frame " << i << " of " << frames.size() << std::endl;
        fr = sframes[i];
        if(i==0)
            prev_fr = pVideoFrame();
        else
            prev_fr = sframes[i-1];

        //create a quantizer
        pColorQuantizer cq = ColorQuantizer::get_quantizer(qm);
        cq->set_max_colors(fr->get_height() * fr->get_width());
        
        for(y = 0; y < fr->get_height(); y++)
        {
            for(x = 0; x < fr->get_width(); x++)
            {
                if(prev_fr)
                {
                    if(!px_equal(prev_fr->get_pixel(x,y), fr->get_pixel(x,y)))
                        cq->add_color(fr->get_pixel(x,y));
                }
                else
                    cq->add_color(fr->get_pixel(x,y));
            }
        }

        if(cq->get_num_colors() > 0)
        {
            //Dither the image
            pGIFImage img = create_gif_image(0,0,fr->get_width(),fr->get_height());
            dither_image(img, fr, prev_fr, cq, 255);
            //debug_ct(img, cq->get_ct());
            
            //set delay_time
            if(i > 0)
            {
                delay = (frames[i]->get_timestamp() - last_timestamp)/10;
                out->back()->set_delay_time(delay);
            }
            last_timestamp = frames[i]->get_timestamp();
            
            out->push_back(img);
        }
    }
    //set delay for last frame
    out->back()->set_delay_time(delay);

    return out;
};

void GIFEncoder::dither_image(pGIFImage out,
                              const pVideoFrame vf,
                              const pVideoFrame pvf,
                              const pColorQuantizer cq,
                              uint8_t colors) const
{
    if(pvf)
    {
        out->set_transparency(true);
        out->set_transparent_index(colors);
        out->set_disposal_method(DISPOSAL_METHOD_NONE);
        //out->set_disposal_method(DISPOSAL_METHOD_RESTORE_BACKGROUND);
        cq->build_ct(colors-1);
    }
    else
        cq->build_ct(colors);
    out->set_local_colortable(cq->get_ct());
    switch(dm)
    {
        case DITHER_FLOYD_STEINBERG:
            dither_FS(vf, pvf, out, cq);
            break;
        case DITHER_NONE:
            dither_none(vf, pvf, out, cq);
            break;
    }
};

void GIFEncoder::dither_FS(const pVideoFrame vf,
                           const pVideoFrame pvf,
                           pGIFImage out, 
                           const pColorQuantizer cq) const
{
    std::cout << "dither_fs" << std::endl;
    //store 2 rows of RGB errors, set to zero
    int32_t* errors = new int32_t[6*vf->get_width()];
    std::memset(errors, 0, 6*vf->get_width()*sizeof(int32_t));
    //pointers to the errors that can be swapped around
    int32_t *this_row = errors, 
            *next_row = errors + 3*vf->get_width(), 
            *swap;
    uint8_t pixel[3], * color;
    int32_t error[3];

    int x, y, index, i;

    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //transparency
            if(pvf && px_equal(vf->get_pixel(x,y), pvf->get_pixel(x,y)))
            {
                out->set_value(x,y,out->transparent_index());
                continue;
            }
            //get the pixel
            std::memcpy(pixel, vf->get_pixel(x,y), 3*sizeof(uint8_t));
            //add the errors, being wary of overflow
            for(i = 0; i < 3; i++)
            {
                error[i] = this_row[3*x+i] + int32_t(pixel[i])*256;
                pixel[i] = (uint8_t)std::max(0, std::min(UINT8_MAX, (error[i]+128)/256));
            }

            //get the closest ct
            index = cq->map_to_ct(pixel);
            //set the pixel
            out->set_value(x, y, index);
            //calculate the errors. Shift 8 places for accuracy
            for(i = 0; i < 3; i++)
                error[i] -= int32_t(cq->get_ct()->get_index(index)[i])*256;

            
            //propagate the errors
            if(x+1 < vf->get_width())
            {
                for(i=0;i<3;i++)
                    this_row[3*x+3+i] += (7 * error[i]) / 16;
            }
            if(y+1 < vf->get_height())
            {
                if(x > 0)
                    for(i=0;i<3;i++)
                        next_row[3*x-3+i] += (3 * error[i]) / 16;
                for(i=0;i<3;i++)
                    next_row[3*x+i] += (5 * error[i]) / 16;
                if(x+1 < vf->get_width())
                    for(i=0;i<3;i++)
                        next_row[3*x+3+i] += error[i] / 16;
            }
        }
        //moving to the next row
        swap = this_row;
        this_row = next_row;
        next_row = this_row;
        memset(next_row, 0, 3*vf->get_width()*sizeof(int32_t));
    }

    //the best way to cover up mistakes ;)
    delete [] errors;
};

void GIFEncoder::dither_none(const pVideoFrame vf,
                             const pVideoFrame pvf,
                             pGIFImage out, 
                             const pColorQuantizer cq) const
{
    std::cout << "dither_none" << std::endl;
    int x,y,index;
    for(y = 0; y < vf->get_height(); y++)
    {
        for(x = 0; x < vf->get_width(); x++)
        {
            //transparency
            if(pvf && px_equal(vf->get_pixel(x,y), pvf->get_pixel(x,y)))
            {
                out->set_value(x,y,out->transparent_index());
                continue;
            }
            index = cq->map_to_ct(vf->get_pixel(x,y));
            out->set_value(x, y, index);
        }
    }
    
};
        
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

std::vector<pVideoFrame> GIFEncoder::detect_bg() const
{
    std::cout << "Detect background" << std::endl;
    cv::Ptr<cv::BackgroundSubtractor> bsub = cv::createBackgroundSubtractorMOG2(frames.size());
    cv::Mat mask;
    cv::Mat *frame;

    std::vector<pVideoFrame> ret, ret2;
    std::vector<pVideoFrame>::const_iterator it;
    pVideoFrame fr;

    std::ofstream f("POI.csv");
    f << "r, g, b" << std::endl;
    const uint8_t* px;

    for(it = frames.begin(); it != frames.end(); it++)
    {
        px = (*it)->get_pixel(304,102);
        f << int(px[0]) << ", " << int(px[1]) << ", " << int(px[2]) << std::endl;
        frame = (*it)->get_mat();

        bsub->apply(*frame, mask);
        ret.push_back(VideoFrame::create_from_mat(&mask, 
                                                  (*it)->get_timestamp(), 
                                                  (*it)->get_position()));
        delete frame;
    }

    bool high;
    for(int i = 0; i < ret.size(); i++)
    {
        fr = VideoFrame::create(ret[i]->get_width(), ret[i]->get_height(), 0); 
        for(int y = 0; y < ret[i]->get_height(); y++)
        {
            for(int x = 0; x < ret[i]->get_width(); x++)
            {
                high = false;
                if(x>0 && y>0                                      && ret[i]->get_pixel(x-1,y-1)[0] > 0) high=true;
                if(x>0                                             && ret[i]->get_pixel(x-1,y  )[0] > 0) high=true;
                if(x>0 && y<ret[i]->get_height()                   && ret[i]->get_pixel(x-1,y+1)[0] > 0) high=true;
                if(x<ret[i]->get_width() && x>0                    && ret[i]->get_pixel(x+1,y-1)[0] > 0) high=true;
                if(x<ret[i]->get_width()                           && ret[i]->get_pixel(x+1,y  )[0] > 0) high=true;
                if(x<ret[i]->get_width() && x<ret[i]->get_width()  && ret[i]->get_pixel(x+1,y+1)[0] > 0) high=true;
                if(y>0                                             && ret[i]->get_pixel(x,y-1)[0] > 0) high=true;
                if(y<ret[i]->get_height()                          && ret[i]->get_pixel(x,y+1)[0] > 0) high=true;

                if(high) fr->set_pixel(x,y,255,255,255);
            }
        }
        ret2.push_back(fr);
    }

    for(int i = 0;  i < ret2.size(); i++)
    {
        frame = ret2[i]->get_mat();
        std::stringstream ss;
        ss << "./mask/frame_" << i << ".jpg";
        cv::imwrite(ss.str().c_str(), *frame);
        delete frame;
    }

    return ret2;
};


std::vector<pVideoFrame> GIFEncoder::get_optical_flow() const
{
    cv::Mat *prev, *next, flow, *save;
    std::vector<pVideoFrame> ret;
    pVideoFrame frame;
    ret.push_back(VideoFrame::create(frames[0]->get_width(), frames[0]->get_height(), 0));

    prev = frames[0]->get_mat();
    cv::cvtColor(*prev, *prev, CV_RGB2GRAY);

    for(int i = 1; i < frames.size(); i++)
    {
        next = frames[i]->get_mat();
        cv::cvtColor(*next, *next, CV_RGB2GRAY);

        cv::calcOpticalFlowFarneback(*prev, *next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        frame = VideoFrame::create(frames[0]->get_width(), frames[0]->get_height(), 0);

        float fv;
        uint8_t v;
        for(int y = 0; y < flow.rows; y ++)
            for(int x = 0; x < flow.cols; x ++)
            {
                const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
                fv = std::sqrt(fxy.x*fxy.x + fxy.y+fxy.y)*10;
                if(fv > 255.5) fv = 255.5;
                v = uint8_t(fv);
                frame->set_pixel(x,y, v, v, v); 
            }

        save = frame->get_mat();
        std::stringstream ss;
        ss << "./flow/frame_" << i << ".jpg";
        cv::imwrite(ss.str().c_str(), *save);
        delete save;

        ret.push_back(frame);

        delete prev;
        prev = next;
    };
    delete next;

    return ret;
};

std::vector<pVideoFrame> GIFEncoder::simplify(float alpha, float sig) const
{
    int i, j,k, y, x;

    uint8_t *pixels = new uint8_t[3*frames.size()], *last;
    float  *fpixels = new float[3*frames.size()], *flast;

    int kernel_length =int(6*sig) + 1;
    int kernel_center = kernel_length/2;
    float kernel[kernel_length], norm;
    for(j=0; j < kernel_length; j++)
    {
        kernel[j] = std::exp(-(j-kernel_center)*(j-kernel_center)/(2*sig*sig));
    }

    //prepare output images
    std::vector<pVideoFrame> ret;
    for(i=0; i < frames.size(); i++)
        ret.push_back(VideoFrame::create(frames[i]->get_width(), frames[i]->get_height(), 0));

    unsigned long c = 0;
    //pixel by pixel
    for(y = 0; y < frames[0]->get_height(); y++)
    {
        for(x = 0; x < frames[0]->get_width(); x++)
        {
            //get the pixels
            for(i = 0; i < frames.size(); i++)
            {
                std::memcpy(pixels+3*i, frames[i]->get_pixel(x,y), 3*sizeof(uint8_t));
            }

            //smooth
            for(i=0; i<frames.size(); i++)
            {
                for(j=0; j<3; j++)
                {
                    fpixels[3*i+j] = 0;
                    norm = 0;
                    for(k=0; k < kernel_length; k++)
                    {
                        if((i+k-kernel_center > 0) &&
                           (i+k-kernel_center < frames.size()))
                        {
                            fpixels[3*i+j] += kernel[k] * float(pixels[3*(i+k-kernel_center)+j]);
                            norm += kernel[k];
                        }
                    }
                    fpixels[3*i+j] /= norm;
                }
            }

            //deltas

            int run_start = 0;
            bool update[ret.size()];
            std::memset(update, 0, ret.size()*sizeof(bool));
            update[0] = true;
            for(i=1; i < ret.size(); i++)
            {
                for(j=0; j < 3; j++)
                {
                    if(std::abs(fpixels[3*i+j] - fpixels[3*run_start+j]) > alpha)
                    {
                        run_start = i;
                        update[i] = true;
                    }
                }
            }
            

            //Fill in pixel values
            for(i=0; i < ret.size(); i++)
            {
                if(update[i])
                {
                    last = pixels+3*i;
                    ret[i]->set_pixel(x,y,last[0], last[1], last[2]);
                }
                else
                    ret[i]->set_pixel(x,y,last[0], last[1], last[2]);
            }
            

        }
    }

    return ret;
};

pGIFImage GIFEncoder::create_gif_image(int left, int top, int width, int height) const
{
    //Create the output image
    GIFImage *ret = new GIFImage(left,
                                 top, 
                                 width, 
                                 height);

    return Glib::RefPtr<GIFImage>(ret);
};
 

