#include "gif.h"

GIFColorTable::GIFColorTable(int depth, bool sorted) :
    depth(depth),
    sorted(sorted),
    transparent_index(-1),
    colors(0)
{
    data = new uint8_t[256*3];
    std::memset(data, 0, 256*3);
};

GIFColorTable::~GIFColorTable()
{
    delete [] data;
};
        
pGIFColorTable GIFColorTable::create(int depth, bool sorted)
{
    return pGIFColorTable(new GIFColorTable(depth, sorted));
};

void GIFColorTable::add_color(const uint8_t *c)
{
    data[3*colors    ] = c[0];
    data[3*colors + 1] = c[1];
    data[3*colors + 2] = c[2];
    colors++;
};

void GIFColorTable::set_transparent()
{
    if(is_transparent())
        return;
    transparent_index = colors;
    uint8_t c[3] = {0,0,0};
    add_color(c);
};

uint8_t GIFColorTable::log_colors() const 
{
    if(colors < 4)
        return 2;
    else
        return uint8_t(std::ceil(std::log(colors)/std::log(2)));
};

int GIFColorTable::write(std::ostream& str) const 
{
    int full_colors = std::pow(2, log_colors());
    str.write(reinterpret_cast<const char*>(data), 3*full_colors);
    return 3*full_colors;
};
        
void GIFColorTable::write_ppm(const char *fname) const
{
    int full_colors = std::pow(2, log_colors());
    std::ofstream o(fname);
    o << "P6 " << full_colors << " 1 255\n";
    o.write(reinterpret_cast<const char*>(data), 3*full_colors);
};

void GIFColorTable::finalize()
{
    //one day I'll build some kind of balanced tree here
};

int GIFColorTable::get_closest(const uint8_t *v) const
{
    //one day this will all be trees
    float m = std::numeric_limits<float>::max(),
          d;
    int idx = 0;
    for(int i = 0; i < colors; i++)
    {
        if(i == transparent_index) continue;
        
        d = ((data[3*i  ] - v[0])*(data[3*i  ] - v[0]) + 
             (data[3*i+1] - v[1])*(data[3*i+1] - v[1]) + 
             (data[3*i+2] - v[2])*(data[3*i+2] - v[2]));
        if(d < m)
        {
            idx = i;
            m = d;
        }
    }
    return idx;
};

GIFImage::GIFImage(int _left, 
                   int _top,
                   int _width, 
                   int _height, 
                   int _delay_time, 
                   pcGIFColorTable _ct) :
    left(_left),
    top(_top),
    width(_width),
    height(_height),
    ct(_ct),
    bytes(-1),
    delay_time(_delay_time),
    flag_interlaced(false),
    disposal_method(DISPOSAL_METHOD_NOT_SPECIFIED),
    flag_user_input(false)
{
    data = new uint8_t[width*height];
    std::memset(data, 0, width*height);
};

GIFImage::~GIFImage(){
    delete [] data;
};

pGIFImage GIFImage::create(int left, 
                           int top,
                           int width, 
                           int height, 
                           int delay_time, 
                           pcGIFColorTable ct)
{
    return pGIFImage(new GIFImage(left, 
                                  top,
                                  width, 
                                  height, 
                                  delay_time, 
                                  ct));
}

void GIFImage::clear_to(uint8_t code){
    std::memset(data, code, width*height);
};

const uint8_t& GIFImage::get_value(int x, int y) const {
    return data[x+y*width];
};

void GIFImage::set_value(int x, int y, uint8_t value) {
    data[x+y*width] = value;
};

int GIFImage::write(std::ostream& str, pGIFColorTable global_ct)
{
    int len = 20;
    //get the active colortable
    pcGIFColorTable active_ct = ct;
    if(active_ct == NULL){
        active_ct = global_ct;
    }

    //Graphic Control Extension
    str.put(0x21);
    str.put(0xF9);
    str.put(0x04);

    str.put(((disposal_method&0x07)<<2) + //disposal method
            (flag_user_input*0x02) + //user input
            (active_ct->is_transparent()*0x01));

    //delay time
    str.put((delay_time/10) & 0xff);
    str.put(((delay_time/10) >> 8) & 0xff);

    //transparent color index
    if(active_ct->is_transparent())
        str.put(active_ct->get_transparent_index());
    else
        str.put(0);

    //block terminator
    str.put(0x00);
    

    //Image Separator
    str.put(0x2c);
    //left
    str.put(left & 0xff);
    str.put((left >> 8) & 0xff);
    //top
    str.put(top & 0xff);
    str.put((top >> 8) & 0xff);
    //width
    str.put(width & 0xff);
    str.put((width >> 8) & 0xff);
    //height
    str.put(height & 0xff);
    str.put((height >> 8) & 0xff);


    uint8_t ct_size= 0;
    if(ct){
        ct_size = ct->log_colors() - 1;
    }

    str.put((ct!=NULL)*0x80 + //ColorTable 
            flag_interlaced * 0x40 + //Interlace 
            (ct!=NULL && ct->is_sorted()) * 0x20 + //Sorted
            (ct_size&0x7)); //ColorTable size (3 bytes)

    //local color table
    if(ct){
        len += ct->write(str);
    }

    //minimum code size
    str.put(active_ct->log_colors());
    //Image Data
    LZW writer(str, active_ct->log_colors());
    len += writer.write(data, width*height);
    len += writer.flush();

    //End image block
    str.put(0);

    bytes = len;
    return len;
};

void GIFImage::write_ppm(const char *fname, pcGIFColorTable global_ct) const
{
    pcGIFColorTable the_ct = ct;
    if(ct == NULL)
        the_ct = global_ct;
    if(the_ct == NULL)
        return;
    std::ofstream o(fname);
    o << "P6 " << get_width() << " " << get_height() << " 255\n";
    for(int y = 0; y < get_height(); y++)
    {
        for(int x = 0; x < get_width(); x++)
        {
            o.write((const char*)the_ct->get_index(get_value(x,y)), 3);
        }
    }
};

std::string GIFImage::as_string() const {
    std::stringstream ss;
    ss << left << "," << top << "+" << width << "x" << height;
    if(bytes > 0)
        ss << " (" << humanize(bytes) << ")";
    return ss.str();
};

GIF::GIF(uint16_t _width, 
         uint16_t _height,
         pGIFColorTable _global_color_table,
         uint16_t _loop_count,
         uint8_t _background_color_index,
         uint8_t _pixel_aspect_ratio):
    width(_width),
    height(_height),
    global_ct(_global_color_table),
    loop_count(_loop_count),
    bg_color_index(_background_color_index),
    par(_pixel_aspect_ratio)
{};

GIF::~GIF() {};

pGIF GIF::create(uint16_t _width, 
         uint16_t _height,
         pGIFColorTable _global_color_table,
         uint16_t _loop_count,
         uint8_t _background_color_index,
         uint8_t _pixel_aspect_ratio)
{
    return pGIF(new GIF(_width, _height, _global_color_table,
                _loop_count, _background_color_index,
                _pixel_aspect_ratio));
};

int GIF::save(const std::string fname) const
{
    std::ofstream of(fname);
    int len = write(of);
    return len;
};

int GIF::write(std::ostream& out) const
{
    int len = 14;
    //header
    out.write("GIF89a", 6);

    //Logical Screen Descriptor
    out.put(width & 0xff);
    out.put((width>>8) & 0xff);
    out.put(height & 0xff);
    out.put((height>>8) & 0xff);

    //packed fields
    out.put(
        (global_ct ? 0x80 : 0) + 
        ((global_ct ? (global_ct->get_depth()-1)&0x07 : 0)<<4) + 
        ((global_ct ? global_ct->is_sorted() : false) * 0x08) + 
        ((global_ct ? global_ct->log_colors()-1 : 0) & 0x07));

    //background color index
    out.put(bg_color_index);
    //pixel aspect ratio
    out.put(par);

    if(global_ct){
        len += global_ct->write(out);
    }

    //NETSCAPE Animation Extension
    bool have_anim = false;
    for(const_iterator i = begin(); i != end(); i++){
        if((*i)->get_delay_time() > 0)
            have_anim = true;
    };
    if(have_anim){
        len += write_animation_hdr(out);
    }

    //write images
    for(const_iterator i = begin(); i != end(); i++){
        len += (*i)->write(out, global_ct);
    };

    //Trailer
    out.put(0x3B);

    return len;
}

int GIF::write_animation_hdr(std::ostream& out) const{
    //Extension Label
    out.put(0x21);
    //Application Extension Label
    out.put(0xff);
    //block size
    out.put(0x0B);
    //App ident + Auth Code
    out << "NETSCAPE2.0";
    //sub block size
    out.put(0x03);
    //sub-block ID
    out.put(0x01);
    //loop count
    out.put(loop_count & 0xff);
    out.put((loop_count >> 8) & 0xff);
    //terminator
    out.put(0x00);
    return 19;
};

std::string GIF::as_string() const {
    std::stringstream ss;
    ss << "GIF " << width << "x" << height << " " << size() << "frames";

    int i = 0;
    for(auto im : *this){
        ss << "\n\t" << ++i << " " << im->as_string();
    }

    return ss.str();
};
