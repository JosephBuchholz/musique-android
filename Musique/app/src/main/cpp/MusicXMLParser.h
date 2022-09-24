#pragma once
#include <string>
//#include "libs/tinyxml/tinyxml.h"
//#include "libs/tinyxml/tinystr.h"
#include "libs/tinyxml2/tinyxml2.h"
#include "MusicData/SongData.h"
#include "MusicData/Types.h"
#include "Utils/Converters.h"

using namespace tinyxml2;

bool FromYesNoToBool(const char* string)
{
    if (strcmp(string, "yes") == 0) {
        return true;
    }
    return false;
}

bool GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue = false)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return FromYesNoToBool(attribute);
    }
    return defaultValue;
}

float GetFloatAttribute(XMLElement* element, const char* s, float defaultValue = 0.0f)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToFloat(attribute);
    }
    return defaultValue;
}

std::string GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue = "")
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return attribute;
    }
    return defaultValue;
}

int GetNumberAttribute(XMLElement* element, const char* s, int defaultValue = 0)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToInt(attribute);
    }
    return defaultValue;
}

AboveBelowType GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue = AboveBelowType::None)
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

StartStopType GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue = StartStopType::None)
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

SongData* ParseMusicXML(const std::string& data, std::string& error)
{
    LOGW("STARTING TO PARSE");
    SongData* songData = new SongData();
    LOGW("Created song data");

    /*{
        tinyxml2::XMLDocument doc;
        const char *d = data.c_str();
        tinyxml2::XMLError xmlError = doc.Parse(d);
        LOGW("parced document with tinyxml2");
        if (xmlError != tinyxml2::XMLError::XML_SUCCESS) {
            LOGE("error: %i", xmlError);
        }

        if (doc.Error())
        {
            LOGE("doc error: %s: %s", doc.ErrorName(), doc.ErrorStr());
        }
    }
    LOGW("succsess with tinyxml2");

    {
        TiXmlDocument doc;
        const char *d = data.c_str();
        doc.Parse(d);
        LOGW("parced document with tinyxml1");
    }
    LOGW("succsess with tinyxml1");*/

    //error = "FAILED";

    /*LOGD("going to parce");
    int i = 0;
    while (i < 10)
    {
        tinyxml2::XMLDocument testDoc;
        const char *dat = data.c_str();
        tinyxml2::XMLError e = testDoc.Parse(dat);

        if (e != tinyxml2::XMLError::XML_SUCCESS) {
            LOGE("error: %i", e);
            error = "XMLERRORINTESTs: " + ToString(int(e));
        }
        LOGW("prace %i", i);
        i++;
    }
    LOGD("finnishedparce");*/
    
    tinyxml2::XMLDocument doc;
    const char *d = data.c_str();
    tinyxml2::XMLError xmlError = doc.Parse(d);
    LOGW("parced document with tinyxml2");
    if (xmlError != tinyxml2::XMLError::XML_SUCCESS) {
        LOGE("error: %i", xmlError);
        error = "XMLERROR: " + ToString(int(xmlError));
    }

    /*LOGD("going to parce again");
    i = 0;
    while (i < 10)
    {
        tinyxml2::XMLDocument testDoc;
        const char *dat = data.c_str();
        tinyxml2::XMLError e = testDoc.Parse(dat);

        if (e != tinyxml2::XMLError::XML_SUCCESS) {
            LOGE("error: %i", e);
            error = "XMLERRORINTESTs: " + ToString(int(e));
        }
        LOGW("prace %i", i);
        i++;
    }
    LOGD("finnished parce again");*/

    /*if (doc.Error())
    {
        LOGE("doc error: %s: %s", doc.ErrorName(), doc.ErrorStr());
        error = "XML DOC ERROR";
    }*/



    /*TiXmlDocument doc;
    const char *d = data.c_str();
    doc.Parse(d);*/
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
            songData->musicXMLVersion = root->Attribute("version");

            // work
            XMLElement* work = root->FirstChildElement("work");
            if (work)
            {
                // work title
                XMLElement* workTitle = work->FirstChildElement("work-title");

                if (workTitle)
                {
                    songData->songTitle = workTitle->GetText();
                }
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
                        songData->instruments.push_back(instrument);

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
                    Instrument* currentInst = songData->GetInstrument(id);
                    if (currentInst == nullptr) {
                        error = "Instrument " + id + " does not exist";
                        return songData;
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
                                        XMLElement* noteElement = element;

                                        Note* currentNote = new Note();
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

    return songData;
}