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

class MusicXMLParser
{
public:

    static Song* ParseMusicXML(const std::string& data, std::string& error); // parses MusicXML data

    // ---- Conversions ----

    static bool FromYesNoToBool(const char* string);
    static Date FromStringToDate(const char* string);

private:
    // ---- Get Value Functions ----

    static std::string GetStringValue(XMLElement* element, std::string defaultValue = "");
    static std::string GetStringValue(std::string elementName, XMLElement* elementParent, std::string defaultValue = "");
    static int GetIntValue(XMLElement* element, int defaultValue = 0);

    // ---- Get Attribute Functions ----

    static bool GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue = false);
    static float GetFloatAttribute(XMLElement* element, const char* s, float defaultValue = 0.0f);
    static std::string GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue = "");
    static int GetNumberAttribute(XMLElement* element, const char* s, int defaultValue = 0);
    static AboveBelowType GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue = AboveBelowType::None);
    static StartStopType GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue = StartStopType::None);
    static FontFamily GetFontFamilyAttribute(XMLElement* element, const char* s, FontFamily defaultValue = FontFamily());
    static FontSize GetFontSizeAttribute(XMLElement* element, const char* s, FontSize defaultValue = FontSize());
    static FontStyle GetFontStyleAttribute(XMLElement* element, const char* s, FontStyle defaultValue = FontStyle::Normal);
    static FontWeight GetFontWeightAttribute(XMLElement* element, const char* s, FontWeight defaultValue = FontWeight::Normal);

    // ---- Parse Functions ----

    static Lyric::SyllabicType ParseSyllabicType(const std::string& value);
    // parses a single lyric element
    static Lyric ParseLyric(XMLElement* lyricElement);
    static void ParseVisibleElement(XMLElement* element, VisibleElement& newElement);
    static void ParseTextualElement(XMLElement* element, TextualElement& newElement);
    static Rehearsal ParseRehearsal(XMLElement* element);
    static Words ParseWords(XMLElement* element);
    static Direction ParseDirection(XMLElement* directionElement);
    static void ParseWorkElement(XMLElement* workElement, std::string& workTitle, int& workNumber);
    static void ParseEncodingElement(XMLElement* encodingElement, Song* song);
    static void ParseIdentificationElement(XMLElement* idElement, Song* song);
    static void ParseDefaultsElement(XMLElement* defaultsElement);
    static void ParseCreditElement(XMLElement* creditElement);
};