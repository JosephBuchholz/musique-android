#ifndef MUSIQUE_MUSICDISPLAYCONSTANTS_H
#define MUSIQUE_MUSICDISPLAYCONSTANTS_H

#include "MusicData/System.h"

#define DEFAULT_STAFF_LINE_WIDTH 0.8333f

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
    float staffLineWidth = DEFAULT_STAFF_LINE_WIDTH;
    float lightBarLineWidth = 2.0833f;
    float heavyBarLineWidth = 6.6667f;
    float legerLineWidth = 1.25f;
    float endingLineWidth = 0.7682f;
    float wedgeLineWidth = 0.957f;
    float enclosureLineWidth = 1.6667f;

    // percentage of the regular size
    float graceNoteSize = 0.66f;
    float cueNoteSize = 0.66f;

    float hyphenDistance = 60.0f;
    float beamDistance = 7.5f;

    float clefChangeScale = 0.75f; // clef changes are commonly drawn 2/3rds the size of normal clefs (according to SMuFL)

    Scaling scaling;

    float ledgerLineMargin = 3.5f;

    // notes stems
    float minNoteStemHeight = 15.0f; // the minimum height for a note stem (not including the beam height)

    // beams
    float maxBeamSlope = 0.333f; // the maximum (abs value) slope that a beam can have
};

#endif // MUSIQUE_MUSICDISPLAYCONSTANTS_H