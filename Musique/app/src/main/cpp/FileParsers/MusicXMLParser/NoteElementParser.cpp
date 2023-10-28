#include "NoteElementParser.h"

#include "MusicXMLHelper.h"
#include "XMLHelper.h"
#include "BaseElementParser.h"
#include "../../Utils/Converters.h"

#include "MusicXMLParser.h"

#include "../../Exceptions/Exceptions.h"

#include <unordered_map>

static std::unordered_map<int, std::shared_ptr<Tuplet>> currentTuplets;

static std::unordered_map<int, std::shared_ptr<GlissandoSlide>> currentGlissandos;
static std::unordered_map<int, std::shared_ptr<GlissandoSlide>> currentSlides;
static std::unordered_map<int, std::unordered_map<int, std::shared_ptr<NoteTie>>> currentTies;

static std::shared_ptr<NoteChord> currentNoteChord;

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

void NoteElementParser::ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, std::vector<bool> staffIsTabInfo, std::shared_ptr<Note> currentNote, std::shared_ptr<Note> previousNote, std::vector<std::shared_ptr<Measure>> currentMeasures, int measureNumber)
{
    currentNote->defaultPosition.x = XMLHelper::GetFloatAttribute(noteElement, "default-x", currentNote->defaultPosition.x);
    currentNote->defaultPosition.y = XMLHelper::GetFloatAttribute(noteElement, "default-y", currentNote->defaultPosition.y);

    currentNote->relativePosition.x = XMLHelper::GetFloatAttribute(noteElement, "relative-x", currentNote->relativePosition.x);
    currentNote->relativePosition.y = XMLHelper::GetFloatAttribute(noteElement, "relative-y", currentNote->relativePosition.y);

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
        currentNote->type = NoteType::Tab;
    }

    // grace
    XMLElement* graceElement = noteElement->FirstChildElement("grace");
    if (graceElement)
    {
        currentNote->isGraceNote = true;
        currentNote->noteSize = NoteSize::Grace;

        currentNote->hasSlash = XMLHelper::GetBoolAttribute(graceElement, "slash", currentNote->hasSlash);
    }

    // chord
    XMLElement* chordElement = noteElement->FirstChildElement("chord");
    if (chordElement)
    {
        currentNote->isChord = true;

        if (!previousNote->isChord)
        {
            currentNoteChord = std::make_shared<NoteChord>();

            previousNote->isChord = true;
            currentNoteChord->m_notes.push_back(previousNote); // push the previous note to the note chord
            //currentMeasures[currentNote->staff - 1]->notes.pop_back(); // pop it from the measure's notes vector

            currentNoteChord->m_notes.push_back(currentNote);

            currentNoteChord->m_measureIndex = measureNumber - 1;
            currentMeasures[currentNote->staff - 1]->noteChords.push_back(currentNoteChord);
        }
        else
        {
            currentNoteChord->m_notes.push_back(currentNote);
        }
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
            currentNote->pitch.step = DiatonicNoteFromString(step->GetText());
        }
        else
        {
        }

        // octave
        XMLElement* octave = pitch->FirstChildElement("octave");
        if (step)
        {
            currentNote->pitch.octave = ToInt(octave->GetText());
        }
        else
        {
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
    else if (currentNote->isGraceNote)
    {
        currentNote->beatPosition = currentTimeInMeasure;
        currentNote->duration.duration = 0.0f;
    }

    /*else
    {
        error = "note has no duration";
    }*/

    // tie
    /*XMLNode* previousTieElement = noteElement->FirstChildElement("tie");
    while (true)
    {
        if (previousTieElement)
        {
            XMLElement* tieElement = previousTieElement->ToElement();

            int number = XMLHelper::GetNumberAttribute(tieElement, "number", 1);
            std::string typeString = XMLHelper::GetStringAttribute(tieElement, "type", "", true);

            if (typeString == "start")
            {
                std::shared_ptr<NoteTie> newTie = std::make_shared<NoteTie>();


            }
            else if (typeString == "stop")
            {
            }
            else
                LOGE("Invalid value in tie element.");
        }
        else
        {
            break;
        }
        previousTieElement = previousTieElement->NextSiblingElement("tie");
    }*/

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
        currentNote->accidental = std::make_shared<Accidental>();

        currentNote->accidental->accidentalType = Accidental::CalculateAccidentalTypeFromString(accidentalElement->GetText());

        currentNote->accidental->isCautionary = XMLHelper::GetBoolAttribute(accidentalElement, "cautionary", false);
        currentNote->accidental->isEditorial = XMLHelper::GetBoolAttribute(accidentalElement, "editorial", false);
        currentNote->accidental->hasBrackets = XMLHelper::GetBoolAttribute(accidentalElement, "bracket", false);
        currentNote->accidental->hasParentheses = XMLHelper::GetBoolAttribute(accidentalElement, "parentheses", false);
    }

    // stem
    XMLElement* stemElement = noteElement->FirstChildElement("stem");
    if (stemElement)
    {
        currentNote->noteStem.stemType = currentNote->noteStem.CalculateStemTypeFromString(stemElement->GetText());
    }

    // notehead
    XMLElement* noteHeadElement = noteElement->FirstChildElement("notehead");
    currentNote->noteHead = ParseNoteHeadElement(noteHeadElement);

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
            MusicXMLParser::ParseSlurElement(slurElement, currentMeasures[currentNote->staff-1], currentNote);
        }

        // ties
        XMLNode* previousTiedElement = notations->FirstChildElement("tied");
        while (true)
        {
            if (previousTiedElement)
            {
                ParseTiedElement(previousTiedElement->ToElement(), currentNote);
            }
            else
            {
                break;
            }
            previousTiedElement = previousTiedElement->NextSiblingElement("tied");
        }

        // technical
        XMLElement* technicalElement = notations->FirstChildElement("technical");
        ParseTechnicalElement(technicalElement, currentNote, staffIsTabInfo[currentNote->staff-1]);
        ParseArticulationsElement(technicalElement, currentNote);

        // articulations
        XMLElement* articulationsElement = notations->FirstChildElement("articulations");
        ParseArticulationsElement(articulationsElement, currentNote);

        // ornaments
        XMLElement* ornamentsElement = notations->FirstChildElement("ornaments");
        ParseOrnamentsElement(ornamentsElement, currentNote);

        XMLNode* previousElement = notations->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();

                if (strcmp(value, "tuplet") == 0) // tuplets
                {
                    // tuplets
                    XMLElement* tupletElement = element;
                    if (tupletElement)
                    {
                        StartStopType startStopType = MusicXMLHelper::GetStartStopAttribute(tupletElement, "type", StartStopType::None, true);
                        int number = XMLHelper::GetNumberAttribute(tupletElement, "number", 1);

                        LOGD("number: %d, is start: %d, size: %d, mSize: %d, staff: %d", number, startStopType == StartStopType::Start, currentTuplets.size(), currentMeasures.size(), currentNote->staff);

                        if (startStopType == StartStopType::Start)
                        {
                            std::shared_ptr<Tuplet> tuplet = std::make_shared<Tuplet>();

                            tuplet->notes.push_back(currentNote);

                            tuplet->hasBracket = XMLHelper::GetBoolAttribute(tupletElement, "bracket", tuplet->hasBracket);

                            currentMeasures[currentNote->staff - 1]->tuplets.push_back(tuplet);
                            currentTuplets[number] = tuplet;
                        }
                        else if (startStopType == StartStopType::Stop)
                        {
                            std::shared_ptr<Tuplet> tuplet = currentTuplets[number];

                            tuplet->notes.push_back(currentNote);

                            currentTuplets.erase(number);
                        }
                    }
                }
                else if (strcmp(value, "fermata") == 0) // fermatas
                {
                    std::shared_ptr<Fermata> fermata = std::make_shared<Fermata>();
                    ParseFermataElement(element, fermata);
                    currentNote->fermata = fermata;
                }
                else if (strcmp(value, "glissando") == 0 || strcmp(value, "slide") == 0) // glissandos and slides
                {
                    ParseGlissandoSlideElement(element, currentNote);
                }
                else if (strcmp(value, "arpeggiate") == 0 ) // arpeggios
                {
                    MusicXMLParser::ParseArpeggioElement(element, currentMeasures[currentNote->staff - 1], currentNote);
                }
            }
            else
            {
                break;
            }

            previousElement = previousElement->NextSiblingElement();
        }
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
    if (!currentNote->isChord) // only add the note if it is not in a note chord
    {
        currentMeasures[currentNote->staff - 1]->notes.push_back(currentNote);
    }
}

void NoteElementParser::ParseTechnicalElement(XMLElement* technicalElement, std::shared_ptr<Note> currentNote, bool isTab)
{
    if (technicalElement)
    {
        XMLNode* previousElement = technicalElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                std::shared_ptr<Technique> newTechnique;
                XMLElement* element = previousElement->ToElement();

                const char* value = element->Value();
                if (strcmp(value, "up-bow") == 0 || strcmp(value, "down-bow") == 0) // bowing
                {
                    std::shared_ptr<Bowing> newBowing = std::make_shared<Bowing>();
                    ParseBowingElement(element, newBowing);
                    newTechnique = newBowing;
                }
                else if (strcmp(value, "tap") == 0 || strcmp(value, "golpe") == 0) // guitar technique
                {
                    std::shared_ptr<GuitarTechnique> newGuitarTechnique = std::make_shared<GuitarTechnique>();
                    ParseGuitarTechnique(element, newGuitarTechnique);
                    newTechnique = newGuitarTechnique;
                }
                else if (strcmp(value, "bend") == 0) // bend
                {
                    std::shared_ptr<Bend> newBend = std::make_shared<Bend>();
                    ParseBendElement(element, newBend);
                    newTechnique = newBend;
                }
                else
                {
                    AddError("Unrecognized element", "Didn't recognize element in TECHNICAL");
                }

                if (newTechnique != nullptr)
                {
                    newTechnique->placement = MusicXMLHelper::GetAboveBelowAttribute(element, "placement", newTechnique->placement);
                    currentNote->techniques.push_back(newTechnique);
                }
            }
            else
            {
                break;
            }

            previousElement = previousElement->NextSiblingElement();
        }
    }

    if (technicalElement)
    {
        // TODO: hammer-ons and pull-offs are commented out since currently MuseScore doesn't export those elements

        // hammer ons
        /*XMLNode* previousHammerOnElement = technicalElement->FirstChildElement("hammer-on");
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
                    //tabSlur.type = MusicXMLHelper::GetStartStopAttribute(hammerOnElement, "type");
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
                    //tabSlur.type = MusicXMLHelper::GetStartStopAttribute(pullOffElement, "type");
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
        }*/

        // - TAB only -
        if (currentNote->type == NoteType::Tab && isTab) {

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

void NoteElementParser::ParseArticulationsElement(XMLElement* articulationsElement, std::shared_ptr<Note> currentNote)
{
    if (articulationsElement)
    {
        XMLNode* previousElement = articulationsElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                std::shared_ptr<Articulation> newArticulation;
                XMLElement* element = previousElement->ToElement();

                const char* value = element->Value();
                if (strcmp(value, "accent") == 0 || strcmp(value, "strong-accent") == 0 || strcmp(value, "soft-accent") == 0) // accents
                {
                    std::shared_ptr<Accent> newAccent = std::make_shared<Accent>();
                    ParseAccentElement(element, newAccent);
                    newArticulation = newAccent;
                }
                else if (strcmp(value, "staccato") == 0 || strcmp(value, "tenuto") == 0 || strcmp(value, "detached-legato") == 0
                        || strcmp(value, "staccatissimo") == 0 || strcmp(value, "spiccato") == 0) // staccato type articulations
                {
                    std::shared_ptr<StaccatoArticulation> newStacArt = std::make_shared<StaccatoArticulation>();
                    ParseStaccatoArticulationElement(element, newStacArt);
                    newArticulation = newStacArt;
                }
                else if (strcmp(value, "scoop") == 0 || strcmp(value, "plop") == 0 || strcmp(value, "doit") == 0 || strcmp(value, "falloff") == 0) // slides
                {
                    std::shared_ptr<Slide> newSlide = std::make_shared<Slide>();
                    ParseSlideElement(element, newSlide);
                    newArticulation = newSlide;
                }
                else if (strcmp(value, "breath-mark") == 0) // breath mark
                {
                    std::shared_ptr<BreathMark> newBreathMark = std::make_shared<BreathMark>();
                    ParseBreathMarkElement(element, newBreathMark);
                    newArticulation = newBreathMark;
                }
                else if (strcmp(value, "caesura") == 0) // caesura
                {
                    std::shared_ptr<Caesura> newCaesura = std::make_shared<Caesura>();
                    ParseCaesuraElement(element, newCaesura);
                    newArticulation = newCaesura;
                }
                else if (strcmp(value, "stress") == 0 || strcmp(value, "unstress") == 0) // stress
                {
                    std::shared_ptr<Stress> newStress = std::make_shared<Stress>();
                    ParseStressElement(element, newStress);
                    newArticulation = newStress;
                }
                else
                {
                    AddError("Unrecognized element", "Didn't recognize element in ARTICULATIONS");
                }

                if (newArticulation != nullptr)
                {
                    newArticulation->placement = MusicXMLHelper::GetAboveBelowAttribute(element, "placement", newArticulation->placement);
                    currentNote->articulations.push_back(newArticulation);
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

void NoteElementParser::ParseAccentElement(XMLElement* element, std::shared_ptr<Accent> newAccent)
{
    const char* value = element->Value();
    if (strcmp(value, "accent") == 0) // accent
    {
        newAccent->type = Accent::Type::Accent;
    }
    else if (strcmp(value, "strong-accent") == 0) // strong accent
    {
        newAccent->type = Accent::Type::StrongAccent;
    }
    else if (strcmp(value, "soft-accent") == 0) // soft accent
    {
        newAccent->type = Accent::Type::SoftAccent;
    }
}

void NoteElementParser::ParseStaccatoArticulationElement(XMLElement* element, std::shared_ptr<StaccatoArticulation> newArticulation)
{
    const char* value = element->Value();
    if (strcmp(value, "staccato") == 0)
    {
        newArticulation->type = StaccatoArticulation::Type::Staccato;
    }
    else if (strcmp(value, "tenuto") == 0)
    {
        newArticulation->type = StaccatoArticulation::Type::Tenuto;
    }
    else if (strcmp(value, "detached-legato") == 0)
    {
        newArticulation->type = StaccatoArticulation::Type::DetachedLegato;
    }
    else if (strcmp(value, "staccatissimo") == 0)
    {
        newArticulation->type = StaccatoArticulation::Type::Staccatissimo;
    }
    else if (strcmp(value, "spiccato") == 0)
    {
        newArticulation->type = StaccatoArticulation::Type::Spiccato;
    }
}

void NoteElementParser::ParseSlideElement(XMLElement* element, std::shared_ptr<Slide> newSlide)
{
    const char* value = element->Value();
    if (strcmp(value, "scoop") == 0)
    {
        newSlide->type = Slide::Type::Scoop;
    }
    else if (strcmp(value, "plop") == 0)
    {
        newSlide->type = Slide::Type::Plop;
    }
    else if (strcmp(value, "doit") == 0)
    {
        newSlide->type = Slide::Type::Doit;
    }
    else if (strcmp(value, "falloff") == 0)
    {
        newSlide->type = Slide::Type::Falloff;
    }
}

void NoteElementParser::ParseBreathMarkElement(XMLElement* element, std::shared_ptr<BreathMark> newBreathMark)
{
    // TODO: implement
}

void NoteElementParser::ParseCaesuraElement(XMLElement* element, std::shared_ptr<Caesura> newCaesura)
{
    const char* value = element->GetText();
    if (value == nullptr)
    {
        newCaesura->type = Caesura::Type::Normal;
    }
    else if (strcmp(value, "normal") == 0)
    {
        newCaesura->type = Caesura::Type::Normal;
    }
    else if (strcmp(value, "thick") == 0)
    {
        newCaesura->type = Caesura::Type::Thick;
    }
    else if (strcmp(value, "short") == 0)
    {
        newCaesura->type = Caesura::Type::Short;
    }
    else if (strcmp(value, "curved") == 0)
    {
        newCaesura->type = Caesura::Type::Curved;
    }
    else if (strcmp(value, "single") == 0)
    {
        newCaesura->type = Caesura::Type::Single;
    }
}

void NoteElementParser::ParseStressElement(XMLElement* element, std::shared_ptr<Stress> newStress)
{
    const char* value = element->Value();
    if (strcmp(value, "stress") == 0)
    {
        newStress->type = Stress::Type::Stress;
    }
    else if (strcmp(value, "unstress") == 0)
    {
        newStress->type = Stress::Type::Unstress;
    }
}

void NoteElementParser::ParseBowingElement(XMLElement* element, std::shared_ptr<Bowing> newBowing)
{
    const char* value = element->Value();
    if (strcmp(value, "up-bow") == 0)
    {
        newBowing->direction = Bowing::Direction::Up;
    }
    else if (strcmp(value, "down-bow") == 0)
    {
        newBowing->direction = Bowing::Direction::Down;
    }
}

void NoteElementParser::ParseGuitarTechnique(XMLElement* element, std::shared_ptr<GuitarTechnique> newTechnique)
{
    const char* value = element->Value();
    if (strcmp(value, "tap") == 0)
    {
        newTechnique->type = GuitarTechnique::Type::Tap;

        std::string handString = XMLHelper::GetStringAttribute(element, "hand", "right");

        if (handString == "right")
            newTechnique->hand = GuitarTechnique::Hand::Right;
        else if (handString == "left")
            newTechnique->hand = GuitarTechnique::Hand::Left;
        else
            newTechnique->hand = GuitarTechnique::Hand::None;
    }
    else if (strcmp(value, "golpe") == 0)
    {
        newTechnique->type = GuitarTechnique::Type::Golpe;
    }
}

void NoteElementParser::ParseBendElement(XMLElement* element, std::shared_ptr<Bend> newTechnique)
{
    if (newTechnique == nullptr)
        throw IsNullException();

    std::string shapeAttribute = XMLHelper::GetStringAttribute(element, "shape", "");

    if (shapeAttribute == "angled")
        newTechnique->displayType = Bend::DisplayType::Angled;
    else if (shapeAttribute == "curved")
        newTechnique->displayType = Bend::DisplayType::Curved;

    newTechnique->firstBeat = XMLHelper::GetFloatAttribute(element, "first-beat", newTechnique->firstBeat * 100.0f) / 100.0f;
    newTechnique->lastBeat = XMLHelper::GetFloatAttribute(element, "last-beat", newTechnique->lastBeat * 100.0f) / 100.0f;

    XMLElement* bendAlter = element->FirstChildElement("bend-alter");
    if (bendAlter)
    {
        newTechnique->alterSemitones = ToFloat(bendAlter->GetText());
    }

    // bend type
    XMLElement* preBendElement = element->FirstChildElement("pre-bend");
    if (preBendElement)
    {
        newTechnique->bendType = Bend::BendType::PreBend;
    }
    else
    {
        XMLElement* releaseElement = element->FirstChildElement("release");
        if (releaseElement)
        {
            newTechnique->bendType = Bend::BendType::Release;
        }
        else
        {
            newTechnique->bendType = Bend::BendType::Normal;
        }
    }
}

NoteHead NoteElementParser::ParseNoteHeadElement(XMLElement* element)
{
    NoteHead noteHead = NoteHead();

    if (element)
    {
        BaseElementParser::ParseVisibleElement(element, noteHead);

        noteHead.filled = XMLHelper::GetBoolAttribute(element, "filled", noteHead.filled);
        noteHead.hasParentheses = XMLHelper::GetBoolAttribute(element, "parentheses", noteHead.hasParentheses);

        std::string noteHeadValue = XMLHelper::GetStringValue(element, "", true);

        if (noteHeadValue == "normal")
            noteHead.type = NoteHead::NoteHeadType::Normal;
        else if (noteHeadValue == "none")
            noteHead.type = NoteHead::NoteHeadType::NoNoteHead;
        else if (noteHeadValue == "other")
            noteHead.type = NoteHead::NoteHeadType::Other;
        else if (noteHeadValue == "arrow down")
            noteHead.type = NoteHead::NoteHeadType::ArrowDown;
        else if (noteHeadValue == "arrow up")
            noteHead.type = NoteHead::NoteHeadType::ArrowUp;
        else if (noteHeadValue == "back slashed")
            noteHead.type = NoteHead::NoteHeadType::BackSlashed;
        else if (noteHeadValue == "circle dot")
            noteHead.type = NoteHead::NoteHeadType::CircleDot;
        else if (noteHeadValue == "circle-x")
            noteHead.type = NoteHead::NoteHeadType::CircleX;
        else if (noteHeadValue == "circled")
            noteHead.type = NoteHead::NoteHeadType::Circled;
        else if (noteHeadValue == "cluster")
            noteHead.type = NoteHead::NoteHeadType::Cluster;
        else if (noteHeadValue == "cross")
            noteHead.type = NoteHead::NoteHeadType::Cross;
        else if (noteHeadValue == "diamond")
            noteHead.type = NoteHead::NoteHeadType::Diamond;
        else if (noteHeadValue == "inverted triangle")
            noteHead.type = NoteHead::NoteHeadType::InvertedTriangle;
        else if (noteHeadValue == "left triangle")
            noteHead.type = NoteHead::NoteHeadType::LeftTriangle;
        else if (noteHeadValue == "rectangle")
            noteHead.type = NoteHead::NoteHeadType::Rectangle;
        else if (noteHeadValue == "slash")
            noteHead.type = NoteHead::NoteHeadType::Slash;
        else if (noteHeadValue == "slashed")
            noteHead.type = NoteHead::NoteHeadType::Slashed;
        else if (noteHeadValue == "square")
            noteHead.type = NoteHead::NoteHeadType::Square;
        else if (noteHeadValue == "triangle")
            noteHead.type = NoteHead::NoteHeadType::Triangle;
        else if (noteHeadValue == "x")
            noteHead.type = NoteHead::NoteHeadType::X;
        else if (noteHeadValue == "do")
            noteHead.type = NoteHead::NoteHeadType::Do;
        else if (noteHeadValue == "re")
            noteHead.type = NoteHead::NoteHeadType::Re;
        else if (noteHeadValue == "mi")
            noteHead.type = NoteHead::NoteHeadType::Mi;
        else if (noteHeadValue == "fa")
            noteHead.type = NoteHead::NoteHeadType::Fa;
        else if (noteHeadValue == "fa up")
            noteHead.type = NoteHead::NoteHeadType::FaUp;
        else if (noteHeadValue == "so")
            noteHead.type = NoteHead::NoteHeadType::So;
        else if (noteHeadValue == "la")
            noteHead.type = NoteHead::NoteHeadType::La;
        else if (noteHeadValue == "ti")
            noteHead.type = NoteHead::NoteHeadType::Ti;
        else
            noteHead.type = NoteHead::NoteHeadType::None;

    }

    return noteHead;
}

void NoteElementParser::ParseFermataElement(XMLElement* element, std::shared_ptr<Fermata> fermata)
{
    if (!fermata)
        throw IsNullException();

    BaseElementParser::ParseVisibleElement(element, fermata);

    std::string fermataShapeString = XMLHelper::GetStringValue(element, "");

    if (fermataShapeString.empty() || fermataShapeString == "normal")
        fermata->shape = Fermata::Shape::Normal;
    else if (fermataShapeString == "angled")
        fermata->shape = Fermata::Shape::Angled;
    else if (fermataShapeString == "square")
        fermata->shape = Fermata::Shape::Square;
    else if (fermataShapeString == "double-angled")
        fermata->shape = Fermata::Shape::DoubleAngled;
    else if (fermataShapeString == "double-square")
        fermata->shape = Fermata::Shape::DoubleSquare;
    else if (fermataShapeString == "double-dot")
        fermata->shape = Fermata::Shape::DoubleDot;
    else if (fermataShapeString == "half-curve")
        fermata->shape = Fermata::Shape::HalfCurve;
    else if (fermataShapeString == "curlew")
        fermata->shape = Fermata::Shape::Curlew;
    else
        fermata->shape = Fermata::Shape::None;

    std::string fermataDirectionString = XMLHelper::GetStringAttribute(element, "type", "upright");

    if (fermataDirectionString == "upright")
        fermata->direction = Fermata::Direction::Upright;
    else if (fermataDirectionString == "inverted")
        fermata->direction = Fermata::Direction::Inverted;
    else
        fermata->direction = Fermata::Direction::None;
}

void NoteElementParser::ParseOrnamentsElement(XMLElement* element, std::shared_ptr<Note> currentNote)
{
    if (element)
    {
        XMLNode* previousElement = element->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                std::shared_ptr<Ornament> newOrnament;
                XMLElement* childElement = previousElement->ToElement();

                const char* value = childElement->Value();
                if (strcmp(value, "trill-mark") == 0) // trill mark
                {
                    std::shared_ptr<TrillMark> newTrillMark = std::make_shared<TrillMark>();
                    ParseTrillMarkElement(childElement, newTrillMark);
                    newOrnament = newTrillMark;
                }
                else if (strcmp(value, "turn") == 0 || strcmp(value, "inverted-turn") == 0 || strcmp(value, "delayed-turn") == 0 || strcmp(value, "delayed-inverted-turn") == 0 ||
                        strcmp(value, "vertical-turn") == 0 || strcmp(value, "inverted-vertical-turn") == 0) // turns
                {
                    std::shared_ptr<Turn> newTurn = std::make_shared<Turn>();
                    ParseTurnElement(childElement, newTurn);
                    newOrnament = newTurn;
                }
                else if (strcmp(value, "mordent") == 0 || strcmp(value, "inverted-mordent") == 0) // mordent
                {
                    std::shared_ptr<Mordent> newMordent = std::make_shared<Mordent>();
                    ParseMordentElement(childElement, newMordent);
                    newOrnament = newMordent;
                }
                else if (strcmp(value, "tremolo") == 0) // tremolo
                {
                    ParseTremoloElement(childElement, currentNote);
                }
                else
                {
                    AddError("Unrecognized element", "Didn't recognize element in ORNAMENTS");
                }

                if (newOrnament != nullptr)
                {
                    newOrnament->placement = MusicXMLHelper::GetAboveBelowAttribute(childElement, "placement", newOrnament->placement);
                    currentNote->ornaments.push_back(newOrnament);
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

void NoteElementParser::ParseTrillMarkElement(XMLElement* element, std::shared_ptr<TrillMark> newOrnament)
{
    if (!element)
        throw IsNullException();
}

void NoteElementParser::ParseTurnElement(XMLElement* element, std::shared_ptr<Turn> newOrnament)
{
    if (!element)
        throw IsNullException();

    const char* value = element->Value();
    if (strcmp(value, "turn") == 0)
        newOrnament->type = Turn::TurnType::Turn;
    else if (strcmp(value, "delayed-turn") == 0)
        newOrnament->type = Turn::TurnType::DelayedTurn;
    else if (strcmp(value, "inverted-turn") == 0)
        newOrnament->type = Turn::TurnType::InvertedTurn;
    else if (strcmp(value, "delayed-inverted-turn") == 0)
        newOrnament->type = Turn::TurnType::DelayedInvertedTurn;
    else if (strcmp(value, "vertical-turn") == 0)
        newOrnament->type = Turn::TurnType::VerticalTurn;
    else if (strcmp(value, "inverted-vertical-turn") == 0)
        newOrnament->type = Turn::TurnType::InvertedVerticalTurn;

    newOrnament->hasSlash = XMLHelper::GetBoolAttribute(element, "slash", newOrnament->hasSlash);
}

void NoteElementParser::ParseMordentElement(XMLElement* element, std::shared_ptr<Mordent> newOrnament)
{
    if (!element)
        throw IsNullException();

    const char* value = element->Value();
    if (strcmp(value, "mordent") == 0)
        newOrnament->type = Mordent::MordentType::Mordent;
    else if (strcmp(value, "inverted-mordent") == 0)
        newOrnament->type = Mordent::MordentType::InvertedMordent;
}

void NoteElementParser::ParseGlissandoSlideElement(XMLElement* element, std::shared_ptr<Note> currentNote)
{
    if (!element)
        throw IsNullException();

    GlissandoSlide::Type glissSlideType = GlissandoSlide::Type::None;

    const char* value = element->Value();
    if (strcmp(value, "glissando") == 0)
        glissSlideType = GlissandoSlide::Type::Glissando;
    else if (strcmp(value, "slide") == 0)
        glissSlideType = GlissandoSlide::Type::Slide;

    StartStopType startStopType = MusicXMLHelper::GetStartStopAttribute(element, "type", StartStopType::None, true);
    int number = XMLHelper::GetNumberAttribute(element, "number", 1);

    //LOGD("number: %d, is start: %d, size: %d, mSize: %d, staff: %d", number, startStopType == StartStopType::Start, currentTuplets.size(), currentMeasures.size(), currentNote->staff);

    if (startStopType == StartStopType::Start)
    {
        std::shared_ptr<GlissandoSlide> glissSlide = std::make_shared<GlissandoSlide>();

        glissSlide->type = glissSlideType;
        glissSlide->notes.first = currentNote;

        BaseElementParser::ParseTextualElement(element, glissSlide);
        BaseElementParser::ParseLineElement(element, glissSlide);

        glissSlide->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", glissSlide->defaultPositionStart);
        glissSlide->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", glissSlide->relativePositionStart);

        glissSlide->text = XMLHelper::GetStringValue(element, glissSlide->text);
        if (!glissSlide->text.empty())
            glissSlide->displayText = true;

        currentNote->glissSlides.push_back(glissSlide);

        if (glissSlideType == GlissandoSlide::Type::Glissando)
            currentGlissandos[number] = glissSlide;
        else if (glissSlideType == GlissandoSlide::Type::Slide)
            currentSlides[number] = glissSlide;
    }
    else if (startStopType == StartStopType::Stop)
    {
        std::shared_ptr<GlissandoSlide> glissSlide;

        if (glissSlideType == GlissandoSlide::Type::Glissando)
            glissSlide = currentGlissandos[number];
        else if (glissSlideType == GlissandoSlide::Type::Slide)
            glissSlide = currentSlides[number];

        if (glissSlide != nullptr)
        {
            glissSlide->notes.second = currentNote;
        }

        glissSlide->defaultPositionEnd = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", glissSlide->defaultPositionEnd);
        glissSlide->relativePositionEnd = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", glissSlide->relativePositionEnd);

        currentNote->glissSlides.push_back(glissSlide);

        if (glissSlideType == GlissandoSlide::Type::Glissando)
            currentGlissandos.erase(number);
        else if (glissSlideType == GlissandoSlide::Type::Slide)
            currentSlides.erase(number);
    }
}

void NoteElementParser::ParseTremoloElement(XMLElement* element, std::shared_ptr<Note> currentNote)
{
    if (!element)
        throw IsNullException();

    std::string typeString = XMLHelper::GetStringAttribute(element, "type", "single");

    if (typeString == "single" || typeString == "unmeasured") // single tremolo
    {
        std::shared_ptr<TremoloSingle> newTremoloSingle = std::make_shared<TremoloSingle>();

        BaseElementParser::ParseVisibleElement(element, newTremoloSingle);

        if (typeString == "single")
            newTremoloSingle->type = TremoloSingle::Type::Normal;
        else if (typeString == "unmeasured")
            newTremoloSingle->type = TremoloSingle::Type::BuzzRoll; // or TremoloSingle::Type::Unmeasured

        newTremoloSingle->tremoloMarks = XMLHelper::GetIntValue(element, newTremoloSingle->tremoloMarks, true);

        currentNote->tremoloSingle = newTremoloSingle;
    }
    else if (typeString == "start" || typeString == "stop") // double tremolo
    {
        LOGW("Double Tremolos are not supported yet.");
    }
}

void NoteElementParser::ParseTiedElement(XMLElement* element, std::shared_ptr<Note> currentNote)
{
    if (!element)
        throw IsNullException();

    int number = XMLHelper::GetNumberAttribute(element, "number", -1);
    int pitch = currentNote->pitch.GetPitchValue();
    std::string typeString = XMLHelper::GetStringAttribute(element, "type", "", true);

    if (typeString == "start")
    {
        std::shared_ptr<NoteTie> newTie = std::make_shared<NoteTie>();

        BaseElementParser::ParseVisibleElement(element, newTie);
        BaseElementParser::ParseLineElement(element, newTie);

        newTie->placement = MusicXMLHelper::GetAboveBelowAttribute(element, "placement", newTie->placement);

        CurveOrientation defaultOrientation = newTie->orientation;
        if (newTie->placement == AboveBelowType::Above)
            defaultOrientation = CurveOrientation::Over;
        else if (newTie->placement == AboveBelowType::Below)
            defaultOrientation = CurveOrientation::Under;
        newTie->orientation = MusicXMLHelper::GetCurveOrientationAttribute(element, "orientation", defaultOrientation);

        newTie->notes.first = currentNote;

        currentNote->tie = newTie;

        if (number != -1)
        {
            currentTies[currentNote->staff][number] = newTie;
        }
        else
        {
            currentTies[currentNote->staff][pitch] = newTie;
        }
    }
    else if (typeString == "stop")
    {
        std::shared_ptr<NoteTie> tie;
        if (number != -1)
        {
            tie = currentTies[currentNote->staff][number];
        }
        else
        {
            tie = currentTies[currentNote->staff][pitch];
        }

        if (tie)
        {
            tie->notes.second = currentNote;

            currentNote->tie = tie;
        }

        currentTies.erase(number);
    }
    else if (typeString == "continue")
    {
        LOGW("continue is not handled in tie element");
    }
    else if (typeString == "let-ring")
    {
        LOGW("let-ring is not handled in tie element");
    }
    else
        LOGE("Invalid value in tie element.");
}