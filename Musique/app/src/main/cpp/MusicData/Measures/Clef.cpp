#include "Clef.h"

void Clef::Render(RenderData& renderData, bool showClef, float positionX, const MusicDisplayConstants& displayConstants, int lines, float offsetX, float offsetY) const
{
    // clef
    if (showClef)
    {
        float positionY = GetClefLinePositionY(displayConstants, lines) + offsetY; // the bottom line + instYPosition

        SMuFLGlyph glyph = SMuFLGlyph(GetClefSMuFLID(*this, lines), positionX + offsetX, positionY, Paint(0xff000000));
        renderData.AddGlyph(glyph);
    }
}

// from the bottom staff line
float Clef::GetClefLinePositionY(const MusicDisplayConstants& displayConstants, int lines) const
{
    float clefYPosition = 0.0f;
    float fromTopLine = 0.0f;
    int spaces = (lines - 1);

    if (sign == "G" || sign == "F" || sign == "C") {
        clefYPosition = (float)(line - 1) * displayConstants.lineSpacing;
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }
    else if (sign == "TAB")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.tabLineSpacing;
        fromTopLine = (spaces*displayConstants.tabLineSpacing) - clefYPosition;
    }
    else if (sign == "percussion")
    {
        clefYPosition = ((float)spaces/2.0f) * displayConstants.lineSpacing;
        fromTopLine = (spaces*displayConstants.lineSpacing) - clefYPosition;
    }

    return fromTopLine;
}

SMuFLID Clef::GetClefSMuFLID(const Clef& clef, int staffLines)
{
    if (clef.sign == "G") {
        return SMuFLID::GClef;
    } else if (clef.sign == "F") {
        return SMuFLID::FClef;
    } else if (clef.sign == "C") {
        return SMuFLID::CClef;
    } else if (clef.sign == "TAB") {
        if (staffLines == 6)
            return SMuFLID::StringTabClef6;
        else
            return SMuFLID::StringTabClef4;
    } else if (clef.sign == "percussion") {
        return SMuFLID::unpitchedPercussionClef1;
    }

    return SMuFLID::None;
}