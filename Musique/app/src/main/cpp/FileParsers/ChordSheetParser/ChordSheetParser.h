#ifndef MUSIQUE_CHORDSHEETPARSER_H
#define MUSIQUE_CHORDSHEETPARSER_H

#include <string>

#include "../../MusicData/Song.h"

class ChordSheetParser
{
public:

    /**
     * Parses a chord sheet file.
     *
     * @param data[in] The string data to be parsed.
     * @param song[out] The song object to parse the data into.
     */
    static void ParseChordSheet(const std::string& data, const std::shared_ptr<Song>& song);
};

#endif //MUSIQUE_CHORDSHEETPARSER_H
