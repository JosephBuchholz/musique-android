#pragma once

#include <string>
//#include "libs/tinyxml/tinyxml.h"
//#include "libs/tinyxml/tinystr.h"
#include "../libs/tinyxml2/tinyxml2.h"
#include "../MusicData/Song.h"
#include "../MusicData/Types.h"
//#include "../Utils/Converters.h"


//#include "libs/tinyxml/tinyxml.h"
//#include "libs/tinyxml/tinystr.h"
/*#include "libs/tinyxml2/tinyxml2.h"
#include "MusicData/Song.h"
#include "MusicData/Types.h"
#include "Utils/Converters.h"*/

using namespace tinyxml2;

enum class ErrorLevel {
    None = 0, Error, Warning, Fatal
};

/**
 * Represents a parsing error.
 */
class ParseError
{
public:

    ParseError() {}
    ParseError(std::string title, std::string desc, std::string parentName, ErrorLevel errorLevel)
        : title(title), desc(desc), parentName(parentName), errorLevel(errorLevel) {}

public:

    std::string parentName = "";
    std::string title = "";
    std::string desc = "";

    ErrorLevel errorLevel = ErrorLevel::Error;
};

static std::vector<ParseError> m_Errors = std::vector<ParseError>();

/**
 * A 'singleton' that parses musicxml formatted files
 */
class MusicXMLParser
{
public:

    /**
     * Parses a musicxml formatted file (.musicxml)
     *
     * @param data[in] the string data to be parsed
     * @param error[out] will contain an error if there was one
     * @return a pointer to a Song object with the parsed data
     */
    static Song* ParseMusicXML(const std::string& data, std::string& error); // parses MusicXML data

    // ---- Conversions ----

    static bool FromYesNoToBool(const char* string);
    static Date FromStringToDate(const char* string);

private:
    // ---- From String ----

    static Chord::HarmonyType GetHarmonyTypeFromString(const std::string& string);

    // ---- Get Value Functions ----

    static std::string GetStringValue(XMLElement* element, std::string defaultValue = "", bool required = false);
    static std::string GetStringValue(const std::string& elementName, XMLElement* elementParent, std::string defaultValue = "", bool required = false);
    static float GetFloatValue(XMLElement* element, float defaultValue = 0.0f, bool required = false);
    static float GetFloatValue(const std::string& elementName, XMLElement* elementParent, float defaultValue = 0.0f, bool required = false);
    static int GetIntValue(XMLElement* element, int defaultValue = 0, bool required = false);
    static int GetIntValue(const std::string& elementName, XMLElement* elementParent, int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(XMLElement* element, unsigned int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(const std::string& elementName, XMLElement* elementParent, unsigned int defaultValue = 0, bool required = false);
    static StartStopType GetStartStopValue(XMLElement* element, StartStopType defaultValue = StartStopType::None, bool required = false);
    static StartStopType GetStartStopValue(const std::string& elementName, XMLElement* elementParent, StartStopType defaultValue = StartStopType::None, bool required = false);

    // ---- Get Attribute Functions ----

    static bool GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue = false, bool required = false);
    static float GetFloatAttribute(XMLElement* element, const char* s, float defaultValue = 0.0f, bool required = false);
    static std::string GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue = "", bool required = false);
    static int GetNumberAttribute(XMLElement* element, const char* s, int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntAttribute(XMLElement* element, const char* s, unsigned int defaultValue = 0, bool required = false);
    static AboveBelowType GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue = AboveBelowType::None, bool required = false);
    static StartStopType GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue = StartStopType::None, bool required = false);
    static RightLeftType GetRightLeftAttribute(XMLElement* element, const char* s, RightLeftType defaultValue = RightLeftType::None, bool required = false);
    static FontFamily GetFontFamilyAttribute(XMLElement* element, const char* s, FontFamily defaultValue = FontFamily(), bool required = false);
    static FontSize GetFontSizeAttribute(XMLElement* element, const char* s, FontSize defaultValue = FontSize(), bool required = false);
    static FontStyle GetFontStyleAttribute(XMLElement* element, const char* s, FontStyle defaultValue = FontStyle::Normal, bool required = false);
    static FontWeight GetFontWeightAttribute(XMLElement* element, const char* s, FontWeight defaultValue = FontWeight::Normal, bool required = false);
    static Justify GetJustifyAttribute(XMLElement* element, const char* s, Justify defaultValue = Justify::None, bool required = false);

    // ---- Parse Functions ----

    static Lyric::SyllabicType ParseSyllabicType(const std::string& value);
    // parses a single lyric element
    static Lyric ParseLyric(XMLElement* lyricElement);

    static void ParseBaseElement(XMLElement* element, BaseElement& newElement);
    static void ParsePrintableElement(XMLElement* element, PrintableElement& newElement);
    static void ParseVisibleElement(XMLElement* element, VisibleElement& newElement);
    static void ParseTextualElement(XMLElement* element, TextualElement& newElement);

    static Rehearsal ParseRehearsal(XMLElement* element);
    static Words ParseWords(XMLElement* element);
    static Direction ParseDirection(XMLElement* directionElement);
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

    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "MusicXMLParser", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "MusicXMLParser", errorLevel); }

    static void PrintErrors();
};