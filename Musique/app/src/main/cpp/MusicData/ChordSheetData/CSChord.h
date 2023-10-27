#ifndef MUSIQUE_CSCHORD_H
#define MUSIQUE_CSCHORD_H

#include <string>
#include "../../RenderData/RenderData.h"
#include "../BaseElements/VisibleElement.h"
#include "../Chords/Chord.h"

class CSChord : public VisibleElement
{
public:

    void Render(RenderData& renderData, const Settings& settings, Vec2<float> measurePosition) const;

public:

    Chord chordSymbol;

    float duration = 1.0f;

    float beatPosition = 0.0f;
    float beatPositionInSong = 0.0f;
};

#endif //MUSIQUE_CSCHORD_H
