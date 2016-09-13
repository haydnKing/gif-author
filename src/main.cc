#include "gifauthor.h"

int main (int argc, char *argv[])
{
    auto ga = GIFAuthor::create();
    return ga->run(argc, argv);
}
