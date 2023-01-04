#pragma once
#include "../MusicData/Clef.h"

enum class SMuFLID : uint32_t
{
    None = 0,
    GClef = 0xE050,
    GClef8vb = 0xE052,
    GClef8va = 0xE053,
    FClef = 0xE062,
    CClef = 0xE05C,
    StringTabClef6 = 0xE06D,
    StringTabClef4 = 0xE06E,
    unpitchedPercussionClef1 = 0xE069,
    noteheadDoubleWhole = 0xE0A0,
    noteheadWhole = 0xE0A2,
    noteheadHalf = 0xE0A3,
    noteheadBlack = 0xE0A4,
    noteheadXBlack = 0xE0A9,

    timeSig0 = 0xE080,
    timeSig1 = 0xE081,
    timeSig2 = 0xE082,
    timeSig3 = 0xE083,
    timeSig4 = 0xE084,
    timeSig5 = 0xE085,
    timeSig6 = 0xE086,
    timeSig7 = 0xE087,
    timeSig8 = 0xE088,
    timeSig9 = 0xE089,
    timeSigCommon = 0xE08A,
    timeSigCutCommon = 0xE08B,

    accidentalFlat = 0xE260,
    accidentalNatural = 0xE261,
    accidentalSharp = 0xE262,
    accidentalDoubleSharp = 0xE263,
    accidentalDoubleFlat = 0xE264,
    accidentalTripleSharp = 0xE265,
    accidentalTripleFlat = 0xE266,
    accidentalNaturalFlat = 0xE267,
    accidentalNaturalSharp = 0xE268,
    accidentalSharpSharp = 0xE269,
    accidentalParensLeft = 0xE26A,
    accidentalParensRight = 0xE26B,
    accidentalBracketLeft = 0xE26C,
    accidentalBracketRight = 0xE26D,

    restMaxima = 0xE4E0,
    restLonga = 0xE4E1,
    restDoubleWhole = 0xE4E2,
    restWhole = 0xE4E3,
    restHalf = 0xE4E4,
    restQuarter = 0xE4E5,
    rest8th = 0xE4E6,
    rest16th = 0xE4E7,
    rest32nd = 0xE4E8,
    rest64th = 0xE4E9,
    rest128th = 0xE4EA,
    rest256th = 0xE4EB,
    rest512th = 0xE4EC,
    rest1024th = 0xE4ED,
    restHBar = 0xE4EE,
    restHBarLeft = 0xE4EF,
    restHBarMiddle = 0xE4F0,
    restHBarRight = 0xE4F1,
    restQuarterOld = 0xE4F2,
    restDoubleWholeLegerLine = 0xE4F3,
    restWholeLegerLine = 0xE4F4,
    restHalfLegerLine = 0xE4F5,
    restQuarterZ = 0xE4F6,

};

static SMuFLID GetClefSMuFLID(Clef clef, int staffLines)
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

static SMuFLID GetNoteHeadSMuFLID(Note::NoteDurationType type)
{
    switch (type)
    {
        case Note::NoteDurationType::None: return SMuFLID::None;
        case Note::NoteDurationType::Whole: return SMuFLID::noteheadWhole;
        case Note::NoteDurationType::Half: return SMuFLID::noteheadHalf;
        case Note::NoteDurationType::Quarter: return SMuFLID::noteheadBlack;
        case Note::NoteDurationType::Eighth: return SMuFLID::noteheadBlack;
        case Note::NoteDurationType::Sixteenth: return SMuFLID::noteheadBlack;
        case Note::NoteDurationType::ThirtySecond: return SMuFLID::noteheadBlack;
    }

    return SMuFLID::None;
}

static SMuFLID GetRestSMuFLID(Note::NoteDurationType type)
{
    switch (type)
    {
        case Note::NoteDurationType::None: return SMuFLID::None;
        case Note::NoteDurationType::Whole: return SMuFLID::restWhole;
        case Note::NoteDurationType::Half: return SMuFLID::restHalf;
        case Note::NoteDurationType::Quarter: return SMuFLID::restQuarter;
        case Note::NoteDurationType::Eighth: return SMuFLID::rest8th;
        case Note::NoteDurationType::Sixteenth: return SMuFLID::rest16th;
        case Note::NoteDurationType::ThirtySecond: return SMuFLID::rest32nd;
    }

    return SMuFLID::None;
}

static SMuFLID GetAccidentalSMuFLID(Accidental::AccidentalType type)
{
    switch (type)
    {
        case Accidental::AccidentalType::None: return SMuFLID::None;
        case Accidental::AccidentalType::Sharp: return SMuFLID::accidentalSharp;
        case Accidental::AccidentalType::Flat: return SMuFLID::accidentalFlat;
        case Accidental::AccidentalType::Natural: return SMuFLID::accidentalNatural;
        case Accidental::AccidentalType::DoubleSharp: return SMuFLID::accidentalDoubleSharp;
        case Accidental::AccidentalType::DoubleFlat: return SMuFLID::accidentalDoubleFlat;
    }

    return SMuFLID::None;
}

static SMuFLID GetTimeSignatureSMuFLID(int time)
{
    switch (time)
    {
        case 0: return SMuFLID::timeSig0;
        case 1: return SMuFLID::timeSig1;
        case 2: return SMuFLID::timeSig2;
        case 3: return SMuFLID::timeSig3;
        case 4: return SMuFLID::timeSig4;
        case 5: return SMuFLID::timeSig5;
        case 6: return SMuFLID::timeSig6;
        case 7: return SMuFLID::timeSig7;
        case 8: return SMuFLID::timeSig8;
        case 9: return SMuFLID::timeSig9;
    }

    return SMuFLID::None;
}