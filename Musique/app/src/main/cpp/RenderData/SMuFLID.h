#ifndef MUSIQUE_SMUFLID_H
#define MUSIQUE_SMUFLID_H

#include "../MusicData/Clef.h"
#include "../MusicData/Types.h"
#include "../MusicData/Notes/Accidental.h"

/**
 * An enum that maps SMuFL glyph names to SMuFL code points.
 */
enum class SMuFLID : uint16_t
{
    None = 0,

    repeatLeft = 0xE040,
    repeatRight = 0xE041,
    repeatRightLeft = 0xE042,
    repeatDots = 0xE043,
    repeatDot = 0xE044,
    dalSegno = 0xE045,
    daCapo = 0xE046,
    segno = 0xE047,
    coda = 0xE048,
    codaSquare = 0xE049,
    segnoSerpent1 = 0xE04A,
    segnoSerpent2 = 0xE04B,
    leftRepeatSmall = 0xE04C,
    rightRepeatSmall = 0xE04D,

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

    augmentationDot = 0xE1E7,

    flag8thUp = 0xE240,
    flag8thDown = 0xE241,
    flag16thUp = 0xE242,
    flag16thDown = 0xE243,
    flag32ndUp = 0xE244,
    flag32ndDown = 0xE245,
    flag64thUp = 0xE246,
    flag64thDown = 0xE247,
    flag128thUp = 0xE248,
    flag128thDown = 0xE249,
    flag256thUp = 0xE24A,
    flag256thDown = 0xE24B,
    flag512thUp = 0xE24C,
    flag512thDown = 0xE24D,
    flag1024thUp = 0xE24E,
    flag1024thDown = 0xE24F,
    flagInternalUp = 0xE250,
    flagInternalDown = 0xE251,

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

    metNoteDoubleWhole = 0xECA0,
    metNoteDoubleWholeSquare = 0xECA1,
    metNoteWhole = 0xECA2,
    metNoteHalfUp = 0xECA3,
    metNoteHalfDown = 0xECA4,
    metNoteQuarterUp = 0xECA5,
    metNoteQuarterDown = 0xECA6,
    metNote8thUp = 0xECA7,
    metNote8thDown = 0xECA8,
    metNote16thUp = 0xECA9,
    metNote16thDown = 0xECAA,
    metNote32ndUp = 0xECAB,
    metNote32ndDown = 0xECAC,
    metNote64thUp = 0xECAD,
    metNote64thDown = 0xECAE,
    metNote128thUp = 0xECAF,
    metNote128thDown = 0xECB0,
    metNote256thUp = 0xECB1,
    metNote256thDown = 0xECB2,
    metNote512thUp = 0xECB3,
    metNote512thDown = 0xECB4,
    metNote1024thUp = 0xECB5,
    metNote1024thDown = 0xECB6,
    metAugmentationDot = 0xECB7
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

static SMuFLID GetNoteHeadSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return SMuFLID::None;
        case NoteValue::Whole: return SMuFLID::noteheadWhole;
        case NoteValue::Half: return SMuFLID::noteheadHalf;
        case NoteValue::Quarter: return SMuFLID::noteheadBlack;
        case NoteValue::Eighth: return SMuFLID::noteheadBlack;
        case NoteValue::Sixteenth: return SMuFLID::noteheadBlack;
        case NoteValue::ThirtySecond: return SMuFLID::noteheadBlack;
    }

    return SMuFLID::None;
}

static SMuFLID GetMetronomeNoteSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::Breve: return SMuFLID::metNoteDoubleWhole;
        case NoteValue::Whole: return SMuFLID::metNoteWhole;
        case NoteValue::Half: return SMuFLID::metNoteHalfUp;
        case NoteValue::Quarter: return SMuFLID::metNoteQuarterUp;
        case NoteValue::Eighth: return SMuFLID::metNote8thUp;
        case NoteValue::Sixteenth: return SMuFLID::metNote16thUp;
        case NoteValue::ThirtySecond: return SMuFLID::metNote32ndUp;
        case NoteValue::_64th: return SMuFLID::metNote64thUp;
        case NoteValue::_128th: return SMuFLID::metNote128thUp;
        case NoteValue::_256th: return SMuFLID::metNote256thUp;
        case NoteValue::_512th: return SMuFLID::metNote512thUp;
        case NoteValue::_1024th: return SMuFLID::metNote1024thUp;

        default: return SMuFLID::None;
    }
}

static SMuFLID GetRestSMuFLID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return SMuFLID::None;
        case NoteValue::Whole: return SMuFLID::restWhole;
        case NoteValue::Half: return SMuFLID::restHalf;
        case NoteValue::Quarter: return SMuFLID::restQuarter;
        case NoteValue::Eighth: return SMuFLID::rest8th;
        case NoteValue::Sixteenth: return SMuFLID::rest16th;
        case NoteValue::ThirtySecond: return SMuFLID::rest32nd;
    }

    return SMuFLID::None;
}

static SMuFLID GetNoteFlagSMuFLID(NoteValue type, bool isUp)
{
    switch (type)
    {
        case NoteValue::Eighth: if (isUp) return SMuFLID::flag8thUp; else return SMuFLID::flag8thDown;
        case NoteValue::Sixteenth: if (isUp) return SMuFLID::flag16thUp; else return SMuFLID::flag16thDown;
        case NoteValue::ThirtySecond: if (isUp) return SMuFLID::flag32ndUp; else return SMuFLID::flag32ndDown;
        default: return SMuFLID::None;
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

#endif // MUSIQUE_SMUFLID_H