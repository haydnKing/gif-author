#include "out.h"

string humanize(int bytes){
    char pre[] = "kMGTPE";
    int exponent = floor(log(bytes)/log(1024));
    stringstream ss;
    if(exponent == 0)
    {
        ss << bytes << " bytes";
    }
    else
    {
        ss << fixed << setprecision(2) << bytes / pow(1024, exponent) << pre[exponent-1] << "B";
    }

    return ss.str();
};
