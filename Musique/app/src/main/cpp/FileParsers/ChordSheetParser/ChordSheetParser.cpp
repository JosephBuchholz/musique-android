#include "ChordSheetParser.h"

bool IsWhiteSpace(char c)
{
    return std::isspace(c);
}

const std::string allowedInChordSymbol = "ABCDEFGb#suMmdiag+1234567890./";
bool IsChordSymbol(const std::string& token)
{
    size_t found = token.find_first_not_of(allowedInChordSymbol);

    if (found == std::string::npos) // didn't find a non chord symbol char, so is a chord symbol
        return true;

    return false; // did find a non chord symbol char, so is not a chord symbol
}

DiatonicNote ParseBasicPitch(char c)
{
    if (c == 'A')
        return DiatonicNote::A;
    else if (c == 'B')
        return  DiatonicNote::B;
    else if (c == 'C')
        return DiatonicNote::C;
    else if (c == 'D')
        return DiatonicNote::D;
    else if (c == 'E')
        return DiatonicNote::E;
    else if (c == 'F')
        return DiatonicNote::F;
    else if (c == 'G')
        return DiatonicNote::G;

    return DiatonicNote::C;
}

float ParseAlter(const std::string& chordSymbol, int index)
{
    if (index < chordSymbol.size())
    {
        char c = chordSymbol[index];

        if (c == '#')
            return 1.0f;
        else if (c == 'b')
            return -1.0f;
    }

    return 0.0f;
}

Chord ParseChordSymbol(const std::string& chordSymbol)
{
    Chord newChord;

    if (chordSymbol.empty())
        return newChord;

    newChord.rootPitch.step = ParseBasicPitch(chordSymbol[0]);
    newChord.rootPitch.alter = ParseAlter(chordSymbol, 1);

    size_t foundSlash = chordSymbol.find_first_of('/');
    if (foundSlash != std::string::npos)
    {
        if (chordSymbol.size() > foundSlash + 1)
        {
            newChord.bassPitch.step = ParseBasicPitch(chordSymbol[foundSlash + 1]);
            newChord.bassPitch.alter = ParseAlter(chordSymbol, foundSlash + 2);
            newChord.hasBassNote = true;
        }
    }

    size_t foundMinor = chordSymbol.find('m');
    if (foundMinor != std::string::npos)
        newChord.harmonyType = Chord::HarmonyType::Minor;

    size_t foundSus4 = chordSymbol.find("sus4");
    if (foundSus4 != std::string::npos)
        newChord.harmonyType = Chord::HarmonyType::SuspendedFourth;

    return newChord;
}

// parses a chord symbol that contains timing info (i.e. Cmaj7..)
std::shared_ptr<CSChord> ParseTimedChordSymbol(const std::string& timedChordSymbol)
{
    size_t found = timedChordSymbol.find_first_of('.');

    std::string chordSymbol;
    std::string rhythm;

    if (found != std::string::npos)
    {
        chordSymbol = timedChordSymbol.substr(0, found);
        rhythm = timedChordSymbol.substr(found);
    }
    else
    {
        chordSymbol = timedChordSymbol;
        rhythm = "....";
    }

    LOGI("chordSymbol: %s, rhythm: %s", chordSymbol.c_str(), rhythm.c_str());

    std::shared_ptr<CSChord> newChord = std::make_shared<CSChord>();
    newChord->chordSymbol = ParseChordSymbol(chordSymbol);

    newChord->duration = (float)rhythm.size();

    return newChord;
}

void ChordSheetParser::ParseChordSheet(const std::string &data, const std::shared_ptr<Song> &song)
{
    if (!song)
        return;

    LOGV_TAG("ChordSheetParser.cpp", "data: %s", data.c_str());

    std::shared_ptr<Instrument> instrument = std::make_shared<Instrument>();

    std::shared_ptr<Staff> staff = std::make_shared<Staff>();

    staff->csStaff = std::make_shared<CSStaff>();
    staff->type = Staff::StaffType::ChordSheet;

    std::vector<std::vector<std::string>> lines;

    std::string currentToken;
    std::vector<std::string> currentLine;
    for (char c : data)
    {
        if (c == '\n')
        {
            if (!currentLine.empty())
            {
                lines.push_back(currentLine);
                currentLine = std::vector<std::string>();
            }

            continue;
        }

        if (IsWhiteSpace(c))
        {
            if (!currentToken.empty())
            {
                currentLine.push_back(currentToken);
                currentToken = "";
            }

            continue;
        }

        currentToken += c;
    }

    if (!currentLine.empty())
    {
        lines.push_back(currentLine);
    }

    std::shared_ptr<CSMeasure> currentMeasure = std::make_shared<CSMeasure>();
    int currentMeasureIndex = 0;
    int systemIndex = 0;
    float currentBeatPositionInMeasure = 0.0f;
    for (const auto& line : lines)
    {
        if (!line.empty())
        {
            if (!line[0].empty())
            {
                if (line[0][0] == '#')
                {
                    continue;
                }

                if (line[0] == "key")
                {

                    continue;
                }
            }
        }

        bool isChords = true;
        for (const auto& token : line)
        {
            isChords = (isChords && IsChordSymbol(token));
            if (!isChords)
                break;
        }

        if (isChords)
        {
            currentMeasure->isFirstMeasureOfSystem = true;

            float beginningMargin = 10.0f;
            float previousChordWidth = beginningMargin;
            for (const auto& token : line)
            {
                std::shared_ptr<CSChord> newChord = ParseTimedChordSymbol(token);

                newChord->beatPosition = currentBeatPositionInMeasure;

                currentBeatPositionInMeasure += newChord->duration;

                newChord->chordSymbol.CalculateChordName(Settings());

                newChord->position = { currentMeasure->width + previousChordWidth, 25.0f };

                previousChordWidth = newChord->duration * 30.0f;

                currentMeasure->width = newChord->position.x;

                currentMeasure->chords.push_back(newChord);

                if (currentBeatPositionInMeasure >= 4.0f)
                {
                    currentMeasure->width += 50.0f;
                    staff->csStaff->measures.push_back(currentMeasure);
                    currentMeasure = std::make_shared<CSMeasure>();
                    currentMeasureIndex++;

                    currentBeatPositionInMeasure = 0.0f;

                    previousChordWidth = beginningMargin;
                }
            }

            // add new system
            std::shared_ptr<System> newSystem = std::make_shared<System>();
            if (!song->systems.empty())
                newSystem->beginningMeasureIndex = song->systems.back()->endingMeasureIndex + 1;
            else
                newSystem->beginningMeasureIndex = 0;

            newSystem->endingMeasureIndex = currentMeasureIndex - 1;

            for (int i = newSystem->beginningMeasureIndex; i <= newSystem->endingMeasureIndex; i++)
            {
                SystemMeasure systemMeasure;
                systemMeasure.measureIndex = i;
                newSystem->systemMeasures.push_back(systemMeasure);
            }

            newSystem->position = { 150.0f, 200.0f + ((float)systemIndex * 75.0f) };

            song->systems.push_back(newSystem);
            systemIndex++;
        }
        else // is lyrics
        {
            LOGI("Is lyrics");

            std::shared_ptr<System> currentSystem = song->systems.back();

            int measureIndex = 0;
            int chordIndex = 0;

            std::shared_ptr<CSChord> currentChord;
            float currentLyricBeatPosition = 0.0f;

            if (measureIndex < staff->csStaff->measures.size())
            {
                std::shared_ptr<CSMeasure> measure = staff->csStaff->measures[measureIndex];
                if (chordIndex < measure->chords.size())
                {
                    currentChord = measure->chords[chordIndex];
                    currentLyricBeatPosition = currentChord->beatPosition;
                }
            }

            if (currentSystem)
            {
                measureIndex = currentSystem->beginningMeasureIndex;
            }

            std::vector<std::shared_ptr<CSLyric>> allLyrics;
            bool foundFirstPlaceMarker = false;

            for (const auto& token : line)
            {
                std::vector<std::shared_ptr<CSLyric>> lyrics;

                size_t firstPlaceMarker = token.find_first_of('_');

                std::string text = token.substr(0, firstPlaceMarker);
                if (!text.empty())
                {
                    std::shared_ptr<CSLyric> newLyric = std::make_shared<CSLyric>();
                    newLyric->lyricText.text = text;
                    if (!foundFirstPlaceMarker)
                        newLyric->isPickupToNextMeasure = true;
                    lyrics.push_back(newLyric);

                    if (measureIndex < staff->csStaff->measures.size())
                    {
                        if (newLyric->isPickupToNextMeasure && measureIndex - 1 >= 0)
                        {
                            std::shared_ptr<CSMeasure> measure = staff->csStaff->measures[measureIndex - 1];

                            newLyric->beatPosition = -1.0f;
                            newLyric->duration = currentChord->duration;

                            measure->lyrics.push_back(newLyric);
                        }
                        else
                        {
                            std::shared_ptr<CSMeasure> measure = staff->csStaff->measures[measureIndex];

                            newLyric->beatPosition = currentLyricBeatPosition;
                            newLyric->duration = currentChord->duration;

                            currentLyricBeatPosition += 0.1f;

                            measure->lyrics.push_back(newLyric);
                        }
                    }
                }

                if (firstPlaceMarker != std::string::npos)
                    foundFirstPlaceMarker = true;

                while (firstPlaceMarker != std::string::npos)
                {
                    size_t nextPlaceMarker = token.find_first_of('_', firstPlaceMarker + 1);

                    text = token.substr(firstPlaceMarker + 1, nextPlaceMarker - (firstPlaceMarker + 1));

                    if (!text.empty())
                    {
                        std::shared_ptr<CSLyric> newLyric = std::make_shared<CSLyric>();
                        newLyric->lyricText.text = text;
                        lyrics.push_back(newLyric);
                        LOGD_TAG("ChordSheetParser", "Placemarker on %s", text.c_str());

                        if (measureIndex < staff->csStaff->measures.size())
                        {
                            while (chordIndex >= staff->csStaff->measures[measureIndex]->chords.size())
                            {
                                LOGV_TAG("ChordSheetParser", "Measure index++: %d, ++: %d, ChordIndex: %d", measureIndex, measureIndex + 1, chordIndex);
                                measureIndex++;
                                chordIndex = 0;

                                if (measureIndex >= staff->csStaff->measures.size())
                                {
                                    measureIndex--;
                                    break;
                                }
                            }

                            std::shared_ptr<CSMeasure> measure = staff->csStaff->measures[measureIndex];
                            if (chordIndex < measure->chords.size())
                            {
                                currentChord = measure->chords[chordIndex];
                                currentLyricBeatPosition = currentChord->beatPosition;

                                newLyric->beatPosition = currentLyricBeatPosition;
                                newLyric->duration = currentChord->duration;

                                currentLyricBeatPosition += 0.1f;

                                chordIndex++;

                            }

                            measure->lyrics.push_back(newLyric);
                        }
                    }

                    firstPlaceMarker = nextPlaceMarker;
                }

                if (lyrics.size() > 1)
                {
                    std::shared_ptr<SyllableGroup> syllableGroup = std::make_shared<SyllableGroup>();
                    syllableGroup->lyrics = lyrics;
                    staff->csStaff->syllableGroups.push_back(syllableGroup);

                    for (const auto& lyric : syllableGroup->lyrics)
                    {
                        lyric->parentSyllableGroup = syllableGroup;
                    }
                }

                for (const auto& lyric : lyrics)
                {
                    allLyrics.push_back(lyric);
                }
            }

            if (!allLyrics.empty())
            {
                std::shared_ptr<LyricalPhrase> lyricalPhrase = std::make_shared<LyricalPhrase>();
                lyricalPhrase->lyrics = allLyrics;
                staff->csStaff->lyricalPhrases.push_back(lyricalPhrase);

                for (const auto& lyric : lyricalPhrase->lyrics)
                {
                    lyric->parentLyricalPhrase = lyricalPhrase;
                }
            }
        }
    }

    staff->csStaff->measures.push_back(currentMeasure);



    instrument->staves.push_back(staff);
    song->instruments.push_back(instrument);

    song->songData = SongData();
    InstrumentInfo instrumentInfo;
    instrumentInfo.visible = true;
    song->songData.instrumentInfos.push_back(instrumentInfo);


    /*float systemWidth = 0.0f;
    std::shared_ptr<System> currentSystem = std::make_shared<System>();
    currentSystem->beginningMeasureIndex = 0;
    int measureIndex = 0;
    int systemIndex = 0;
    for (const auto& measure : staff->csStaff->measures)
    {
        systemWidth += measure.width;

        SystemMeasure systemMeasure;
        systemMeasure.measureIndex = measureIndex;
        currentSystem->systemMeasures.push_back(systemMeasure);

        if (systemWidth > 1000.0f)
        {
            currentSystem->endingMeasureIndex = measureIndex;
            currentSystem->position = { 50.0f, 200.0f + (systemIndex * 75.0f) };
            song->systems.push_back(currentSystem);

            currentSystem = std::make_shared<System>();
            currentSystem->beginningMeasureIndex = measureIndex + 1;
            systemIndex++;
        }

        measureIndex++;
    }*/

    LOGI("System count: %d", song->systems.size());

    /*Page page = Page();
    song->pages.push_back(page);
    page.pageNumber.SetNumber(1);
    systemIndex = 0;
    for (const auto& system : song->systems)
    {
        page.systems.push_back(system);

        instrument->systemPositionData.push_back({ 50.0f, 0.0f });
        staff->systemPositionData.push_back({ 0.0f, 0.0f });

        systemIndex++;
    }*/

    Credit credit;
    credit.words = CreditWords();
    credit.creditType = Credit::CreditType::Title;
    credit.words.text = "Hymn Of Heaven";
    credit.words.positionX = song->displayConstants.pageWidth / 2.0f;
    credit.words.positionY = song->displayConstants.pageHeight - 80.0f;
    credit.words.align = CreditWords::TextAlign::Center;
    credit.words.fontSize.size = 20.0f;
    credit.words.justify = Justify::Center;
    song->credits.push_back(credit);

    song->displayConstants.systemLayout.systemLeftMargin = 200.0f;
    song->displayConstants.leftMargin = 200.0f;
}
