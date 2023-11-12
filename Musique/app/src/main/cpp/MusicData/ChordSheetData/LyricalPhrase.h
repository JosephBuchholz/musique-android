#ifndef MUSIQUE_LYRICALPHRASE_H
#define MUSIQUE_LYRICALPHRASE_H

#include <vector>

#include "CSLyric.h"

class CSLyric;

class LyricalPhrase
{
public:

    std::vector<std::shared_ptr<CSLyric>> lyrics;
};

#endif //MUSIQUE_LYRICALPHRASE_H
