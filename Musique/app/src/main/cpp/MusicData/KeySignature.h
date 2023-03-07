#ifndef MUSIQUE_KEYSIGNATURE_H
#define MUSIQUE_KEYSIGNATURE_H

struct KeySignature {

    KeySignature(int fifths) : fifths(fifths) {}

    enum class Mode {
        None = 0, Major, Minor
    };

    static Mode GetModeFromString(const std::string& string) {
        if (string == "major") {
            return Mode::Major;
        } else if (string == "minor") {
            return Mode::Minor;
        }
        return Mode::None;
    }

    int fifths = 0;
    //int previousFifths = 0; // the fifths of the previous key signature
    Mode mode = Mode::None;
    bool print = true;
};

#endif // MUSIQUE_KEYSIGNATURE_H