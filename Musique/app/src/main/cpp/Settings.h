#ifndef MUSIQUE_SETTINGS_H
#define MUSIQUE_SETTINGS_H

#include "MusicDisplayConstants.h"

class Settings
{
public:
    enum class MusicLayout
    {
        None = 0, Horizontal, Vertical, Paged
    };

    enum class ChordSymbolStyle
    {
        None = 0, Auto, Normal, Jazzy
    };

    enum class ShowChordDiagram
    {
        None = 0, Auto, Never
    };

public:

    MusicLayout musicLayout = MusicLayout::Paged;
    Orientation pagesOrientation = Orientation::Horizontal;

    ChordSymbolStyle chordSymbolStyle = ChordSymbolStyle::Auto;
    ShowChordDiagram showChordDiagram = ShowChordDiagram::Auto;
};

#endif // MUSIQUE_SETTINGS_H