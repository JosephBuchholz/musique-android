#include "KeySignature.h"

void KeySignature::Render(RenderData& renderData, bool showKeySignature, float positionX, float lineSpacing, int lines, const Clef& clef, float offsetX, float offsetY) const
{
    if (showKeySignature && fifths != 0 && print && clef.sign != "TAB") {
        float positionY = (lineSpacing * float(lines - 1)) + offsetY; // the bottom line + instYPosition

        int octaveOffset = 0;
        if (clef.sign == "F")
            octaveOffset = -2;

        if (fifths > 0) // sharps
        {
            for (int n = 1; n <= fifths; n++) // loop through notes in 5ths ending at C#
            {
                float y = 0;
                switch (n)
                {
                    case 1: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::F, 5 + octaveOffset, 0.0f), clef); break;
                    case 2: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::C, 5 + octaveOffset, 0.0f), clef); break;
                    case 3: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::G, 5 + octaveOffset, 0.0f), clef); break;
                    case 4: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::D, 5 + octaveOffset, 0.0f), clef); break;
                    case 5: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::A, 4 + octaveOffset, 0.0f), clef); break;
                    case 6: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::E, 5 + octaveOffset, 0.0f), clef); break;
                    case 7: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::B, 4 + octaveOffset, 0.0f), clef); break;
                    default: break;
                }

                positionY = (lineSpacing * y) + offsetY;
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalSharp, { positionX + offsetX, positionY }, Paint(0xff000000)));
                positionX += 10.0f;
            }
        }
        else // flats
        {
            for (int n = -1; n >= fifths; n--) // loop through notes in descending 5ths ending at Cb
            {
                float y = 0.0f;
                switch (n)
                {
                    case -1: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::B, 4 + octaveOffset, 0.0f), clef); break;
                    case -2: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::E, 5 + octaveOffset, 0.0f), clef); break;
                    case -3: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::A, 4 + octaveOffset, 0.0f), clef); break;
                    case -4: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::D, 5 + octaveOffset, 0.0f), clef); break;
                    case -5: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::G, 4 + octaveOffset, 0.0f), clef); break;
                    case -6: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::C, 5 + octaveOffset, 0.0f), clef); break;
                    case -7: y = Pitch::GetPitchPositionY(Pitch(DiatonicNote::F, 4 + octaveOffset, 0.0f), clef); break;
                    default: break;
                }

                positionY = (lineSpacing * y) + offsetY;
                renderData.AddGlyph(SMuFLGlyph(SMuFLID::accidentalFlat, { positionX + offsetX, positionY }, Paint(0xff000000)));
                positionX += 8.5f;
            }
        }

        // TODO: add naturals
    }
}

void KeySignature::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants)
{

}