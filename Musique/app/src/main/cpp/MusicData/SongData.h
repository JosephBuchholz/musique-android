#pragma once
#include "InstrumentInfo.h"
#include <vector>
#include <string>

class SongData
{
public:
    std::string songTitle = "Title";

    int workNumber = -1; // -1 is an invalid value

    std::string movementNumber = "";
    std::string movementTitle = "";

    std::vector<InstrumentInfo> instrumentInfos;

private:

};