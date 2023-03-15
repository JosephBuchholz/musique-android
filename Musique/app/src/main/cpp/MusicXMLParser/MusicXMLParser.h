#ifndef MUSIQUE_MUSICXMLPARSER_H
#define MUSIQUE_MUSICXMLPARSER_H

#include <string>
#include <unordered_map>
//#include "libs/tinyxml/tinyxml.h"
//#include "libs/tinyxml/tinystr.h"
#include "../libs/tinyxml2/tinyxml2.h"
#include "../MusicData/Song.h"
#include "../MusicData/Types.h"
//#include "../Utils/Converters.h"

#include "XMLHelper.h"
#include "MusicXMLHelper.h"
#include "ParseError.h"



//#include "libs/tinyxml/tinyxml.h"
//#include "libs/tinyxml/tinystr.h"
/*#include "libs/tinyxml2/tinyxml2.h"
#include "MusicData/Song.h"
#include "MusicData/Types.h"
#include "Utils/Converters.h"*/

using namespace tinyxml2;

/**
 * A 'singleton' that parses musicxml formatted files
 */
class MusicXMLParser
{
    friend class XMLHelper;
public:

    /**
     * Parses a musicxml formatted file (.musicxml)
     *
     * @param data[in] the string data to be parsed
     * @param error[out] will contain an error if there was one
     * @return a pointer to a Song object with the parsed data
     */
    static Song* ParseMusicXML(const std::string& data, std::string& error); // parses MusicXML data

private:

    // ---- Parse Functions ----

    static Lyric::SyllabicType ParseSyllabicType(const std::string& value);
    // parses a single lyric element
    static Lyric ParseLyric(XMLElement* lyricElement);

    static void ParseBaseElement(XMLElement* element, BaseElement& newElement);
    static void ParsePrintableElement(XMLElement* element, PrintableElement& newElement);
    static void ParseVisibleElement(XMLElement* element, VisibleElement& newElement);
    static void ParseTextualElement(XMLElement* element, TextualElement& newElement);
    static void ParseLineElement(XMLElement* element, LineElement& newElement);

    static Rehearsal ParseRehearsal(XMLElement* element);
    static Words ParseWords(XMLElement* element);
    static std::shared_ptr<MetronomeMark> ParseMetronomeMark(XMLElement* element);
    static Dynamic ParseDynamicElement(XMLElement* element);
    static std::shared_ptr<DynamicWedge> ParseDynamicWedgeElement(XMLElement* element, float currentTimeInMeasure);
    static std::shared_ptr<DashesDirection> ParseDashesDirectionElement(XMLElement* element, float currentTimeInMeasure);
    static Direction ParseDirection(XMLElement* directionElement, bool& isNewDirection, float currentTimeInMeasure);
    static void ParseWorkElement(XMLElement* workElement, std::string& workTitle, int& workNumber);
    static void ParseEncodingElement(XMLElement* encodingElement, Song* song);
    static void ParseIdentificationElement(XMLElement* idElement, Song* song);
    static MusicDisplayConstants ParseDefaultsElement(XMLElement* defaultsElement);
    static Credit ParseCreditElement(XMLElement* creditElement);
    static void ParseFrameElement(XMLElement* frameElement, Chord& chord);
    static void ParseHarmonyElement(XMLElement* harmonyElement, float& currentTimeInMeasure, std::vector<Measure*> currentMeasures);
    static void ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, std::vector<bool> staffIsTabInfo, Note* currentNote, Note* previousNote, std::vector<Measure*> currentMeasures, int measureNumber, std::string& error);
    static void ParseTechnicalElement(XMLElement* technicalElement, Note* currentNote, bool isTab);
    static Barline ParseBarlineElement(XMLElement* barlineElement);

protected:
    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "MusicXMLParser", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "MusicXMLParser", errorLevel); }

private:


    //static std::vector<std::shared_ptr<DynamicWedge>> currentDynamicWedges;
    //static int currentDynamicWedgesCount;
};

//std::unordered_map<int, std::shared_ptr<DynamicWedge>> MusicXMLParser::currentDynamicWedges = std::unordered_map<int, std::shared_ptr<DynamicWedge>>();
//int MusicXMLParser::currentDynamicWedgesCount = 0;

#endif // MUSIQUE_MUSICXMLPARSER_H