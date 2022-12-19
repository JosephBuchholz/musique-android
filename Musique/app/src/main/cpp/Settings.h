#pragma once

#include "MusicDisplayConstants.h"

class Settings
{
public:
    enum class MusicLayout
    {
        None = 0, Horizontal, Vertical, Paged
    } musicLayout = MusicLayout::Vertical;

    MusicDisplayConstants displayConstants;
};