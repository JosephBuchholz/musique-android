#ifndef MUSIQUE_ASSETID_H
#define MUSIQUE_ASSETID_H

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

#endif // MUSIQUE_ASSETID_H