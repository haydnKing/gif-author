//#include "gifauthor.h"
#include <iostream>

#include "util/cline.h"

int main (int argc, char *argv[])
{
    std::cout << "Hello, world" << std::endl;
    pOptionGroup og = OptionGroup::create("mainGroup");

    og->add_option<int>("intoption", "an option that's an integer", 5);
    og->add_option<float>("floatoption", "an option that's a float", 4.0);
    og->add_option("boolopt", "an option that's a bool");

    std::cout << og->help() << std::endl;
    return 0;
}
