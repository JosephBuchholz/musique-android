#ifndef MUSIQUE_SETTINGS_H
#define MUSIQUE_SETTINGS_H

#include "MusicDisplayConstants.h"

class Settings
{
public:
    enum class MusicLayout
    {
        None = 0, Horizontal, Vertical, Paged
    } musicLayout = MusicLayout::Paged;

    Orientation pagesOrientation = Orientation::Horizontal;
};

#endif // MUSIQUE_SETTINGS_H