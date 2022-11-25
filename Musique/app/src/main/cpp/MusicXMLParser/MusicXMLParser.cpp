#include "MusicXMLParser.h"
#include "../Utils/Converters.h"

// ---- Conversions ----

bool MusicXMLParser::FromYesNoToBool(const char* string)
{
    if (strcmp(string, "yes") == 0) {
        return true;
    }
    return false;
}

Date MusicXMLParser::FromStringToDate(const char* string)
{
    Date date = Date();
    char* copy = strdup(string); // copy string so that it isn't const and can be used in strtok
    char* s = strtok(copy, "-"); // the first part of the date (year)

    int i = 0;
    while (s != nullptr) // get the different parts of the date using strtok(), (strtok splits a string using a delimiter)
    {
        if (i == 0) // year (yyyy)
            date.year = ToInt(s);
        else if (i == 1) // month (mm)
            date.month = ToInt(s);
        else if (i == 2) // day (dd)
            date.day = ToInt(s);
        s = strtok(nullptr, "-"); // get the next part of the date
        i++;
    }

    free(copy); // free the copied string

    return date;
}

// ---- Get Value Functions ----

std::string MusicXMLParser::GetStringValue(XMLElement* element, std::string defaultValue)
{
    if (element) {
        return element->GetText();
    }
    return defaultValue;
}

std::string MusicXMLParser::GetStringValue(const std::string& elementName, XMLElement* elementParent, std::string defaultValue)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetStringValue(element, defaultValue);
    }
    return defaultValue;
}

float MusicXMLParser::GetFloatValue(XMLElement* element, float defaultValue)
{
    if (element) {
        const char* s = element->GetText();
        float value;
        try {
            value = std::stof(s);
        }
        catch (std::exception& e) {
            // TODO: error
            LOGE("NOT A FLOAT");
            return defaultValue;
        }
        return value;
    }
    return defaultValue;
}

float MusicXMLParser::GetFloatValue(const std::string& elementName, XMLElement* elementParent, float defaultValue)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetFloatValue(element, defaultValue);
    }
    return defaultValue;
}

int MusicXMLParser::GetIntValue(XMLElement* element, int defaultValue)
{
    if (element) {
        std::string c = element->GetText();
        if (IsInt(c))
            return ToInt(c);
    }
    return defaultValue;
}

int MusicXMLParser::GetIntValue(const std::string& elementName, XMLElement* elementParent, int defaultValue)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetIntValue(element, defaultValue);
    }
    return defaultValue;
}

unsigned int MusicXMLParser::GetUnsignedIntValue(XMLElement* element, unsigned int defaultValue)
{
    if (element) {
        std::string c = element->GetText();
        if (IsUnsignedInt(c))
            return ToUnsignedInt(c);
    }
    return defaultValue;
}

unsigned int MusicXMLParser::GetUnsignedIntValue(const std::string& elementName, XMLElement* elementParent, unsigned int defaultValue)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetUnsignedIntValue(element, defaultValue);
    }
    return defaultValue;
}


// ---- Get Attribute Functions ----

bool MusicXMLParser::GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return FromYesNoToBool(attribute);
    }
    return defaultValue;
}

float MusicXMLParser::GetFloatAttribute(XMLElement* element, const char* s, float defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToFloat(attribute);
    }
    return defaultValue;
}

std::string MusicXMLParser::GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return attribute;
    }
    return defaultValue;
}

int MusicXMLParser::GetNumberAttribute(XMLElement* element, const char* s, int defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToInt(attribute);
    }
    return defaultValue;
}

AboveBelowType MusicXMLParser::GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "above") == 0) {
            return AboveBelowType::Above;
        } else if (strcmp(attribute, "below") == 0) {
            return AboveBelowType::Below;
        }
    }
    return defaultValue;
}

StartStopType MusicXMLParser::GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "start") == 0) {
            return StartStopType::Start;
        } else if (strcmp(attribute, "stop") == 0) {
            return StartStopType::Stop;
        } else if (strcmp(attribute, "continue") == 0) {
            return StartStopType::Continue;
        }
    }
    return defaultValue;
}

FontFamily MusicXMLParser::GetFontFamilyAttribute(XMLElement* element, const char* s, FontFamily defaultValue)
{
    const char* attribute = element->Attribute(s);
    FontFamily fontFamily = defaultValue;
    if (attribute) {
        fontFamily.fonts.push_back(attribute); // TODO: need to parse out comma sperated values
    }
    return fontFamily;
}

FontSize MusicXMLParser::GetFontSizeAttribute(XMLElement* element, const char* s, FontSize defaultValue)
{
    // TODO: needs implemented
    return defaultValue;
}

FontStyle MusicXMLParser::GetFontStyleAttribute(XMLElement* element, const char* s, FontStyle defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "normal") == 0) {
            return FontStyle::Normal;
        } else if (strcmp(attribute, "italic") == 0) {
            return FontStyle::Italic;
        }
    }
    return defaultValue;
}

FontWeight MusicXMLParser::GetFontWeightAttribute(XMLElement* element, const char* s, FontWeight defaultValue)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "normal") == 0) {
            return FontWeight::Normal;
        } else if (strcmp(attribute, "bold") == 0) {
            return FontWeight::Bold;
        }
    }
    return defaultValue;
}

// ---- Parse Functions ----

Lyric::SyllabicType MusicXMLParser::ParseSyllabicType(const std::string& value)
{
    if (value == "begin")
        return Lyric::SyllabicType::Begin;
    else if (value == "end")
        return Lyric::SyllabicType::End;
    else if (value == "middle")
        return Lyric::SyllabicType::Middle;
    else if (value == "single")
        return Lyric::SyllabicType::Single;
    else
        LOGE("unrecognized syllabic type");
    return Lyric::SyllabicType::None;
}

// parses a single lyric element
Lyric MusicXMLParser::ParseLyric(XMLElement* lyricElement)
{
    Lyric lyric = Lyric();
    if (lyricElement)
    {
        lyric.number = GetNumberAttribute(lyricElement, "number", lyric.number);

        // loop through all syllabic and text elements
        XMLNode* previousElement = lyricElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "syllabic") == 0) // syllabic
                {
                    XMLElement* syllabicElement = element;
                    lyric.syllabics.push_back(ParseSyllabicType(syllabicElement->GetText())); // parse type and add to syllabic vector
                }
                else if (strcmp(value, "text") == 0) // text
                {
                    XMLElement* textElement = element;
                    LyricText text = LyricText();
                    text.text = textElement->GetText();
                    lyric.text.push_back(text);
                }
                else
                {
                    LOGE("didn't recognize element in LYRIC");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
    else
    {
        LOGE("lyricElement is null");
    }
    return lyric;
}

void MusicXMLParser::ParseBaseElement(XMLElement* element, BaseElement& newElement)
{
    if (element)
    {
        // TODO: implement
    }
}

void MusicXMLParser::ParsePrintableElement(XMLElement* element, PrintableElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParseBaseElement(element, newElement);
    }
}

void MusicXMLParser::ParseVisibleElement(XMLElement* element, VisibleElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParsePrintableElement(element, newElement);
    }
}

void MusicXMLParser::ParseTextualElement(XMLElement* element, TextualElement& newElement)
{
    if (element)
    {
        newElement.fontFamily = GetFontFamilyAttribute(element, "font-family");
        newElement.fontSize = GetFontSizeAttribute(element, "font-size");
        newElement.fontStyle = GetFontStyleAttribute(element, "font-style");
        newElement.fontWeight = GetFontWeightAttribute(element, "font-weight");

        ParseVisibleElement(element, newElement);
    }
}

Rehearsal MusicXMLParser::ParseRehearsal(XMLElement* element)
{
    Rehearsal rehearsal = Rehearsal();

    if (element)
    {
        rehearsal.text.string = element->GetText();
        ParseTextualElement(element, rehearsal);
    }

    return rehearsal;
}

Words MusicXMLParser::ParseWords(XMLElement* element)
{
    Words words = Words();

    if (element)
    {
        words.text.string = element->GetText();
        ParseTextualElement(element, words);
    }

    return words;
}

Direction MusicXMLParser::ParseDirection(XMLElement* directionElement)
{
    Direction direction = Direction();
    if (directionElement)
    {
        XMLElement* directionTypeElement = directionElement->FirstChildElement("direction-type");
        if (directionTypeElement)
        {
            // rehearsal
            XMLElement* rehearsalElement = directionTypeElement->FirstChildElement("rehearsal");
            if (rehearsalElement)
            {
                direction.rehearsals.push_back(ParseRehearsal(rehearsalElement));
            }

            // words
            XMLElement* wordsElement = directionTypeElement->FirstChildElement("words");
            if (wordsElement)
            {
                direction.words.push_back(ParseWords(wordsElement));
            }
        }

    }
    return direction;
}

void MusicXMLParser::ParseWorkElement(XMLElement* workElement, std::string& workTitle, int& workNumber)
{
    if (workElement)
    {
        // work title
        XMLElement* workTitleElement = workElement->FirstChildElement("work-title");
        workTitle = GetStringValue(workTitleElement, workTitle);

        // work number
        XMLElement* workNumberElement = workElement->FirstChildElement("work-number");
        if (workNumberElement)
        {
            workNumber = GetIntValue(workNumberElement, workNumber);
        }
    }
}

void MusicXMLParser::ParseEncodingElement(XMLElement* encodingElement, Song* song)
{
    if (encodingElement)
    {
        // loop through all elements
        XMLNode* previousElement = encodingElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "encoding-date") == 0) // encoding-date
                {
                    song->encodingDate = FromStringToDate(GetStringValue(element, "1900-01-01").c_str());
                }
                else if (strcmp(value, "encoder") == 0) // encoder
                {
                    XMLElement* encoderElement = element;
                    Song::Encoder encoder = Song::Encoder();
                    encoder.name = GetStringValue(encoderElement, encoder.name);
                    encoder.strType = GetStringAttribute(encoderElement, "type", encoder.strType);
                    if (encoder.strType == "music")
                        encoder.type = Song::Encoder::EncoderType::Music;
                    else if (encoder.strType == "words")
                        encoder.type = Song::Encoder::EncoderType::Words;
                    else if (encoder.strType == "arrangement")
                        encoder.type = Song::Encoder::EncoderType::Arrangement;
                    song->encoders.push_back(encoder);
                }
                else if (strcmp(value, "software") == 0) // software
                {
                    song->software = GetStringValue(element, song->software);
                }
                else if (strcmp(value, "encoding-description") == 0) // encoding description
                {
                    song->encodingDescription = GetStringValue(element, song->encodingDescription);
                }
                else if (strcmp(value, "supports") == 0) // supports
                {
                    // TODO: handle this
                }
                else
                {
                    LOGE("didn't recognize element in Encoding");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
}

void MusicXMLParser::ParseIdentificationElement(XMLElement* idElement, Song* song)
{
    if (idElement)
    {
        // loop through all elements
        XMLNode* previousElement = idElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "creator") == 0) // creator
                {
                    XMLElement* creatorElement = element;
                    Song::Creator creator = Song::Creator();
                    creator.name = GetStringValue(creatorElement, creator.name);
                    creator.strType = GetStringAttribute(creatorElement, "type", creator.strType);
                    if (creator.strType == "composer")
                        creator.type = Song::Creator::CreatorType::Composer;
                    else if (creator.strType == "lyricist")
                        creator.type = Song::Creator::CreatorType::Lyricist;
                    else if (creator.strType == "arranger")
                        creator.type = Song::Creator::CreatorType::Arranger;
                    song->creators.push_back(creator);
                }
                else if (strcmp(value, "rights") == 0) // rights
                {
                    XMLElement* rightElement = element;
                    Song::Rights rights = Song::Rights();
                    rights.right = GetStringValue(rightElement, rights.right);
                    rights.strType = GetStringAttribute(rightElement, "type", rights.strType);
                    if (rights.strType == "music")
                        rights.type = Song::Rights::RightType::Music;
                    else if (rights.strType == "words")
                        rights.type = Song::Rights::RightType::Words;
                    else if (rights.strType == "arrangement")
                        rights.type = Song::Rights::RightType::Arrangement;
                    song->rights.push_back(rights);
                }
                else if (strcmp(value, "encoding") == 0) // encoding
                {
                    ParseEncodingElement(element, song);
                }
                else if (strcmp(value, "source") == 0) // source
                {
                    // TODO: handle this
                }
                else if (strcmp(value, "relation") == 0) // relation
                {
                    // TODO: handle this
                }
                else if (strcmp(value, "miscellaneous") == 0) // miscellaneous
                {
                    // TODO: handle this
                }
                else
                {
                    LOGE("didn't recognize element in Identification");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
}

void MusicXMLParser::ParseDefaultsElement(XMLElement* defaultsElement)
{
    // TODO: implement
}

void MusicXMLParser::ParseCreditElement(XMLElement* creditElement)
{
    // TODO: implement
}

void MusicXMLParser::ParseHarmonyElement(XMLElement* harmonyElement, float& currentTimeInMeasure, std::vector<Measure*> currentMeasures)
{
    Chord newChord;

    // root note
    XMLElement* rootElement = harmonyElement->FirstChildElement("root");
    if (rootElement)
    {
        XMLElement* rootStepElement = rootElement->FirstChildElement("root-step");
        if (rootStepElement)
        {
            newChord.rootPitch.step = GetStringValue(rootStepElement, newChord.rootPitch.step);
        }
        else
            ; // TODO: error: this element is required

        newChord.rootPitch.alter = GetFloatValue("root-alter", rootElement, newChord.rootPitch.alter);
    }

    // bass note
    XMLElement* bassElement = harmonyElement->FirstChildElement("bass");
    if (bassElement)
    {
        XMLElement* bassStepElement = bassElement->FirstChildElement("bass-step");
        if (bassStepElement)
        {
            newChord.bassPitch.step = GetStringValue(bassStepElement, newChord.bassPitch.step);
        }
        else
            ; // TODO: error: this element is required

        newChord.bassPitch.alter = GetFloatValue("bass-alter", bassElement, newChord.bassPitch.alter);

        newChord.bassSeparator.string = GetStringValue("bass-separator", bassElement, newChord.bassSeparator.string);

        std::string bassPosStr = GetStringAttribute(bassElement, "arrangement", "");
        if (bassPosStr == "horizontal")
            newChord.bassPos = Chord::BassPos::Horizontal;
        else if (bassPosStr == "vertical")
            newChord.bassPos = Chord::BassPos::Vertical;
        else if (bassPosStr == "diagonal")
            newChord.bassPos = Chord::BassPos::Diagonal;
        else if (bassPosStr.empty())
            newChord.bassPos = Chord::BassPos::None;
        else
            ; // TODO: error: this is not possible

        newChord.hasBassNote = true;
    }

    // kind note
    XMLElement* kindElement = harmonyElement->FirstChildElement("kind");
    if (kindElement)
    {
        std::string typeString = GetStringValue(kindElement, "");

        if (typeString == "none")
            newChord.harmonyType = Chord::HarmonyType::NoHarmony;
        else if (typeString == "other")
            newChord.harmonyType = Chord::HarmonyType::Other;
        else if (typeString == "augmented")
            newChord.harmonyType = Chord::HarmonyType::Augmented;
        else if (typeString == "augmented-seventh")
            newChord.harmonyType = Chord::HarmonyType::AugmentedSeventh;
        else if (typeString == "diminished")
            newChord.harmonyType = Chord::HarmonyType::Diminished;
        else if (typeString == "diminished-seventh")
            newChord.harmonyType = Chord::HarmonyType::DiminishedSeventh;
        else if (typeString == "dominant")
            newChord.harmonyType = Chord::HarmonyType::Dominant;
        else if (typeString == "dominant-11th")
            newChord.harmonyType = Chord::HarmonyType::Dominant11th;
        else if (typeString == "dominant-13th")
            newChord.harmonyType = Chord::HarmonyType::Dominant13th;
        else if (typeString == "dominant-ninth")
            newChord.harmonyType = Chord::HarmonyType::DominantNinth;
        else if (typeString == "French")
            newChord.harmonyType = Chord::HarmonyType::French;
        else if (typeString == "German")
            newChord.harmonyType = Chord::HarmonyType::German;
        else if (typeString == "half-diminished")
            newChord.harmonyType = Chord::HarmonyType::HalfDiminished;
        else if (typeString == "Italian")
            newChord.harmonyType = Chord::HarmonyType::Italian;
        else if (typeString == "major")
            newChord.harmonyType = Chord::HarmonyType::Major;
        else if (typeString == "major-11th")
            newChord.harmonyType = Chord::HarmonyType::Major11th;
        else if (typeString == "major-13th")
            newChord.harmonyType = Chord::HarmonyType::Major13th;
        else if (typeString == "major-minor")
            newChord.harmonyType = Chord::HarmonyType::MajorMinor;
        else if (typeString == "major-ninth")
            newChord.harmonyType = Chord::HarmonyType::MajorNinth;
        else if (typeString == "major-seventh")
            newChord.harmonyType = Chord::HarmonyType::MajorSeventh;
        else if (typeString == "major-sixth")
            newChord.harmonyType = Chord::HarmonyType::MajorSixth;
        else if (typeString == "minor")
            newChord.harmonyType = Chord::HarmonyType::Minor;
        else if (typeString == "minor-11th")
            newChord.harmonyType = Chord::HarmonyType::Minor11th;
        else if (typeString == "minor-13th")
            newChord.harmonyType = Chord::HarmonyType::Minor13th;
        else if (typeString == "minor-ninth")
            newChord.harmonyType = Chord::HarmonyType::MinorNinth;
        else if (typeString == "minor-seventh")
            newChord.harmonyType = Chord::HarmonyType::MinorSeventh;
        else if (typeString == "minor-sixth")
            newChord.harmonyType = Chord::HarmonyType::MinorSixth;
        else if (typeString == "Neapolitan")
            newChord.harmonyType = Chord::HarmonyType::Neapolitan;
        else if (typeString == "pedal")
            newChord.harmonyType = Chord::HarmonyType::Pedal;
        else if (typeString == "power")
            newChord.harmonyType = Chord::HarmonyType::Power;
        else if (typeString == "suspended-fourth")
            newChord.harmonyType = Chord::HarmonyType::SuspendedFourth;
        else if (typeString == "suspended-second")
            newChord.harmonyType = Chord::HarmonyType::SuspendedSecond;
        else if (typeString == "Tristan")
            newChord.harmonyType = Chord::HarmonyType::Tristan;
        else
            ; // TODO: error: this is not possible
        newChord.brackets = GetBoolAttribute(kindElement, "bracket-degrees", newChord.brackets);
        newChord.parentheses = GetBoolAttribute(kindElement, "parentheses-degrees", newChord.parentheses);
        newChord.stackDegrees = GetBoolAttribute(kindElement, "stack-degrees", newChord.stackDegrees);
        newChord.useSymbols = GetBoolAttribute(kindElement, "use-symbols", newChord.useSymbols);
        newChord.harmonyTypeText = GetStringAttribute(kindElement, "text", newChord.harmonyTypeText);
    }
    else
        ; // TODO: error: this element is required

    // loop through all degree elements
    XMLNode* previousElement = kindElement->FirstChildElement(); // first element
    while (true)
    {
        if (previousElement) {
            XMLElement* element = previousElement->ToElement();
            const char* value = element->Value();
            if (strcmp(value, "degree") == 0)
            {
                XMLElement* degreeElement = element;
                ChordDegree newDegree = ChordDegree();

                ParsePrintableElement(degreeElement, newDegree);

                // degree value element
                XMLElement* degreeValueElement = degreeElement->FirstChildElement("degree-value");
                if (degreeValueElement)
                {
                    ParseVisibleElement(degreeValueElement, newDegree.degree);
                    newDegree.degree.degree = GetUnsignedIntValue(degreeValueElement, newDegree.degree.degree);
                }

                // degree type element
                XMLElement* degreeTypeElement = degreeElement->FirstChildElement("degree-type");
                if (degreeTypeElement)
                {
                    ParseVisibleElement(degreeTypeElement, newDegree.degreeType);
                    std::string s = GetStringValue(degreeTypeElement, "");
                    if (s == "add")
                        newDegree.degreeType.type = DegreeType::Type::Add;
                    else if (s == "subtract")
                        newDegree.degreeType.type = DegreeType::Type::Subtract;
                    else if (s == "alter")
                        newDegree.degreeType.type = DegreeType::Type::Alter;
                    else if (s.empty())
                        newDegree.degreeType.type = DegreeType::Type::None;
                    else
                        ; // TODO: error not a valid value
                }

                // degree alter element
                XMLElement* degreeAlterElement = degreeElement->FirstChildElement("degree-alter");
                if (degreeAlterElement)
                {
                    ParseVisibleElement(degreeAlterElement, newDegree.degreeAlter);
                    newDegree.degreeAlter.alter = GetFloatValue(degreeAlterElement, newDegree.degreeAlter.alter);
                }

                newChord.degrees.push_back(newDegree);
            }
        }
        else
        {
            break;
        }
        previousElement = previousElement->NextSiblingElement();
    }

    newChord.beatPosition = currentTimeInMeasure; // note: harmony elements don't increment the time
    newChord.CalculateChordName();
    //LOGE("Chord (%s) is at %f", newChord.chordName.string.c_str(), newChord.beatPosition);
    currentMeasures[newChord.staff - 1]->chords.push_back(newChord);
}

void MusicXMLParser::ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, bool isTab, Note* currentNote, Note* previousNote, std::vector<Measure*> currentMeasures, int measureNumber, std::string& error)
{
    if (isTab) {
        currentNote->type = Note::NoteType::Tab;
    }

    // staff
    XMLElement* staffElement = noteElement->FirstChildElement("staff");
    if (staffElement)
    {
        currentNote->staff = ToInt(staffElement->GetText());
    }
    else
    {
        currentNote->staff = 1;
    }

    // chord
    XMLElement* chordElement = noteElement->FirstChildElement("chord");
    if (chordElement)
    {
        currentNote->isChord = true;
    }

    // rest
    XMLElement* restElement = noteElement->FirstChildElement("rest");
    if (restElement)
    {
        currentNote->isRest = true;
        currentNote->isFullMeasureRest = GetBoolAttribute(restElement, "measure", false);
    }

    // pitch
    XMLElement* pitch = noteElement->FirstChildElement("pitch");
    if (pitch)
    {
        // step
        XMLElement* step = pitch->FirstChildElement("step");
        if (step)
        {
            currentNote->pitch.step = step->GetText();
        }
        else
        {
            error = "no step element in pitch element";
        }

        // octave
        XMLElement* octave = pitch->FirstChildElement("octave");
        if (step)
        {
            currentNote->pitch.octave = ToInt(octave->GetText());
        }
        else
        {
            error = "no octave element in pitch element";
        }

        // alter
        XMLElement* alter = pitch->FirstChildElement("alter");
        if (alter)
        {
            currentNote->pitch.alter = float(ToInt(alter->GetText()));
        }
    }

    // duration
    XMLElement* duration = noteElement->FirstChildElement("duration");
    if (duration)
    {
        int divisions = currentMeasures[currentNote->staff-1]->divisions;
        if (divisions != 0)
        {
            currentNote->duration.duration = (1.0f / (float)divisions) * (float)ToInt(duration->GetText());
        }
        else
        {
            error = "Error divisions is zero";
        }

        // calculating beat position of note
        if (currentNote->isChord) { // is a chord so beat position is the same as the previous note's beat position
            if (previousNote)
                currentNote->beatPosition = currentTimeInMeasure - previousNote->duration.duration;
            else
                currentNote->beatPosition = 0.0f;
        } else { // not a chord so beat position is equal to the current time in the measure
            currentNote->beatPosition = currentTimeInMeasure;
        }

        // increment time in measure
        if (!currentNote->isChord)
        {
            currentTimeInMeasure += currentNote->duration.duration;
        }
    }
    /*else
    {
        error = "note has no duration";
    }*/

    // tie
    XMLElement* tieElement = noteElement->FirstChildElement("tie");
    if (tieElement)
    {

    }

    // voice
    XMLElement* voiceElement = noteElement->FirstChildElement("voice");
    if (voiceElement)
    {
        currentNote->voice = ToInt(voiceElement->GetText());
    }

    // type
    XMLElement* noteType = noteElement->FirstChildElement("type");
    if (noteType)
    {
        currentNote->CalculateDurationTypeFromString(noteType->GetText());
    }

    // lyrics
    XMLNode* previousLyricElement = noteElement->FirstChildElement("lyric");
    while (true)
    {
        if (previousLyricElement) {
            XMLElement* lyricElement = previousLyricElement->ToElement();
            Lyric lyric = ParseLyric(lyricElement);
            currentNote->lyrics.push_back(lyric);
        }
        else
        {
            break;
        }
        previousLyricElement = previousLyricElement->NextSiblingElement("lyric");
    }

    // accidental
    XMLElement* accidentalElement = noteElement->FirstChildElement("accidental");
    if (accidentalElement)
    {
        currentNote->accidental.accidentalType = Accidental::CalculateAccidentalTypeFromString(accidentalElement->GetText());

        currentNote->accidental.isCautionary = GetBoolAttribute(accidentalElement, "cautionary", false);
        currentNote->accidental.isEditorial = GetBoolAttribute(accidentalElement, "editorial", false);
        currentNote->accidental.hasBrackets = GetBoolAttribute(accidentalElement, "bracket", false);
        currentNote->accidental.hasParentheses = GetBoolAttribute(accidentalElement, "parentheses", false);
    }

    // stem
    XMLElement* stemElement = noteElement->FirstChildElement("stem");
    if (stemElement)
    {
        currentNote->noteStem.stemType = currentNote->noteStem.CalculateStemTypeFromString(stemElement->GetText());
    }

    // beam
    XMLElement* beamElement = noteElement->FirstChildElement("beam");
    if (beamElement)
    {
        currentNote->beam.beamType = currentNote->beam.CalculateBeamTypeFromString(beamElement->GetText());
        currentNote->beam.beamLevel = GetNumberAttribute(beamElement, "number", 1);
    }

    // notations
    XMLElement* notations = noteElement->FirstChildElement("notations");
    if (notations)
    {
        // slurs
        XMLElement* slurElement = notations->FirstChildElement("slur");
        if (slurElement)
        {
            Slur slur = Slur();
            slur.id = GetNumberAttribute(slurElement, "number");
            slur.placement = GetAboveBelowAttribute(slurElement, "placement");
            slur.type = GetStartStopAttribute(slurElement, "type");
            currentNote->slurs.push_back(slur);
        }

        // technical
        XMLElement* technical = notations->FirstChildElement("technical");
        if (technical)
        {
            // hammer ons
            XMLNode* previousHammerOnElement = technical->FirstChildElement("hammer-on");
            while (true)
            {
                if (previousHammerOnElement) {
                    XMLElement* hammerOnElement = previousHammerOnElement->ToElement();
                    if (hammerOnElement)
                    {
                        TABSlur tabSlur = TABSlur();
                        tabSlur.slurType = TABSlur::SlurType::HammerOn;
                        tabSlur.id = GetNumberAttribute(hammerOnElement, "number");
                        tabSlur.placement = GetAboveBelowAttribute(hammerOnElement, "placement");
                        tabSlur.type = GetStartStopAttribute(hammerOnElement, "type");
                        if (hammerOnElement->GetText()) {
                            tabSlur.text = hammerOnElement->GetText();
                        }
                        currentNote->tabSlurs.push_back(tabSlur);
                    }
                }
                else
                {
                    break;
                }
                previousHammerOnElement = previousHammerOnElement->NextSiblingElement("hammer-on");
            }

            // pull offs
            XMLNode* previousPullOffElement = technical->FirstChildElement("pull-off");
            while (true)
            {
                if (previousPullOffElement) {
                    XMLElement* pullOffElement = previousPullOffElement->ToElement();
                    if (pullOffElement)
                    {
                        TABSlur tabSlur = TABSlur();
                        tabSlur.slurType = TABSlur::SlurType::PullOff;
                        tabSlur.id = GetNumberAttribute(pullOffElement, "number");
                        tabSlur.placement = GetAboveBelowAttribute(pullOffElement, "placement");
                        tabSlur.type = GetStartStopAttribute(pullOffElement, "type");
                        if (pullOffElement->GetText()) {
                            tabSlur.text = pullOffElement->GetText();
                        }
                        currentNote->tabSlurs.push_back(tabSlur);
                    }
                }
                else
                {
                    break;
                }
                previousPullOffElement = previousPullOffElement->NextSiblingElement("pull_off");
            }

            // - TAB only -
            if (currentNote->type == Note::NoteType::Tab and isTab) {

                // string
                XMLElement* string = technical->FirstChildElement("string");
                if (string)
                {
                    currentNote->string = ToInt(string->GetText());
                }

                // fret
                XMLElement* fret = technical->FirstChildElement("fret");
                if (fret)
                {
                    currentNote->fret = ToInt(fret->GetText());
                }
            }
        }
    }

    currentNote->measureIndex = measureNumber - 1;
    currentMeasures[currentNote->staff - 1]->notes.push_back(currentNote);
}

// main file parsing
Song* MusicXMLParser::ParseMusicXML(const std::string& data, std::string& error)
{
    LOGW("STARTING TO PARSE");
    Song* song = new Song();
    LOGW("Created song data");

    tinyxml2::XMLDocument doc;
    const char *d = data.c_str();
    tinyxml2::XMLError xmlError = doc.Parse(d);
    LOGW("parced document with tinyxml2");
    if (xmlError != tinyxml2::XMLError::XML_SUCCESS) {
        LOGE("error: %i", xmlError);
        error = "XMLERROR: " + ToString(int(xmlError));
    }

    if (doc.Error())
    {
        LOGE("doc error: %s: %s", doc.ErrorName(), doc.ErrorStr());
        error = "XML DOC ERROR";
        //error = doc.ErrorDesc();
    }
    else
    {
        // will create problems if it is socre-timewise
        XMLElement* root = doc.FirstChildElement("score-partwise");
        if (root)
        {
            song->musicXMLVersion = root->Attribute("version");

            // work
            XMLElement* workElement = root->FirstChildElement("work");
            if (workElement)
            {
                ParseWorkElement(workElement, song->songData.songTitle, song->songData.workNumber);
            }

            song->songData.movementNumber = GetStringValue("movement-number", root, song->songData.movementNumber); // movement number
            song->songData.movementTitle = GetStringValue("movement-title", root, song->songData.movementTitle); // movement title

            // identification
            XMLElement* identificationElement = root->FirstChildElement("identification");
            if (identificationElement)
            {
                ParseIdentificationElement(identificationElement, song);
            }

            // defaults
            XMLElement* defaultsElement = root->FirstChildElement("defaults");
            if (defaultsElement)
            {
                ParseDefaultsElement(defaultsElement);
            }

            // credit
            XMLElement* creditElement = root->FirstChildElement("credit");
            if (creditElement)
            {
                ParseCreditElement(creditElement);
            }

            // part list
            XMLElement* partList = root->FirstChildElement("part-list");
            if (partList)
            {
                // score parts
                XMLNode* previousScorePartElement = partList->FirstChildElement("score-part");
                while (true)
                {
                    if (previousScorePartElement)
                    {
                        XMLElement* scorePart = previousScorePartElement->ToElement();
                        Instrument* instrument = new Instrument();
                        song->instruments.push_back(instrument);

                        // part id
                        instrument->id = scorePart->Attribute("id");

                        // part name
                        XMLElement* partName = scorePart->FirstChildElement("part-name");
                        if (partName)
                        {
                            instrument->name.string = partName->GetText();
                            instrument->name.print = GetBoolAttribute(partName, "print-object", true);
                        }

                        // part name abbreviation
                        XMLElement* partAbbreviation = scorePart->FirstChildElement("part-abbreviation");
                        if (partAbbreviation)
                        {
                            instrument->nameAbbreviation.string = partAbbreviation->GetText();
                            instrument->nameAbbreviation.print = GetBoolAttribute(partAbbreviation, "print-object", true);
                        }

                        // midi instrument
                        XMLElement* midiInstrumentElement = scorePart->FirstChildElement("midi-instrument");
                        if (midiInstrumentElement)
                        {
                            instrument->midiInstrument.id = GetStringAttribute(midiInstrumentElement, "id", "");

                            // midi channel
                            XMLElement* channelElement = midiInstrumentElement->FirstChildElement("midi-channel");
                            if (channelElement)
                            {
                                instrument->midiInstrument.channel = ToInt(channelElement->GetText()) - 1;
                            }

                            // midi channel
                            XMLElement* programElement = midiInstrumentElement->FirstChildElement("midi-program");
                            if (programElement)
                            {
                                instrument->midiInstrument.program = ToInt(programElement->GetText()) - 1;
                            }

                            // volume
                            XMLElement* volumeElement = midiInstrumentElement->FirstChildElement("volume");
                            if (volumeElement)
                            {
                                instrument->midiInstrument.volume = ToInt(volumeElement->GetText());
                            }

                            // pan
                            XMLElement* panElement = midiInstrumentElement->FirstChildElement("pan");
                            if (panElement)
                            {
                                instrument->midiInstrument.pan = ToInt(panElement->GetText());
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                    previousScorePartElement = previousScorePartElement->NextSiblingElement("score-part");
                }

                // old score part
                /*XMLElement* scorePart = partList->FirstChildElement("score-part");
                if (scorePart)
                {
                    Instrument* instrument = new Instrument();
                    songData->instruments.push_back(instrument);

                    // part id
                    instrument->id = scorePart->Attribute("id");

                    // part name
                    XMLElement* partName = scorePart->FirstChildElement("part-name");
                    if (partName)
                    {
                        instrument->name.string = partName->GetText();
                    }
                }
                else
                {
                    error = "ERROR no score-part";
                }*/
            }
            else
            {
                error = "ERROR no part-list";
            }


            // parts
            XMLNode* previous = root->FirstChildElement("part");
            while (true) // looping through all parts
            {
                if (previous)
                {
                    XMLElement* part = previous->ToElement();
                    std::string id = part->Attribute("id");
                    Instrument* currentInst = song->GetInstrument(id);
                    if (currentInst == nullptr) {
                        error = "Instrument " + id + " does not exist";
                        return song;
                    }

                    //currentInst->staves.push_back(new Staff());
                    /*LOGD("Part id: %s", id.c_str());
                    Instrument* currentInst = songData->GetInstrument(id);
                    if (currentInst != nullptr) {
                        LOGD("Instrument is not null");
                        Staff *s = new Staff();
                        if (s != nullptr) {
                            LOGD("Staff is not null");
                            currentInst->staves.push_back(s);
                            LOGD("Pushed back staff");
                        } else {
                            LOGD("Staff is null");
                        }
                    } else {
                        LOGD("Instrument is null");
                    */

                    // measures

                    bool firstMeasure = true;
                    bool isTab = false;
                    XMLNode* previousMeasureElement = part->FirstChildElement("measure");
                    std::vector<Measure*> previousMeasures;
                    while (true)
                    {
                        if (previousMeasureElement)
                        {
                            XMLElement* measure = previousMeasureElement->ToElement();
                            int measureNumber = ToInt(measure->Attribute("number"));
                            std::vector<Measure*> currentMeasures;
                            if (!firstMeasure)
                            {
                                // creating measures for each staff
                                for (int i = 0; i < currentInst->staves.size(); i++)
                                {
                                    Measure* newMeasure = new Measure();
                                    newMeasure->number = measureNumber;
                                    newMeasure->index = measureNumber - 1;
                                    newMeasure->staff = i+1;
                                    currentMeasures.push_back(newMeasure);
                                }
                            }

                            float currentTimeInMeasure = 0.0f; // keeps track of the current time that has passed in the current measure in beats(quarter notes)

                            // attributes
                            XMLElement* attributes = measure->FirstChildElement("attributes");
                            if (attributes)
                            {
                                // staves
                                int numStaves = 1;
                                XMLElement* stavesElement = attributes->FirstChildElement("staves");
                                if (stavesElement)
                                {
                                    numStaves = ToInt(stavesElement->GetText());
                                }

                                if (firstMeasure)
                                {
                                    // adding staves
                                    for (int i = 0; i < numStaves; i++)
                                    {
                                        currentInst->staves.push_back(new Staff());
                                    }

                                    // creating measures for each staff
                                    for (int i = 0; i < currentInst->staves.size(); i++)
                                    {
                                        Measure* newMeasure = new Measure();
                                        newMeasure->staff = i+1;
                                        currentMeasures.push_back(newMeasure);
                                    }
                                }

                                // divisions
                                XMLElement* divisions = attributes->FirstChildElement("divisions");
                                if (divisions) {
                                    for (auto m : currentMeasures) { m->divisions = ToInt(divisions->GetText()); } // setting divisions for all current measures
                                }

                                // key signature
                                XMLElement* keySignatureElement = attributes->FirstChildElement("key");
                                if (keySignatureElement)
                                {
                                    KeySignature keySignature = KeySignature(0);
                                    // print object
                                    keySignature.print = GetBoolAttribute(keySignatureElement, "print-object", true);

                                    // fifths
                                    XMLElement* fifths = keySignatureElement->FirstChildElement("fifths");
                                    if (fifths)
                                        keySignature.fifths = ToInt(fifths->GetText());

                                    // mode
                                    XMLElement* mode = keySignatureElement->FirstChildElement("mode");
                                    if (mode)
                                        keySignature.mode = KeySignature::GetModeFromString(mode->GetText());

                                    for (auto m : currentMeasures) { m->keySignature = keySignature; m->showKeySignature = true; }
                                }

                                // time signature
                                XMLElement* timeSignatureElement = attributes->FirstChildElement("time");
                                if (timeSignatureElement)
                                {
                                    TimeSignature timeSignature = TimeSignature();
                                    // print object
                                    timeSignature.print = GetBoolAttribute(timeSignatureElement, "print-object", true);

                                    const char* symbol = timeSignatureElement->Attribute("symbol");
                                    if (symbol)
                                        timeSignature.displayType = TimeSignature::GetDisplayTypeFromString(symbol);

                                    XMLElement* beats = timeSignatureElement->FirstChildElement("beats");
                                    if (beats)
                                        timeSignature.notes = ToInt(beats->GetText());
                                    XMLElement* beatType = timeSignatureElement->FirstChildElement("beat-type");
                                    if (beatType)
                                        timeSignature.noteType = ToInt(beatType->GetText());

                                    for (auto m : currentMeasures) { m->timeSignature = timeSignature; m->showTimeSignature = true; m->CalculateDuration(); }
                                }

                                // clef
                                XMLNode* previousClefElement = attributes->FirstChildElement("clef");
                                if (previousClefElement)
                                {
                                    while (true)
                                    {
                                        if (previousClefElement)
                                        {
                                            XMLElement* clefElement = previousClefElement->ToElement();
                                            int clefNumber = GetNumberAttribute(clefElement, "number", 1);

                                            XMLElement* signElement = clefElement->FirstChildElement("sign");
                                            if (signElement)
                                            {
                                                currentMeasures[clefNumber-1]->clef.sign = signElement->GetText();
                                                if (currentMeasures[clefNumber-1]->clef.sign == "TAB")
                                                {
                                                    if (firstMeasure) {
                                                        currentInst->staves[clefNumber-1]->type = Staff::StaffType::Tab;
                                                        isTab = true;
                                                    }
                                                }
                                            }

                                            XMLElement* line = clefElement->FirstChildElement("line");
                                            if (line)
                                                currentMeasures[clefNumber-1]->clef.line = ToInt(line->GetText());

                                            currentMeasures[clefNumber-1]->showClef = true;
                                        }
                                        else // no more clefs
                                        {
                                            break;
                                        }
                                        previousClefElement = previousClefElement->NextSiblingElement("clef");
                                    }
                                }

                                /*// clef
                                XMLElement* clef = attributes->FirstChildElement("clef");
                                if (clef)
                                {
                                    XMLElement* sign = clef->FirstChildElement("sign");
                                    if (sign)
                                    {
                                        currentMeasure->clef.sign = sign->GetText();
                                        if (currentMeasure->clef.sign == "TAB")
                                        {
                                            if (firstMeasure) {
                                                currentInst->staves.push_back(new TabStaff());
                                                isTab = true;
                                            }
                                        }
                                        else
                                        {
                                            if (firstMeasure)
                                                currentInst->staves.push_back(new Staff());
                                        }
                                    }

                                    XMLElement* line = clef->FirstChildElement("line");
                                    if (line)
                                        currentMeasure->clef.line = ToInt(line->GetText());

                                    currentMeasure->showClef = true;
                                }
                                else
                                {
                                    if (firstMeasure)
                                        currentInst->staves.push_back(new Staff());
                                }*/

                                // transpose
                                XMLElement* transposeElement = attributes->FirstChildElement("transpose");
                                if (transposeElement)
                                {
                                    Transpose transpose = Transpose();
                                    // diatonic
                                    XMLElement* diatonic = transposeElement->FirstChildElement("diatonic");
                                    if (diatonic)
                                    {
                                        transpose.diatonic = ToInt(diatonic->GetText());
                                    }

                                    // chromatic
                                    XMLElement* chromatic = transposeElement->FirstChildElement("chromatic");
                                    if (chromatic)
                                    {
                                        transpose.chromatic = ToInt(chromatic->GetText());
                                    }

                                    // octave change
                                    XMLElement* octaveChange = transposeElement->FirstChildElement("octave-change");
                                    if (octaveChange)
                                    {
                                        transpose.octaveChange = ToInt(octaveChange->GetText());
                                    }

                                    for (auto m : currentMeasures) { m->transpose = transpose; }
                                }

                                // staff details
                                XMLElement* staffDetails = attributes->FirstChildElement("staff-details");
                                if (staffDetails)
                                {
                                    // staff lines
                                    XMLElement* staffLines = staffDetails->FirstChildElement("staff-lines");
                                    if (staffLines)
                                        currentInst->staves.back()->lines = ToInt(staffLines->GetText());

                                    // staff tunings (TAB only)
                                    if (currentInst->staves.back()->type == Staff::StaffType::Tab) {
                                        XMLNode *previousStaffTuning = staffDetails->FirstChildElement("staff-tuning");

                                        Staff* tabStaff = currentInst->staves.back();

                                        while (true) {
                                            if (previousStaffTuning) {
                                                XMLElement *staffTuning = previousStaffTuning->ToElement();
                                                XMLElement *tuningStep = staffTuning->FirstChildElement(
                                                        "tuning-step");
                                                StaffTuning tuning;
                                                if (tuningStep)
                                                    tuning.pitch.step = tuningStep->GetText();
                                                XMLElement *tuningOctave = staffTuning->FirstChildElement(
                                                        "tuning-octave");
                                                if (tuningOctave)
                                                    tuning.pitch.octave = ToInt(
                                                            tuningOctave->GetText());
                                                XMLElement *tuningAlter = staffTuning->FirstChildElement(
                                                        "tuning-alter");
                                                if (tuningAlter)
                                                    tuning.pitch.alter = ToInt(
                                                            tuningAlter->GetText());
                                                tabStaff->tunings.push_back(tuning);
                                            } else {
                                                break;
                                            }
                                            previousStaffTuning = previousStaffTuning->NextSiblingElement("staff-tuning");
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (firstMeasure)
                                    currentInst->staves.push_back(new Staff());

                                if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                                {
                                    int i = 0;
                                    for (auto m : currentMeasures)
                                    {
                                        m->timeSignature = previousMeasures[i]->timeSignature;
                                        m->CalculateDuration();
                                        m->keySignature = previousMeasures[i]->keySignature;
                                        m->clef = previousMeasures[i]->clef;
                                        m->divisions = previousMeasures[i]->divisions;
                                        m->transpose = previousMeasures[i]->transpose;
                                        i++;
                                    }
                                }
                            }

                            // notes and backup and forward elements

                            XMLNode* previousElement = measure->FirstChildElement(); // the first element
                            //XMLNode* previousNoteElement = measure->FirstChildElement("note");
                            Note* previousNote = nullptr;
                            while (true)
                            {
                                if (previousElement)
                                {
                                    XMLElement* element = previousElement->ToElement();
                                    const char* value = element->Value();
                                    if (strcmp(value, "note") == 0) // note
                                    {
                                        Note* currentNote = new Note();
                                        ParseNoteElement(element, currentTimeInMeasure, isTab, currentNote, previousNote, currentMeasures, measureNumber, error);
                                        previousNote = currentNote;
                                    }
                                    else if (strcmp(value, "backup") == 0) // backup time in measure
                                    {
                                        // duration
                                        XMLElement* durationElement = element->FirstChildElement("duration");
                                        if (durationElement)
                                        {
                                            int divisions = currentMeasures[0]->divisions;
                                            float duration = 0.0f;
                                            if (divisions != 0)
                                            {
                                                duration = (1.0f / (float)divisions) * (float)ToInt(durationElement->GetText());
                                            }
                                            else
                                            {
                                                error = "Error divisions is zero";
                                            }

                                            currentTimeInMeasure -= duration;
                                        }
                                        else
                                        {
                                            error = "backup element has no duration";
                                        }
                                    }
                                    else if (strcmp(value, "harmony") == 0) // harmony element (i.e. chords)
                                    {
                                        ParseHarmonyElement(element, currentTimeInMeasure, currentMeasures);
                                    }
                                    else if (strcmp(value, "forward") == 0) // increment time in measure
                                    {
                                        // duration
                                        XMLElement* durationElement = element->FirstChildElement("duration");
                                        if (durationElement)
                                        {
                                            int divisions = currentMeasures[0]->divisions;
                                            float duration = 0.0f;
                                            if (divisions != 0)
                                            {
                                                duration = (1.0f / (float)divisions) * (float)ToInt(durationElement->GetText());
                                            }
                                            else
                                            {
                                                error = "Error divisions is zero";
                                            }

                                            currentTimeInMeasure += duration;
                                        }
                                        else
                                        {
                                            error = "backup element has no duration";
                                        }
                                    }
                                    else if (strcmp(value, "direction") == 0) // direction
                                    {
                                        // direction
                                        Direction direction = ParseDirection(element);
                                        direction.beatPosition = currentTimeInMeasure;
                                        currentMeasures[0]->directions.push_back(direction);

                                        // sound
                                        XMLElement* soundElement = element->FirstChildElement("sound");
                                        if (soundElement)
                                        {
                                            SoundEvent event = SoundEvent();

                                            event.dynamics = GetFloatAttribute(soundElement, "dynamics", -1.0f);
                                            event.tempo = GetFloatAttribute(soundElement, "tempo", -1.0f);
                                            event.pan = GetNumberAttribute(soundElement, "pan", event.pan);

                                            // getting beat position of sound event
                                            event.beatPosition = currentTimeInMeasure;

                                            currentMeasures[0]->soundEvents.push_back(event);
                                            for (auto m : currentMeasures) { m->soundEvents.push_back(event); }
                                        }
                                    }
                                }
                                else
                                {
                                    break;
                                }
                                previousElement = previousElement->NextSibling(); // get the next element
                                //previousNoteElement = measure->IterateChildren("note", previousNoteElement);
                            }

                            // adding measure to staff
                            /*LOGD("adding measures");
                            int staffIndex = 0;
                            for (Staff* staff : currentInst->staves)
                            {
                                LOGD("staff %i", staffIndex);
                                Measure* newMeasure = new Measure();
                                newMeasure->staff = staffIndex + 1;
                                newMeasure->CopyData(currentMeasure);
                                LOGD("measure %i, %i", newMeasure->notes.size(), currentMeasure->notes.size());
                                currentInst->staves[staffIndex]->measures.push_back(newMeasure);
                                staffIndex++;
                            }
                            LOGD("done adding meausres");
                            if (previousMeasure != nullptr) {
                                LOGD("deleting previous measure");
                                previousMeasure->notes.clear();
                                delete previousMeasure; // deleting previous measure (but not it's notes because they were all given to other measures)
                                previousMeasure = nullptr;
                            }
                            LOGD("setting previous measure");*/

                            // adding measures to correct staves
                            for (Measure* m : currentMeasures)
                            {
                                currentInst->staves[m->staff-1]->measures.push_back(m);
                            }

                            previousMeasures = currentMeasures;
                            firstMeasure = false;
                        }
                        else
                        {
                            /*if (previousMeasure != nullptr) // delete
                            {
                                LOGD("deleting last previous measure");
                                previousMeasure->notes.clear();
                                delete previousMeasure; // deleting previous measure (but not it's notes because they were all given to other measures)
                                previousMeasure = nullptr;
                                LOGD("done deleting last previous measure");
                            }*/
                            break;
                        }
                        previousMeasureElement = previousMeasureElement->NextSiblingElement("measure");
                    }

                }
                else
                {
                    break;
                }

                previous = previous->NextSiblingElement("part");
            }
        }
        else
        {
            error = "ERROR no root";
        }
    }

    LOGD("Finnished");
    doc.Clear();

    return song;
}