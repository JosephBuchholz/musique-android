#include "TranspositionRequest.h"

#include "Utils/Math.h"

int MusicalKey::GetFifths() const
{
   return (int)root - 8;
}

float MusicalKey::GetAlterFromDiatonicNote(const DiatonicNote& note) const
{
    int fifths = GetFifths();

    if ((note == DiatonicNote::F && fifths >= 1) ||
        (note == DiatonicNote::C && fifths >= 2) ||
        (note == DiatonicNote::G && fifths >= 3) ||
        (note == DiatonicNote::D && fifths >= 4) ||
        (note == DiatonicNote::A && fifths >= 5) ||
        (note == DiatonicNote::E && fifths >= 6) ||
        (note == DiatonicNote::B && fifths >= 7))
        return 1.0f;
    else if ((note == DiatonicNote::B && fifths <= -1) ||
        (note == DiatonicNote::E && fifths <= -2) ||
        (note == DiatonicNote::A && fifths <= -3) ||
        (note == DiatonicNote::D && fifths <= -4) ||
        (note == DiatonicNote::G && fifths <= -5) ||
        (note == DiatonicNote::C && fifths <= -6) ||
        (note == DiatonicNote::F && fifths <= -7))
        return -1.0f;

    return 0.0f;
}

void MusicalKey::AddInterval(int interval, int diatonicInterval)
{
    int newPitchValue = GetPitchValue(root) + interval;
    newPitchValue %= 12;
    if (newPitchValue < 0)
        newPitchValue += 12;

    if (newPitchValue == 0)
       root = KeyPitch::C;
    else if (newPitchValue == 1)
        root = KeyPitch::DFlat;
    else if (newPitchValue == 2)
        root = KeyPitch::D;
    else if (newPitchValue == 3)
        root = KeyPitch::EFlat;
    else if (newPitchValue == 4)
        root = KeyPitch::E;
    else if (newPitchValue == 5)
        root = KeyPitch::F;
    else if (newPitchValue == 6)
        root = KeyPitch::FSharp;
    else if (newPitchValue == 7)
        root = KeyPitch::G;
    else if (newPitchValue == 8)
        root = KeyPitch::AFlat;
    else if (newPitchValue == 9)
        root = KeyPitch::A;
    else if (newPitchValue == 10)
        root = KeyPitch::BFlat;
    else if (newPitchValue == 11)
        root = KeyPitch::B;
}

MusicalKey::KeyPitch MusicalKey::GetKeyFromFifths(int fifths)
{
   return (KeyPitch)(fifths + 8);
}

int MusicalKey::GetPitchValue(KeyPitch root)
{
    switch (root)
    {
        case KeyPitch::C: return 0;
        case KeyPitch::CSharp:
        case KeyPitch::DFlat: return 1;
        case KeyPitch::D: return 2;
        case KeyPitch::EFlat: return 3;
        case KeyPitch::E: return 4;
        case KeyPitch::F: return 5;
        case KeyPitch::FSharp:
        case KeyPitch::GFlat: return 6;
        case KeyPitch::G: return 7;
        case KeyPitch::AFlat: return 8;
        case KeyPitch::A: return 9;
        case KeyPitch::BFlat: return 10;
        case KeyPitch::B:
        case KeyPitch::CFlat: return 11;
        default: return -1;
    }
}

DiatonicNote MusicalKey::GetDiatonicValue(KeyPitch root)
{
    switch (root)
    {
        case KeyPitch::CFlat:
        case KeyPitch::C:
        case KeyPitch::CSharp: return DiatonicNote::C;
        case KeyPitch::DFlat:
        case KeyPitch::D: return DiatonicNote::D;
        case KeyPitch::EFlat:
        case KeyPitch::E: return DiatonicNote::E;
        case KeyPitch::F:
        case KeyPitch::FSharp: return DiatonicNote::F;
        case KeyPitch::GFlat:
        case KeyPitch::G: return DiatonicNote::G;
        case KeyPitch::AFlat:
        case KeyPitch::A: return DiatonicNote::A;
        case KeyPitch::BFlat:
        case KeyPitch::B: return DiatonicNote::B;
        default: return DiatonicNote::C;
    }
}

int TranspositionRequest::GetDiatonicInterval(const MusicalKey &key1, const MusicalKey &key2) const
{
    int interval = (int)MusicalKey::GetDiatonicValue(key1.root) - (int)MusicalKey::GetDiatonicValue(key2.root);
    if (interval < 0)
        interval += 7;

    if (direction == TransposeDirection::Up)
    {
        if (interval == 0)
            return interval + 7;
        else
            return interval;
    }
    else if (direction == TransposeDirection::Down)
        return interval - 7;
    else if (direction == TransposeDirection::Closest)
    {
        if (interval <= std::abs(interval - 7))
            return interval; // up
        else
            return interval - 7; // down
    }

    return 0;
}

int TranspositionRequest::GetInterval() const
{
    int interval = MusicalKey::GetPitchValue(key.root) - MusicalKey::GetPitchValue(originalKey.root);
    if (interval < 0)
        interval += 12;

    if (direction == TransposeDirection::Up)
    {
        if (interval == 0)
            return interval + 12;
        else
            return interval;
    }
    else if (direction == TransposeDirection::Down)
        return interval - 12;
    else if (direction == TransposeDirection::Closest)
    {
        if (interval <= std::abs(interval - 12))
            return interval; // up
        else
            return interval - 12; // down
    }

    return 0;
}

int TranspositionRequest::GetDiatonicInterval() const
{
    return GetDiatonicInterval(key, originalKey);
}

TranspositionRequest::TransposeDirection TranspositionRequest::GetTransposeDirection() const
{
    if (direction == TransposeDirection::Closest)
    {
        int interval = MusicalKey::GetPitchValue(key.root) - MusicalKey::GetPitchValue(originalKey.root);
        if (interval < 0)
            interval += 12;

        if (interval <= std::abs(interval - 12))
            return TransposeDirection::Up;
        else
            return TransposeDirection::Down;
    }

    return direction;
}
