#include "ComplexLine.h"

#include "../RenderMeasurement.h"
#include "../Utils/Math.h"

std::vector<SMuFLID> ComplexLine::wavyTrillLineSegments = {
        SMuFLID::wiggleTrillFastest,
        SMuFLID::wiggleTrillFasterStill,
        SMuFLID::wiggleTrillFaster,
        SMuFLID::wiggleTrillFast,
        SMuFLID::wiggleTrill,
        SMuFLID::wiggleTrillSlow,
        SMuFLID::wiggleTrillSlower,
        SMuFLID::wiggleTrillSlowerStill,
        SMuFLID::wiggleTrillSlowest
};

std::vector<SMuFLID> ComplexLine::wavyVibratoLineSegments = {
        SMuFLID::wiggleVibrato
};

std::vector<SMuFLID> ComplexLine::wavyVibratoWideLineSegments = {
        SMuFLID::wiggleVibratoWide
};

std::vector<SMuFLID> ComplexLine::wavyGuitarVibratoLineSegments = {
        SMuFLID::guitarVibratoStroke
};

std::vector<SMuFLID> ComplexLine::wavyGuitarWideVibratoLineSegments = {
        SMuFLID::guitarWideVibratoStroke
};

std::vector<SMuFLID> ComplexLine::wavyLineSegments = {
        SMuFLID::wiggleWavyNarrow,
        SMuFLID::wiggleWavy,
        SMuFLID::wiggleWavyWide
};

std::vector<SMuFLID> ComplexLine::sawtoothLineSegments = {
        SMuFLID::wiggleSawtoothNarrow,
        SMuFLID::wiggleSawtooth,
        SMuFLID::wiggleSawtoothWide
};

std::vector<SMuFLID> ComplexLine::squaretoothLineSegments = {
        SMuFLID::wiggleSquareWaveNarrow,
        SMuFLID::wiggleSquareWave,
        SMuFLID::wiggleSquareWaveWide
};

std::vector<SMuFLID> ComplexLine::circularLineSegments = {
        SMuFLID::wiggleCircularLargest,
        SMuFLID::wiggleCircularLargerStill,
        SMuFLID::wiggleCircularLarger,
        SMuFLID::wiggleCircularLarge,
        SMuFLID::wiggleCircular,
        SMuFLID::wiggleCircularSmall,
        SMuFLID::wiggleCircularSmall,
        SMuFLID::wiggleCircularEnd,
        SMuFLID::wiggleCircularEnd
};

std::vector<SMuFLID> ComplexLine::accelRitBeamLineSegments = {
        SMuFLID::beamAccelRit1,
        SMuFLID::beamAccelRit2,
        SMuFLID::beamAccelRit3,
        SMuFLID::beamAccelRit4,
        SMuFLID::beamAccelRit5,
        SMuFLID::beamAccelRit6,
        SMuFLID::beamAccelRit7,
        SMuFLID::beamAccelRit8,
        SMuFLID::beamAccelRit9,
        SMuFLID::beamAccelRit10,
        SMuFLID::beamAccelRit11,
        SMuFLID::beamAccelRit12,
        SMuFLID::beamAccelRit13,
        SMuFLID::beamAccelRit14,
        SMuFLID::beamAccelRit15,
        SMuFLID::beamAccelRitFinal
};

std::vector<SMuFLID> ComplexLine::randomLineSegments = {
        SMuFLID::wiggleRandom1,
        SMuFLID::wiggleRandom2,
        SMuFLID::wiggleRandom3,
        SMuFLID::wiggleRandom4
};

std::vector<SMuFLID> ComplexLine::arpeggiatoUpLineSegments = {
        SMuFLID::wiggleArpeggiatoUp
};

std::vector<SMuFLID> ComplexLine::arpeggiatoDownLineSegments = {
        SMuFLID::wiggleArpeggiatoDown
};

std::vector<SMuFLID> ComplexLine::wavyGlissLineSegments = {
        SMuFLID::wiggleGlissando
};

float ComplexLine::ConstLineFunction(int glyphIndex)
{
    return 0.0f;
}

// see: https://stackoverflow.com/questions/686353/random-float-number-generation
float ComplexLine::RandomLineFunction(int glyphIndex)
{
    return (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/2))) - 1.0f;
}

float ComplexLine::FasterLineFunction(int glyphIndex)
{
    return (-1.0f + (glyphIndex * (2.0f / 10)));
}

float ComplexLine::SlowerLineFunction(int glyphIndex)
{
    return -(-1.0f + (glyphIndex * (2.0f / 10)));
}

ComplexLine::ComplexLine(ComplexLineType lineType, float (*lineFunction) (int glyphIndex))
        : complexLineType(lineType), lineFunction(lineFunction) {}

void ComplexLine::Render(RenderData& renderData, Vec2<float> parentPosition, Vec2<float> offset) const
{
    Vec2<float> pos = { positionStart.x + parentPosition.x + offset.x, positionStart.y + parentPosition.y + offset.y };

    Paint paint;
    paint.rotateDegrees = 0.0f;

    std::vector<SMuFLID> glyphs = GetLineSegmentsVector();

    Vec2<float> endPosRelativeToStart = positionEnd - positionStart;

    float angleDegrees = 0.0f;

    if (endPosRelativeToStart.x != 0.0f && endPosRelativeToStart.y != 0.0f)
    {
        float angleRadians = std::atan(endPosRelativeToStart.y / endPosRelativeToStart.x);
        angleDegrees = RadiansToDegrees(angleRadians);

        if (endPosRelativeToStart.x < 0.0f)
            angleDegrees += 180.0f;
    }
    else
    {
        if (endPosRelativeToStart.x > 0.0f)
            angleDegrees = 0.0f;
        else if (endPosRelativeToStart.y > 0.0f)
            angleDegrees = 90.0f;
        else if (endPosRelativeToStart.x < 0.0f)
            angleDegrees = 180.0f;
        else // endPosRelativeToStart.y < 0.0f || (endPosRelativeToStart.x == 0 && endPosRelativeToStart.y == 0)
            angleDegrees = 270.0f;
    }

    angleDegrees = -angleDegrees;

    paint.rotateDegrees = angleDegrees;

    float magnitude = std::sqrt((endPosRelativeToStart.x * endPosRelativeToStart.x) + (endPosRelativeToStart.y * endPosRelativeToStart.y));
    Vec2<float> directionUnitVector = { endPosRelativeToStart.x / magnitude, endPosRelativeToStart.y / magnitude};

    float lineLength = 0.0f;

    if (endGlyph != SMuFLID::None)
    {
        float glyphWidth = RenderMeasurement::MeasureGlyph(endGlyph);
        lineLength += glyphWidth;
    }

    if (startGlyph != SMuFLID::None)
    {
        renderData.AddGlyph(SMuFLGlyph(startGlyph, pos, paint));
        float glyphWidth = RenderMeasurement::MeasureGlyph(startGlyph);
        pos += directionUnitVector * Vec2<float>{ glyphWidth, glyphWidth };
        lineLength += glyphWidth;
    }

    int i = 0;
    while (lineLength < magnitude)
    {
        float intensity = lineFunction(i);

        int halfSize = glyphs.size() / 2;
        int sacled = (int)(intensity * (float)halfSize) + halfSize;
        int glyphIndex = sacled;

        if (glyphIndex >= glyphs.size() || glyphIndex < 0)
            break;

        //LOGD("intensity: %f, glyphIndex: %d", intensity, glyphIndex);

        SMuFLID glyph = glyphs[glyphIndex];
        float glyphWidth = RenderMeasurement::MeasureGlyph(glyphs[glyphIndex]);

        if (lineLength + glyphWidth >= magnitude)
            break;

        renderData.AddGlyph(SMuFLGlyph(glyph, pos, paint));
        pos += directionUnitVector * Vec2<float>{ glyphWidth, glyphWidth };
        lineLength += glyphWidth;

        i++;
    }

    if (endGlyph != SMuFLID::None)
    {
        renderData.AddGlyph(SMuFLGlyph(endGlyph, pos, paint));
    }
}

std::vector<SMuFLID> ComplexLine::GetLineSegmentsVector() const
{
    switch (complexLineType)
    {
        case ComplexLineType::WavyTrill: return wavyTrillLineSegments;
        case ComplexLineType::WavyVibrato: return wavyVibratoLineSegments;
        case ComplexLineType::WavyVibratoWide: return wavyVibratoWideLineSegments;
        case ComplexLineType::WavyGuitarVibrato: return wavyGuitarVibratoLineSegments;
        case ComplexLineType::WavyGuitarWideVibrato: return wavyGuitarWideVibratoLineSegments;
        case ComplexLineType::Wavy: return wavyLineSegments;
        case ComplexLineType::Sawtooth: return sawtoothLineSegments;
        case ComplexLineType::Squaretooth: return squaretoothLineSegments;
        case ComplexLineType::Circular: return circularLineSegments;
        case ComplexLineType::AccelRitBeam: return accelRitBeamLineSegments;
        case ComplexLineType::Random: return randomLineSegments;
        case ComplexLineType::ArpeggioUp: return arpeggiatoUpLineSegments;
        case ComplexLineType::ArpeggioDown: return arpeggiatoDownLineSegments;
        case ComplexLineType::WavyGliss: return wavyGlissLineSegments;
        default: return wavyLineSegments;
    }
}