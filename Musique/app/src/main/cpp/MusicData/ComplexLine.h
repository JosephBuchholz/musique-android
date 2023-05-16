#ifndef MUSIQUE_COMPLEXLINE_H
#define MUSIQUE_COMPLEXLINE_H

#include "BaseElements/VisibleElement.h"

class ComplexLine : public VisibleElement
{
public:

    enum class ComplexLineType {
        None = 0, WavyTrill, WavyVibrato, WavyVibratoWide, WavyGuitarVibrato, WavyGuitarWideVibrato, Wavy, Sawtooth, Squaretooth, Circular, AccelRitBeam, Random, ArpeggioUp, ArpeggioDown, WavyGliss
    };

public:
    static float ConstLineFunction(int glyphIndex);
    static float RandomLineFunction(int glyphIndex);
    static float FasterLineFunction(int glyphIndex);
    static float SlowerLineFunction(int glyphIndex);

    ComplexLine() {}
    ComplexLine(ComplexLineType lineType, float (*lineFunction) (int glyphIndex) = ComplexLine::ConstLineFunction);

    void Render(RenderData& renderData, Vec2<float> parentPosition, Vec2<float> offset = { 0.0f, 0.0f }) const;

public:

    ComplexLineType complexLineType = ComplexLineType::WavyTrill;

    float (*lineFunction) (int glyphIndex) = ComplexLine::ConstLineFunction;

    SMuFLID startGlyph = SMuFLID::None;
    SMuFLID endGlyph = SMuFLID::None;

    // relative to the parent position
    Vec2<float> positionStart = { 0.0f, 0.0f };
    Vec2<float> positionEnd = { 0.0f, 0.0f };

private:

    std::vector<SMuFLID> GetLineSegmentsVector() const;

    static std::vector<SMuFLID> wavyTrillLineSegments;
    static std::vector<SMuFLID> wavyVibratoLineSegments;
    static std::vector<SMuFLID> wavyVibratoWideLineSegments;
    static std::vector<SMuFLID> wavyGuitarVibratoLineSegments;
    static std::vector<SMuFLID> wavyGuitarWideVibratoLineSegments;
    static std::vector<SMuFLID> wavyLineSegments;
    static std::vector<SMuFLID> sawtoothLineSegments;
    static std::vector<SMuFLID> squaretoothLineSegments;
    static std::vector<SMuFLID> circularLineSegments;
    static std::vector<SMuFLID> accelRitBeamLineSegments;
    static std::vector<SMuFLID> randomLineSegments;
    static std::vector<SMuFLID> arpeggiatoUpLineSegments;
    static std::vector<SMuFLID> arpeggiatoDownLineSegments;
    static std::vector<SMuFLID> wavyGlissLineSegments;
};

#endif //MUSIQUE_COMPLEXLINE_H
