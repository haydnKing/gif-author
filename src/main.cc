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
    std::cout << "parsing..." << std::endl;

    std::vector<string> args;
    for(int i = 1; i < argc; i++)
    {
        args.push_back(argv[i]);
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }
    std::vector<string> uk = og->parse(args);

    if(uk.size() > 0)
    {
        for(auto it : uk)
        {
            std::cout << "Unknown argument: " << it << std::endl;
        }
    }

    return 0;
}
