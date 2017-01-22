#include "gifauthor.h"

int main (int argc, char *argv[])
{
    pGIFAuthor ga = GIFAuthor::create(argc, argv);

    ga->run();  

    return 0;
}
