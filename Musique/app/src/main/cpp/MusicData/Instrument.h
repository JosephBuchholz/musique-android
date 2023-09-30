#ifndef MUSIQUE_INSTRUMENT_H
#define MUSIQUE_INSTRUMENT_H

#include "Staff.h"
#include <vector>
#include <string>
#include "String.h"
#include "MidiInstrument.h"
#include "Notes/Slur.h"

#include "InstrumentBracket.h"
#include "../TranspositionRequest.h"

class Instrument
{

public:
    // gets the max height of the stave's measures from start to end
    float GetMiddleHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetMiddleHeight(const MusicDisplayConstants& displayConstants, int systemIndex);
    float GetAboveHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetBelowHeight(float lineSpacing, float tabLineSpacing, int start, int end);
    float GetTotalHeight(float lineSpacing, float tabLineSpacing, int start, int end);

    BoundingBox GetTotalBoundingBox(const MusicDisplayConstants& displayConstants, int systemIndex) const;

    void CalculateTotalBoundingBoxes(const MusicDisplayConstants& displayConstants, const std::vector<std::shared_ptr<System>>& systems);

    void CalculateSystemPositionData(const MusicDisplayConstants& displayConstants, int systemIndex, int startMeasureIndex, int endMeasureIndex, bool firstInst, Vec2<float>& previousInstPosition, float& previousInstBelowAndMiddleHeight);

    /**
     * Gets the number of measures
     *
     * @return the number of measures
     */
    int GetMeasureCount();

    void Transpose(const TranspositionRequest& transposeRequest);

public:
    std::string id = "";

    String name = String("Instrument");
    String nameAbbreviation = String();

    MidiInstrument midiInstrument = MidiInstrument();

    std::vector<std::shared_ptr<Staff>> staves;

    //std::vector<std::shared_ptr<Slur>> slurs; // slurs that are contained in this part (including cross system slurs and cross staff slurs)

    std::shared_ptr<InstrumentBracket> instrumentBracket;

    //std::vector<BoundingBox> instrumentBoundingBoxes;
    std::vector<Vec2<float>> systemPositionData; // the position for each system
    std::vector<BoundingBox> systemBoundingBoxes; // the bounding boxes for each system
};

#endif // MUSIQUE_INSTRUMENT_H