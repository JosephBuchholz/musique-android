#ifndef MUSIQUE_SYSTEM_H
#define MUSIQUE_SYSTEM_H

#include <unordered_map>

#include "../Collisions/Vec2.h"
#include "SystemMeasure.h"

/**
 * A class that holds data for a system on a page of sheet music
 */
class System
{
public:
    struct SystemLayout {
        float systemDistance = 127.0f;
        float topSystemDistance = 127.0f;

        float systemLeftMargin = 0.0f;
        float systemRightMargin = 0.0f;
    };

public:
    SystemLayout layout = SystemLayout();

    bool showBeginningTimeSignature = false; // whether to show time signature at the beginning of the system
    //float timeSignaturePositionX = 0.0f; // relative to the start of the start of the system/start of first measure

    bool showBeginningClef = true; // whether to show clef at the beginning of the system
    //float clefPositionX = 0.0f; // relative to the start of the start of the system/start of first measure

    bool showBeginningKeySignature = true; // whether to show key signature at the beginning of the system
    //float keySignaturePositionX = 0.0f; // relative to the start of the start of the system/start of first measure

    int beginningMeasureIndex = 0; // the index of the first measure in this system
    int endingMeasureIndex = 0; // the index of the last measure in this system

    struct SystemMeasureData
    {
        float timeSignaturePositionX = 0.0f;
        float clefPositionX = 0.0f;
        float keySignaturePositionX = 0.0f;

        float measureWidth = 0.0f;
        float measureBeginningWidth = 0.0f; // the width of the beginning part of the measure (the clef + time signature + key signature (basically up until the first note starts))

        float repeatBarlinePositionX = 0.0f; // the position of the repeat barline at the beginning of the measure.
    };

    // key: the measure index | value: the system wide data for the measure
    std::unordered_map<int, SystemMeasureData> systemMeasureData;

    std::vector<SystemMeasure> systemMeasures;

    Vec2<float> position = { 0.0f, 0.0f }; // relative to the page
};

#endif // MUSIQUE_SYSTEM_H