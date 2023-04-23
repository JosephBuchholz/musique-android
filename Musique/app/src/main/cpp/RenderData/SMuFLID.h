#ifndef MUSIQUE_SMUFLID_H
#define MUSIQUE_SMUFLID_H

#include "../MusicData/Types.h"
#include "../MusicData/Notes/Accidental.h"

/**
 * An enum that maps SMuFL glyph names to SMuFL code points.
 */
enum class SMuFLID : uint16_t
{
    None = 0,
    ErrorGlyph = 0xEE06, // just a random glyph (for debugging)

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

    // articulations

    articAccentAbove = 0xE4A0,
    articAccentBelow = 0xE4A1,
    articStaccatoAbove = 0xE4A2,
    articStaccatoBelow = 0xE4A3,
    articTenutoAbove = 0xE4A4,
    articTenutoBelow = 0xE4A5,
    articStaccatissimoAbove = 0xE4A6,
    articStaccatissimoBelow = 0xE4A7,
    articStaccatissimoWedgeAbove = 0xE4A8,
    articStaccatissimoWedgeBelow = 0xE4A9,
    articStaccatissimoStrokeAbove = 0xE4AA,
    articStaccatissimoStrokeBelow = 0xE4AB,
    articMarcatoAbove = 0xE4AC,
    articMarcatoBelow = 0xE4AD,
    articMarcatoStaccatoAbove = 0xE4AE,
    articMarcatoStaccatoBelow = 0xE4AF,
    articAccentStaccatoAbove = 0xE4B0,
    articAccentStaccatoBelow = 0xE4B1,
    articTenutoStaccatoAbove = 0xE4B2,
    articTenutoStaccatoBelow = 0xE4B3,
    articTenutoAccentAbove = 0xE4B4,
    articTenutoAccentBelow = 0xE4B5,
    articStressAbove = 0xE4B6,
    articStressBelow = 0xE4B7,
    articUnstressAbove = 0xE4B8,
    articUnstressBelow = 0xE4B9,
    articLaissezVibrerAbove = 0xE4BA,
    articLaissezVibrerBelow = 0xE4BB,
    articMarcatoTenutoAbove = 0xE4BC,
    articMarcatoTenutoBelow = 0xE4BD,

    // holds and pauses

    fermataAbove = 0xE4C0,
    fermataBelow = 0xE4C1,
    fermataVeryShortAbove = 0xE4C2,
    fermataVeryShortBelow = 0xE4C3,
    fermataShortAbove = 0xE4C4,
    fermataShortBelow = 0xE4C5,
    fermataLongAbove = 0xE4C6,
    fermataLongBelow = 0xE4C7,
    fermataVeryLongAbove = 0xE4C8,
    fermataVeryLongBelow = 0xE4C9,
    fermataLongHenzeAbove = 0xE4CA,
    fermataLongHenzeBelow = 0xE4CB,
    fermataShortHenzeAbove = 0xE4CC,
    fermataShortHenzeBelow = 0xE4CD,
    breathMarkComma = 0xE4CE,
    breathMarkTick = 0xE4CF,
    breathMarkUpbow = 0xE4D0,
    caesura = 0xE4D1,
    caesuraThick = 0xE4D2,
    caesuraShort = 0xE4D3,
    caesuraCurved = 0xE4D4,
    breathMarkSalzedo = 0xE4D5,
    curlewSign = 0xE4D6,
    caesuraSingleStroke = 0xE4D7,

    // rests
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

    // dynamics
    dynamicPiano = 0xE520,
    dynamicMezzo = 0xE521,
    dynamicForte = 0xE522,
    dynamicRinforzando = 0xE523,
    dynamicSforzando = 0xE524,
    dynamicZ = 0xE525,
    dynamicNiente = 0xE526,
    dynamicPPPPPP = 0xE527,
    dynamicPPPPP = 0xE528,
    dynamicPPPP = 0xE529,
    dynamicPPP = 0xE52A,
    dynamicPP = 0xE52B,
    dynamicMP = 0xE52C,
    dynamicMF = 0xE52D,
    dynamicPF = 0xE52E,
    dynamicFF = 0xE52F,
    dynamicFFF = 0xE530,
    dynamicFFFF = 0xE531,
    dynamicFFFFF = 0xE532,
    dynamicFFFFFF = 0xE533,
    dynamicFortePiano = 0xE534,
    dynamicForzando = 0xE535,
    dynamicSforzando1 = 0xE536,
    dynamicSforzandoPiano = 0xE537,
    dynamicSforzandoPianissimo = 0xE538,
    dynamicSforzato = 0xE539,
    dynamicSforzatoPiano = 0xE53A,
    dynamicSforzatoFF = 0xE53B,
    dynamicRinforzando1 = 0xE53C,
    dynamicRinforzando2 = 0xE53D,
    dynamicCrescendoHairpin = 0xE53E,
    dynamicDiminuendoHairpin = 0xE53F,
    dynamicMessaDiVoce = 0xE540,
    dynamicNienteForHairpin = 0xE541,
    dynamicHairpinParenthesisLeft = 0xE542,
    dynamicHairpinParenthesisRight = 0xE543,
    dynamicHairpinBracketLeft = 0xE544,
    dynamicHairpinBracketRight = 0xE545,
    dynamicCombinedSeparatorColon = 0xE546,
    dynamicCombinedSeparatorHyphen = 0xE547,
    dynamicCombinedSeparatorSpace = 0xE548,
    dynamicCombinedSeparatorSlash = 0xE549,

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
        default: return SMuFLID::noteheadDoubleWhole;
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
        default: return SMuFLID::restDoubleWhole;
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