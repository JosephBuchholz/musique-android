#ifndef MUSIQUE_ASSETID_H
#define MUSIQUE_ASSETID_H

#include "MusicData/Types.h"
//#include "MusicData/Measures/Clef.h"
#include "MusicData/Notes/Accidental.h"

enum class AssetID {
    None = 0,
    WholeNote,
    HalfNote,
    QuarterNote,
    EightNote,
    SixteenthNote,
    Time1,
    Time2,
    Time3,
    Time4,
    Time5,
    Time6,
    Time7,
    Time8,
    Time9,
    Time0,
    TrebleClef,
    BassClef,
    AltoClef,
    TABClef,
    PercussionClef,
    QuarterNoteNoteHead,
    HalfNoteNoteHead,
    XNoteHead,
    Sharp,
    Flat,
    Natural,
    DoubleSharp,
    DoubleFlat,
    WholeRest,
    HalfRest,
    QuarterRest,
    EightRest,
    SixteenthRest,
    ThirtySecondRest,
    KeySignature1Sharp,
    KeySignature2Sharps,
    KeySignature3Sharps,
    KeySignature4Sharps,
    KeySignature5Sharps,
    KeySignature6Sharps,
    KeySignature7Sharps,
    KeySignature1Flat,
    KeySignature2Flats,
    KeySignature3Flats,
    KeySignature4Flats,
    KeySignature5Flats,
    KeySignature6Flats,
    KeySignature7Flats
};

static AssetID GetKeySignatureAssetID(int fifths)
{
    switch (fifths)
    {
        case 1: return AssetID::KeySignature1Sharp;
        case 2: return AssetID::KeySignature2Sharps;
        case 3: return AssetID::KeySignature3Sharps;
        case 4: return AssetID::KeySignature4Sharps;
        case 5: return AssetID::KeySignature5Sharps;
        case 6: return AssetID::KeySignature6Sharps;
        case 7: return AssetID::KeySignature7Sharps;
        case -1: return AssetID::KeySignature1Flat;
        case -2: return AssetID::KeySignature2Flats;
        case -3: return AssetID::KeySignature3Flats;
        case -4: return AssetID::KeySignature4Flats;
        case -5: return AssetID::KeySignature5Flats;
        case -6: return AssetID::KeySignature6Flats;
        case -7: return AssetID::KeySignature7Flats;
    }

    return AssetID::None;
}

static AssetID GetTimeSignatureAssetID(int time)
{
    switch (time)
    {
        case 1: return AssetID::Time1;
        case 2: return AssetID::Time2;
        case 3: return AssetID::Time3;
        case 4: return AssetID::Time4;
        case 5: return AssetID::Time5;
        case 6: return AssetID::Time6;
        case 7: return AssetID::Time7;
        case 8: return AssetID::Time8;
        case 9: return AssetID::Time9;
        case 0: return AssetID::Time0;
    }

    return AssetID::None;
}

static AssetID GetNoteAssetID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return AssetID::None;
        case NoteValue::Whole: return AssetID::WholeNote;
        case NoteValue::Half: return AssetID::HalfNote;
        case NoteValue::Quarter: return AssetID::QuarterNote;
        case NoteValue::Eighth: return AssetID::EightNote;
        case NoteValue::Sixteenth: return AssetID::SixteenthNote;
        default: return AssetID::None;
    }
}

static AssetID GetNoteHeadAssetID(NoteValue type)
{
    switch (type)
    {
        case NoteValue::None: return AssetID::None;
        case NoteValue::Whole: return AssetID::WholeNote;
        case NoteValue::Half: return AssetID::HalfNoteNoteHead;
        case NoteValue::Quarter: return AssetID::QuarterNoteNoteHead;
        case NoteValue::Eighth: return AssetID::QuarterNoteNoteHead;
        case NoteValue::Sixteenth: return AssetID::QuarterNoteNoteHead;
        case NoteValue::ThirtySecond: return AssetID::QuarterNoteNoteHead;
    }

    return AssetID::None;
}

/*static AssetID GetClefAssetID(Clef clef)
{
    if (clef.sign == "G") {
        return AssetID::TrebleClef;
    } else if (clef.sign == "F") {
        return AssetID::BassClef;
    } else if (clef.sign == "C") {
        return AssetID::AltoClef;
    } else if (clef.sign == "TAB") {
        return AssetID::TABClef;
    } else if (clef.sign == "percussion") {
        return AssetID::PercussionClef;
    }

    return AssetID::None;
}*/

static AssetID GetAccidentalAssetID(Accidental::AccidentalType type)
{
    switch (type)
    {
        case Accidental::AccidentalType::None: return AssetID::None;
        case Accidental::AccidentalType::Sharp: return AssetID::Sharp;
        case Accidental::AccidentalType::Flat: return AssetID::Flat;
        case Accidental::AccidentalType::Natural: return AssetID::Natural;
        case Accidental::AccidentalType::DoubleSharp: return AssetID::DoubleSharp;
        case Accidental::AccidentalType::DoubleFlat: return AssetID::DoubleFlat;
    }

    return AssetID::None;
}

#endif // MUSIQUE_ASSETID_H