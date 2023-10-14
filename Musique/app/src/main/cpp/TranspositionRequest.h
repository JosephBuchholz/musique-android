#ifndef MUSIQUE_TRANSPOSITIONREQUEST_H
#define MUSIQUE_TRANSPOSITIONREQUEST_H

#include <string>
#include "MusicData/Types.h"

class MusicalKey
{
public:
    enum class KeyPitch
    {
        None = 0, CFlat, GFlat, DFlat, AFlat, EFlat, BFlat, F, C, G, D, A, E, B, FSharp, CSharp
    };

    enum class KeyMode
    {
        None = 0, Major, Dorian, Phrygian, Lydian, Mixolydian, Minor, Locrian
    };

public:
    int GetFifths() const;
    float GetAlterFromDiatonicNote(const DiatonicNote& note) const;

    void AddInterval(int interval, int diatonicInterval);

    static KeyPitch GetKeyFromFifths(int fifths);

    static int GetPitchValue(KeyPitch root);
    static DiatonicNote GetDiatonicValue(KeyPitch root);

public:

    KeyPitch root = KeyPitch::C;
    KeyMode mode = KeyMode::Major;
};

// note: remember to add accidentals: if have B# in the key of C and I transpose to G then it should be a F## (F# + the sharp in the key)
class TranspositionRequest
{
public:
    enum class TransposeDirection
    {
        None = 0, Closest, Up, Down
    };

public:
    int GetInterval() const;
    int GetDiatonicInterval() const;

    int GetDiatonicInterval(const MusicalKey& key1, const MusicalKey& key2) const;

    TransposeDirection GetTransposeDirection() const;

public:
    MusicalKey key = MusicalKey(); // the key to transpose to
    MusicalKey originalKey = MusicalKey(); // the key to transpose from

    TransposeDirection direction = TransposeDirection::Closest;

    enum class TransposeTablatureType
    {
        None = 0,
        Auto,       // if part contains open strings UseCapo, else NoCapo
        UseCapo,    // always use capo or different tunings
        NoCapo      // don't use capo, change the fingerings
    } transposeTablatureType = TransposeTablatureType::Auto;
};

#endif //MUSIQUE_TRANSPOSITIONREQUEST_H
