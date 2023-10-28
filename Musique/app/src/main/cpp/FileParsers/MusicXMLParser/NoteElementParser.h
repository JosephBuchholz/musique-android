/**
 * This file defines the `NoteElementParser` class which parses note elements for MusicXML files.
 */

#ifndef MUSIQUE_NOTEELEMENTPARSER_H
#define MUSIQUE_NOTEELEMENTPARSER_H

#include <string>
#include <memory>

#include "../../libs/tinyxml2/tinyxml2.h"
#include "ParseError.h"

#include "../../MusicData/Measures/Measure.h"
#include "../../MusicData/Notes/Note.h"
#include "../../MusicData/Notes/Lyric.h"

#include "../../MusicData/Notes/Articulations/Accent.h"
#include "../../MusicData/Notes/Articulations/BreathMark.h"
#include "../../MusicData/Notes/Articulations/Caesura.h"
#include "../../MusicData/Notes/Articulations/Slide.h"
#include "../../MusicData/Notes/Articulations/StaccatoArticulation.h"
#include "../../MusicData/Notes/Articulations/Stress.h"

#include "../../MusicData/Notes/Techniques/Bowing.h"
#include "../../MusicData/Notes/Techniques/GuitarTechnique.h"
#include "../../MusicData/Notes/Techniques/Bend.h"

#include "../../MusicData/Notes/Ornaments/Ornaments.h"
#include "../../MusicData/Notes/Ornaments/TrillMark.h"
#include "../../MusicData/Notes/Ornaments/Mordent.h"
#include "../../MusicData/Notes/Ornaments/Vibrato.h"
#include "../../MusicData/Notes/Ornaments/Turn.h"

#include "../../MusicData/Notes/GlissandoSlide.h"

using namespace tinyxml2;

/**
 * A 'singleton' that parses note elements for MusicXML files.
 */
class NoteElementParser
{
public:

    /**
     * Parses a note element in a MusicXML file.
     *
     * @param noteElement A pointer to the `XMLElement` object.
     * @param currentTimeInMeasure The current beat position in the current measure.
     * @param staffIsTabInfo Info on whether a staff is a tablature staff.
     * @param currentNote A pointer to the current note.
     * @param previousNote A pointer to the previous note.
     * @param currentMeasures The current measures.
     * @param measureNumber The measure number.
     * @param error An error string (this should be deleted)
     */
    static void ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, std::vector<bool> staffIsTabInfo, std::shared_ptr<Note> currentNote, std::shared_ptr<Note> previousNote, std::vector<std::shared_ptr<Measure>> currentMeasures, int measureNumber);

private:
    static Lyric ParseLyric(XMLElement* lyricElement);
    static void ParseTechnicalElement(XMLElement* technicalElement, std::shared_ptr<Note> currentNote, bool isTab);

    static void ParseAccentElement(XMLElement* element, std::shared_ptr<Accent> newAccent);
    static void ParseStaccatoArticulationElement(XMLElement* element, std::shared_ptr<StaccatoArticulation> newArticulation);
    static void ParseSlideElement(XMLElement* element, std::shared_ptr<Slide> newSlide);
    static void ParseBreathMarkElement(XMLElement* element, std::shared_ptr<BreathMark> newBreathMark);
    static void ParseCaesuraElement(XMLElement* element, std::shared_ptr<Caesura> newCaesura);
    static void ParseStressElement(XMLElement* element, std::shared_ptr<Stress> newStress);

    static void ParseArticulationsElement(XMLElement* articulationsElement, std::shared_ptr<Note> currentNote);

    static void ParseBowingElement(XMLElement* element, std::shared_ptr<Bowing> newBowing);
    static void ParseGuitarTechnique(XMLElement* element, std::shared_ptr<GuitarTechnique> newTechnique);
    static void ParseBendElement(XMLElement* element, std::shared_ptr<Bend> newTechnique);

    static NoteHead ParseNoteHeadElement(XMLElement* element);

    static void ParseFermataElement(XMLElement* element, std::shared_ptr<Fermata> fermata);

    static void ParseOrnamentsElement(XMLElement* element, std::shared_ptr<Note> currentNote);
    static void ParseTrillMarkElement(XMLElement* element, std::shared_ptr<TrillMark> newOrnament);
    static void ParseTurnElement(XMLElement* element, std::shared_ptr<Turn> newOrnament);
    static void ParseMordentElement(XMLElement* element, std::shared_ptr<Mordent> newOrnament);

    static void ParseGlissandoSlideElement(XMLElement* element, std::shared_ptr<Note> currentNote);

    static void ParseTremoloElement(XMLElement* element, std::shared_ptr<Note> currentNote);
    static void ParseTiedElement(XMLElement* element, std::shared_ptr<Note> currentNote);

protected:
    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "BaseElementParser", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "BaseElementParser", errorLevel); }
};


#endif //MUSIQUE_NOTEELEMENTPARSER_H
