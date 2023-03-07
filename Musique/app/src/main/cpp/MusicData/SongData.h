#ifndef MUSIQUE_SONGDATA_H
#define MUSIQUE_SONGDATA_H

#include "InstrumentInfo.h"
#include <vector>
#include <string>

class SongData
{
public:
    std::string songTitle = "";

    int workNumber = -1; // -1 is an invalid value

    std::string movementNumber = "";
    std::string movementTitle = "";

    std::vector<InstrumentInfo> instrumentInfos;

private:

};

#endif // MUSIQUE_SONGDATA_H