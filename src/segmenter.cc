#include "segmenter.h"

class DeltaSegmenter;
class SmoothDeltaSegmenter;

Segmenter *Segmenter::get_segmentor(SegmentationMethod method)
{
    Segmenter *ret;
    switch(method)
    {
        case SM_SIMPLE_DELTA:
            ret = new DeltaSegmenter();
    }
    return ret;
};

/*******************************************************************
 *************************************************** DELTA SEGMENTER
 *******************************************************************/

class DeltaSegmenter : public Segmenter
{
    public:
        DeltaSegmenter() {};
        ~DeltaSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);
};

void DeltaSegmenter::segment(const std::vector<pVideoFrame> frames, 
                             std::vector<pVideoFrame>& out_frames,
                             std::vector<pBitset>& out_bits)
{
    //worry about where to get settings later
    float delta = 2.0;

    //No transparency in the first frame
    out_bits.push_back(pBitset());

    //cycle through each frame
    int i, x,y;
    pBitset out;
    uint8_t *px_this, *px_prev;
    for(i=1; i < frames.size(); i++)
    {
        out = pBitset::create(frames[i]->get_width(), frames[i]->get_height(), false);
        for(y=0; y<frames[i]->get_height(); y++)
        {
            for(x=0; x < frames[i]->get_width(); x++)
            {
                px_this = frames[i]->get_pixel(x,y);
                px_prev = frames[i-1]->get_pixel(x,y);
                if(std::abs(px_this[0] - px_prev[0]) > delta ||
                   std::abs(px_this[1] - px_prev[1]) > delta ||
                   std::abs(px_this[2] - px_prev[2]) > delta)
                    out->set(x,y,true);
            }
        }
        out_bits.push_back(out);
    }
};
    
/*******************************************************************
 ******************************************** PER-PIXEL SEGMENTER
 *******************************************************************/

/*
 * Extract the timeline for individual pixels
 */
class PixelSegmenter : public Segmenter
{
    public:
        PixelSegmenter() {};
        ~PixelSegmenter() {};

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);

    protected:
        virtual void process_pixel(int length, uint8_t *in, uint8_t *out, bool *changed) = 0;

};

void PixelSegmenter::segment(const std::vector<pVideoFrame> frames, 
                                std::vector<pVideoFrame>& out_frames,
                                std::vector<pBitset>& out_bits)
{
    int x, y, z, w = frames[0]->get_width(), h = frames[0]->get_height();
    uint8_t *i_px = new uint8_t[3*frames.size()],
            *o_px = new uint8_t[3*frames.size()];
    bool *changed = new bool[frames.size()];
    const uint8_t *px; 
    std::vector<pVideoFrame> t_out_frames;
    std::vector<pBitset> t_out_bits;

    for(z = 0; z < frames.size(); z++)
    {
        t_out_frames.push_back(VideoFrame::create(w,h));
        t_out_bits.push_back(BitSet::create(w,h));
    }

    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            //get the original pixels
            for(z=0; z < frames.size(); z++)
            {
                px = frames[z]->get_pixel(x,y);
                i_px[3*z  ] = px[0];
                i_px[3*z+1] = px[1];
                i_px[3*z+2] = px[2];
            }

            process_pixel(frames.size(), i_px, o_px, changed);

            //set the processed pixels and bits
            for(z = 0; z < frames.size(); z++)
            {
                px = t_out_frames[z]->get_pixel(x,y);
                px[0] = o_px[3*z  ];
                px[1] = o_px[3*z+1];
                px[2] = o_px[3*z+2];
                t_out_bits[z]->set_bit(x,y,changed[z]);
            }

        }
    }

    out_frames.insert(out_frames.end(), t_out_frames.begin(), t_out_frames.begin());
    out_bits.insert(out_bits.end(), t_out_bits.begin(), t_out_bits.begin());

    delete [] i_px;
    delete [] o_px;
    delete [] changed;
};
    
/*******************************************************************
 ****************************************** Smooth Delta Segmenter
 *******************************************************************/

/*
 * A per pixel segmenter which first identifies scene transitions
 */
class SmoothDeltaSegmenter : public PixelSegmenter
{
    public:
        SmoothDeltaSegmenter();
        ~SmoothDeltaSegmenter();

        void segment(const std::vector<pVideoFrame> frames, 
                     std::vector<pVideoFrame>& out_frames,
                     std::vector<pBitset>& out_bits);

    protected:
        virtual void process_pixel(int length, uint8_t *in, uint8_t *out, bool *changed);

        static void get_max_d(uint8_t* lhs, uint8_t* rhs, uint8_t &curr);

        //settings
        float sig_space=2.0, 
              sig_time=2.0,
              alpha=0.85, //scene detection
              delta_scene=1.0, //scene detection
              smooth_limit=30., //don't smooth mountains or troughs greater than this
              delta_pixel=5.0;

        uint32_t *kernel;
        int kernel_length, kernel_center;
};
        
SmoothDeltaSegmenter::SmoothDeltaSegmenter() 
{
    kernel_length = int(6*sig_time)+1;
    if(kernel_length%2 == 0) kernel_length++;
    kernel_center = kernel_length/2;

    kernel = new uint32_t[kernel_length];
    
    for(int j=0; j < kernel_length; j++)
    {
        //scale kernel so that precision should be OK
        kernel[j] = uint32_t((UINT32_MAX / (1000*2.506*sig_t)) * std::exp(-(j-kernel_center)*(j-kernel_center)/(2*sig_t*sig_t)));
        std::cout << "kernel[" << j << "] = " << kernel[j] << std::endl;
    }
};
        
SmoothDeltaSegmenter::~SmoothDeltaSegmenter() 
{
    delete [] kernel;
};

void SmoothDeltaSegmenter::segment(const std::vector<pVideoFrame> frames, 
                                   std::vector<pVideoFrame>& out_frames,
                                   std::vector<pBitset>& out_bits)
{
    std::vector<pVideoFrame> blurred_frames;
    for(frames::const_iterator it = frames.begin(); it != frames.end(); it++)
    {
        blurred_frames.push_back((*it)->blur(sig_space));
    }
    //v1 assume no scenes
    PixelSegmenter::segment(blurred_frames, out_frames, out_bits);
};
        
void SmoothDeltaSegmenter::get_max_d(uint8_t* lhs, uint8_t* rhs, uint8_t &curr)
{
    for(int i = 0; i < 3; i++)
    {
        if(rhs[i] > lhs[i])
        {
            if(curr < (rhs[i]-lhs[i]))
                curr = rhs[i]-lhs[i];
        }
        else if(curr < (lhs[i]-rhs[i]))
            curr = rhs[i]-lhs[i];
    }
};

void SmoothDeltaSegmenter::process_pixel(int length, 
                                         uint8_t *in, 
                                         uint8_t *out, 
                                         bool *changed)
{
    uint32_t r,g,b,n;
    uint8_t max_d;
    int i,j,k;
    //smooth the input and save it in out
    for(i = 0; i < length; i++)
    {
        max_d = 0;
        if(i > 0)
            get_max_d(in+3*(i-1), in+3*i, max_d);
        if(i < length-1)
            get_max_d(in+3*i, in+3*(i+1), max_d);

        //only smooth if max_d is greater than threshold
        if(max_d > smooth_limit)
        {
            out[3*i] = in[3*i];
            out[3*i+1] = in[3*i+1];
            out[3*i+2] = in[3*i+2];
        }
        else
        {
            r = g = b = n = 0;
            for(k = 0; k < kernel_length; k++)
            {
                j = i + k - kernel_center;
                if((j) > 0 &&
                   (j) < length)
                {
                    n += kernel[k];
                    r += kernel[k] * in[3*j];
                    g += kernel[k] * in[3*j+1];
                    b += kernel[k] * in[3*j+2];
                }
            }
            out[3*i  ] = uint8_t(r/n);
            out[3*i+1] = uint8_t(g/n);
            out[3*i+2] = uint8_t(b/n);
        }
    }


    //now update when the step in the smoothed version is larger than step
    //always update the first pixel
    changed[0] = true;
    for(i=1; i < length; i++)
    {
        max_d = 0;
        get_max_d(in+3*(i-1), in+3*i, max_d);
        changed[i] = (max_d > delta_pixels);
    }


    //now fill in average values
    j = 0;
    r = in[0];
    g = in[1];
    b = in[2];
    for(i=1; i < length; i++)
    {
        if(changed[i])
        {
            out[3*j  ] = r / (i-j);
            out[3*j+1] = g / (i-j);
            out[3*j+2] = b / (i-j);
            r = g = b = 0;
            j = i;
        }

        r += in[3*i  ];
        g += in[3*i+1];
        b += in[3*i+2];
    }
    out[3*j  ] = r / (i-j);
    out[3*j+1] = g / (i-j);
    out[3*j+2] = b / (i-j);
        

};
