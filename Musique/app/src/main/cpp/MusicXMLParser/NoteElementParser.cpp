#include "NoteElementParser.h"

#include "MusicXMLHelper.h"
#include "XMLHelper.h"
#include "BaseElementParser.h"
#include "../Utils/Converters.h"

Lyric NoteElementParser::ParseLyric(XMLElement* lyricElement)
{
    Lyric lyric = Lyric();
    if (lyricElement)
    {
        lyric.defX = XMLHelper::GetFloatAttribute(lyricElement, "default-x", lyric.defX);
        lyric.defY = XMLHelper::GetFloatAttribute(lyricElement, "default-y", lyric.defY);

        lyric.relX = XMLHelper::GetFloatAttribute(lyricElement, "relative-x", lyric.relX);
        lyric.relY = XMLHelper::GetFloatAttribute(lyricElement, "relative-y", lyric.relY);

        lyric.number = XMLHelper::GetNumberAttribute(lyricElement, "number", lyric.number);

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
                    lyric.syllabics.push_back(MusicXMLHelper::FromStringToSyllabicType(syllabicElement->GetText())); // parse type and add to syllabic vector
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
                    AddError("Unrecognized element", "Didn't recognize element in LYRIC");
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

void NoteElementParser::ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, std::vector<bool> staffIsTabInfo, Note* currentNote, Note* previousNote, std::vector<Measure*> currentMeasures, int measureNumber, std::string& error)
{
    currentNote->defX = XMLHelper::GetFloatAttribute(noteElement, "default-x", currentNote->defX);
    currentNote->defY = XMLHelper::GetFloatAttribute(noteElement, "default-y", currentNote->defY);

    currentNote->relX = XMLHelper::GetFloatAttribute(noteElement, "relative-x", currentNote->relX);
    currentNote->relY = XMLHelper::GetFloatAttribute(noteElement, "relative-y", currentNote->relY);

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

    if (staffIsTabInfo[currentNote->staff-1]) {
        currentNote->type = Note::NoteType::Tab;
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
        currentNote->isFullMeasureRest = XMLHelper::GetBoolAttribute(restElement, "measure", false);
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
        std::string typeString = XMLHelper::GetStringAttribute(tieElement, "type", "", true);

        if (typeString == "start")
            currentNote->tie.type = NoteTie::TieType::Start;
        else if (typeString == "stop")
            currentNote->tie.type = NoteTie::TieType::Stop;
        else
            currentNote->tie.type = NoteTie::TieType::None;
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

        currentNote->accidental.isCautionary = XMLHelper::GetBoolAttribute(accidentalElement, "cautionary", false);
        currentNote->accidental.isEditorial = XMLHelper::GetBoolAttribute(accidentalElement, "editorial", false);
        currentNote->accidental.hasBrackets = XMLHelper::GetBoolAttribute(accidentalElement, "bracket", false);
        currentNote->accidental.hasParentheses = XMLHelper::GetBoolAttribute(accidentalElement, "parentheses", false);
    }

    // stem
    XMLElement* stemElement = noteElement->FirstChildElement("stem");
    if (stemElement)
    {
        currentNote->noteStem.stemType = currentNote->noteStem.CalculateStemTypeFromString(stemElement->GetText());
    }

    // beam
    XMLNode* previousBeamElement = noteElement->FirstChildElement("beam");
    while (true)
    {
        if (previousBeamElement) {
            XMLElement* beamElement = previousBeamElement->ToElement();
            if (beamElement)
            {
                NoteBeamData noteBeamData = NoteBeamData();

                noteBeamData.beamType = NoteBeamData::CalculateBeamTypeFromString(beamElement->GetText());
                noteBeamData.beamLevel = XMLHelper::GetNumberAttribute(beamElement, "number", 1);

                currentNote->beamData.push_back(noteBeamData);
            }
        }
        else
        {
            break;
        }

        previousBeamElement = previousBeamElement->NextSiblingElement("beam");
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
            slur.id = XMLHelper::GetNumberAttribute(slurElement, "number");
            slur.placement = MusicXMLHelper::GetAboveBelowAttribute(slurElement, "placement");
            slur.type = MusicXMLHelper::GetStartStopAttribute(slurElement, "type");
            currentNote->slurs.push_back(slur);
        }

        // technical
        XMLElement* technical = notations->FirstChildElement("technical");
        ParseTechnicalElement(technical, currentNote, staffIsTabInfo[currentNote->staff-1]);
    }

    // dot
    XMLElement* dotElement = noteElement->FirstChildElement("dot"); // TODO: modify to allow the parsing of multiple dots
    if (dotElement)
    {
        AugmentationDot dot = AugmentationDot();

        BaseElementParser::ParseVisibleElement(dotElement, dot);

        dot.placement = MusicXMLHelper::GetAboveBelowAttribute(dotElement, "placement", dot.placement);

        currentNote->dots.push_back(dot);
    }

    currentNote->measureIndex = measureNumber - 1;
    currentMeasures[currentNote->staff - 1]->notes.push_back(currentNote);
}

void NoteElementParser::ParseTechnicalElement(XMLElement* technicalElement, Note* currentNote, bool isTab)
{
    if (technicalElement)
    {
        // hammer ons
        XMLNode* previousHammerOnElement = technicalElement->FirstChildElement("hammer-on");
        while (true)
        {
            if (previousHammerOnElement) {
                XMLElement* hammerOnElement = previousHammerOnElement->ToElement();
                if (hammerOnElement)
                {
                    TABSlur tabSlur = TABSlur();
                    tabSlur.slurType = TABSlur::SlurType::HammerOn;
                    tabSlur.id = XMLHelper::GetNumberAttribute(hammerOnElement, "number");
                    tabSlur.placement = MusicXMLHelper::GetAboveBelowAttribute(hammerOnElement, "placement");
                    tabSlur.type = MusicXMLHelper::GetStartStopAttribute(hammerOnElement, "type");
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
        XMLNode* previousPullOffElement = technicalElement->FirstChildElement("pull-off");
        while (true)
        {
            if (previousPullOffElement) {
                XMLElement* pullOffElement = previousPullOffElement->ToElement();
                if (pullOffElement)
                {
                    TABSlur tabSlur = TABSlur();
                    tabSlur.slurType = TABSlur::SlurType::PullOff;
                    tabSlur.id = XMLHelper::GetNumberAttribute(pullOffElement, "number");
                    tabSlur.placement = MusicXMLHelper::GetAboveBelowAttribute(pullOffElement, "placement");
                    tabSlur.type = MusicXMLHelper::GetStartStopAttribute(pullOffElement, "type");
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
        if (currentNote->type == Note::NoteType::Tab && isTab) {

            // string
            XMLElement* string = technicalElement->FirstChildElement("string");
            if (string)
            {
                currentNote->string = ToInt(string->GetText());
            }

            // fret
            XMLElement* fret = technicalElement->FirstChildElement("fret");
            if (fret)
            {
                currentNote->fret = ToInt(fret->GetText());
            }
        }
    }
}