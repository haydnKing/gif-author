/*
 * Test driver for gif.h
 */

#include "gif.h"
#include <iostream>
#include <fstream>

GIFColorTable* get_bw_ct();

GIFImage* get_test_image(int w, int h);

int main(){
    int w = 83;
    int h = w;
    GIF test(w,h, get_bw_ct());
    test.push_back(*get_test_image(w,h));

    std::ofstream ofile("test.gif");
    test.write(ofile);
    ofile.close();

    return 0;
};

GIFColorTable* get_bw_ct(){
    GIFColorTable* ct = new GIFColorTable();
    ct->push_color(RGBColor(0,0,0));
    ct->push_color(RGBColor(255,255,255));
    return ct;
};
    
GIFImage* get_test_image(int w, int h) {
    GIFImage *im = new GIFImage(0,0,w,h);
    im->clear_to(1);
    for(int x=0; x<w; x++){
        for(int y=0; y < h; y++){
            if(x>y)
                im->set_value(x,y,0);
            
        }
    }
    return im;
};
    
