#ifndef MUSIQUE_MUSICDISPLAYCONSTANTS_H
#define MUSIQUE_MUSICDISPLAYCONSTANTS_H

#include "MusicData/System.h"

struct Scaling
{
    float millimeters;
    float tenths;
};

struct MusicDisplayConstants
{
    // all in tenths

    float lineSpacing = 10.0f;
    float tabLineSpacing = 13.33f;

    float pageWidth = 1233.87f;
    float pageHeight = 1596.77f;

    float leftMargin = 80.0f;
    float rightMargin = 80.0f;
    float topMargin = 80.0f;
    float bottomMargin = 80.0f;

    System::SystemLayout systemLayout = System::SystemLayout();

    float staffDistance = 80.0f;

    float stemLineWidth = 1.25f;
    float beamLineWidth = 5.0f;
    float staffLineWidth = 0.8333f;
    float lightBarLineWidth = 2.0833f;
    float heavyBarLineWidth = 6.6667f;
    float legerLineWidth = 1.25f;
    float endingLineWidth = 0.7682f;
    float wedgeLineWidth = 0.957f;
    float enclosureLineWidth = 1.6667f;

    float graceNoteSize = 66.0f;
    float cueNoteSize = 66.0f;

    float hyphenDistance = 60.0f;
    float beamDistance = 7.5f;

    float clefChangeScale = 0.75f; // clef changes are commonly drawn 2/3rds the size of normal clefs (according to SMuFL)

    Scaling scaling;

};

#endif // MUSIQUE_MUSICDISPLAYCONSTANTS_H