#ifndef MUSIQUE_TRANSPOSE_H
#define MUSIQUE_TRANSPOSE_H

#include <string>

struct Transpose
{
    int diatonic = 0;
    int chromatic = 0;
    int octaveChange = 0;
};

#endif // MUSIQUE_TRANSPOSE_H