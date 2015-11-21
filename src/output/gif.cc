#include "gif.h"


GIFColorTable::GIFColorTable(int _depth,
                             int _colors,
                             bool _sorted, 
                             uint8_t* _data):
    depth(_depth),
    colors(_colors),
    sorted(_sorted)
{
    if(_data==NULL){
        data = new uint8_t[3 * std::pow(2, colors)];
        std::memset(data, 0, 3*std::pow(2,colors));
    }
    else {
        data = _data;
    }
};

GIFColorTable::~GIFColorTable() 
{
    delete data;
};

void GIFColorTable::set_color(int index, uint8_t r, uint8_t g, uint8_t b)
{
    data[index + RED] = r;
    data[index + GREEN] = g;
    data[index + BLUE] = b;
};

int GIFColorTable::write(std::ostringstream& str) const
{
    std.write(reinterpret_cast<char*>data, 3*std::pow(2,colors));

    return 3*std::pow(2,colors);
};

GIFImage::GIFImage(int _left, 
                   int _top,
                   int _width, 
                   int _height, 
                   uint8_t* _data,
                   int _delay_time, 
                   ColorTable* _ct) :
    left(_left),
    top(_top),
    width(_width),
    height(_height),
    data(_data),
    ct(_ct)
{};

GIFImage::~GIFImage(){
    delete data;
    if(ct != NULL){
        delete ct;
    }
};

void GIFImage::write(std::ostream& str, ColorTable* global_ct) const
{
    //Graphic Control Extension
    str.put(0x21);
    srt.put(0xF9);
    str.put(0x04);

    str.put((disposal_method&0x07)<<2 + //disposal method
            flag_user_input*0x02 + //user input
            flag_transparency*0x01);

    //delay time
    str.put(delay_time & 0xff);
    str.put((delay_time >> 8) & 0xff);

    //transparent color index
    if(flag_transparency)
        str.put(t_color_index);
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
        ct_size = ct->pow_colors() - 1;
    }

    std.put((ct!=NULL)*0x80 + //ColorTable 
            flag_interlace * 0x40 + //Interlace 
            flag_sorted * 0x20 + //Sorted
            ct_size&0x7); //ColorTable size (3 bytes)

    //local color table
    if(ct){
        ct->write(str);
    }

    ColorTable* active_ct = ct;
    if(active_ct == NULL){
        active_ct = global_ct;
    }

    //minimum code size
    str.put(active_ct->depth());
    //Image Data
    writer = LZW(str, active_ct->depth());
    for(int i=0; i < width*height; i++)
        writer.write(i);
    writer.flush();

    //End image block
    str.put(0);
};

GIF::GIF(int _width, 
         int _height,
         ColorTable* _global_color_table,
         uint8_t _background_color_index,
         uint8_t _pixel_aspect_ratio):
    width(_width),
    height(_height),
    global_ct(_global_color_table),
    bg_color_index(_background_color_index),
    par(_pixel_aspect_ratio)
{};

GIF::~GIF() {
    if(global_ct){
        delete global_ct;
    }
};

void GIF::write(std::ostream& out) const
{
    //header
    out.write("GIF89a", 6);

    //Logical Screen Descriptor
    out.put(width & 0xff);
    out.put((width>>8) & 0xff);
    out.put(height & 0xff);
    out.put((height>>8) & 0xff);

    //packed fields
    out.put(global_ct!=NULL * 0x80 + 
            (global_ct ? global_ct->depth()*0x07 : 0)<<4 + 
            (global_ct ? global_ct->is_sorted() : false) * 0x80 + 
            (global_ct ? global_ct->pow_colors()-1 : 0) & 0x07);

    //background color index
    out.put(bg_color_index);
    //pixel aspect ratio
    out.put(par);

    if(global_ct){
        global_ct->write(out);
    }

    //NETSCAPE Extension
    //

    //write images
    for(std::list<GIFImage>::iterator i = begin(); i != end; i++){
        i->write(out);
    };

    //Trailer
    out.put(0x3B);

}
