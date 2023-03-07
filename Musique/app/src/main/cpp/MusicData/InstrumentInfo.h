#ifndef MUSIQUE_INSTRUMENTINFO_H
#define MUSIQUE_INSTRUMENTINFO_H

#include <string>

class InstrumentInfo
{
public:
    std::string name = "";
    int volume = 0;
    bool visible = true;
};

#endif // MUSIQUE_INSTRUMENTINFO_H