#include "gifauthor.h"

int main (int argc, char *argv[])
{
    pGIFAuthor ga = GIFAuthor::create(argc, argv);
    ga->generate();

    return 0;
}
