/*
 * Test driver for gif.h
 */

#include "gif.h"
#include <iostream>
#include <fstream>

GIFColorTable* get_bw_ct();

GIFImage* get_test_image();

int main(){
    GIF test(3,5, get_bw_ct());
    test.push_back(*get_test_image());

    std::ofstream ofile("test.gif");
    test.write(ofile);
    ofile.close();

    std::cout << "Mark" << std::endl;

    return 0;
};

GIFColorTable* get_bw_ct(){
    GIFColorTable* ct = new GIFColorTable();
    ct->push_color(RGBColor(0,0,0));
    ct->push_color(RGBColor(255,255,255));
    ct->push_color(RGBColor(128,128,128));
    ct->push_color(RGBColor(255,255,255));
    return ct;
};
    
GIFImage* get_test_image() {
    uint8_t* data = new uint8_t[3*5];
    data[ 0] = 0;
    data[ 1] = 1;
    data[ 2] = 1;
    data[ 3] = 1;
    data[ 4] = 0;
    data[ 5] = 1;
    data[ 6] = 1;
    data[ 7] = 1;
    data[ 8] = 1;
    data[ 9] = 1;
    data[10] = 1;
    data[11] = 1;
    data[12] = 1;
    data[13] = 1;
    data[14] = 1;
    GIFImage *i = new GIFImage(0,0,3,5,data);
    return i;
};
    
