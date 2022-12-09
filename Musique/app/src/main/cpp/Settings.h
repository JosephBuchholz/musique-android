#pragma once

class Settings
{
public:
    enum class MusicLayout
    {
        None = 0, Horizontal, Vertical, Paged
    } musicLayout = MusicLayout::Vertical;
};