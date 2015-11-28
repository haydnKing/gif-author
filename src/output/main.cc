/*
 * Test driver for gif.h
 */

#include "gif.h"
#include <iostream>
#include <fstream>

GIFColorTable* get_bw_ct();

GIFImage* get_test_image(int w, int h, bool alt=false);

int main(){
    int w = 83;
    int h = w;
    GIF test(w,h, get_bw_ct());
    test.push_back(*get_test_image(w,h));
    test.push_back(*get_test_image(w,h,true));

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
    
GIFImage* get_test_image(int w, int h, bool alt) {
    GIFImage *im = new GIFImage(0,0,w,h,50);
    int c = 1;
    if(alt)
    {
        im->clear_to(1);
        c = 0;
    }

    for(int x=0; x<w; x++){
        for(int y=0; y < h; y++){
            if((x>y && (h-y)>x) || (y>x && (h-y)<x)) {
                if(y%2==0)
                    im->set_value(x,y,c);
            }
            else {
                if(x%2==0)
                    im->set_value(x,y,c);
            }
            
        }
    }
    return im;
};
    
