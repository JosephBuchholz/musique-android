/**
 * This file defines the `MusicXMLHelper` class which is a helper class for parsing MusicXML files.
 */

#ifndef MUSIQUE_MUSICXMLHELPER_H
#define MUSIQUE_MUSICXMLHELPER_H

#include <string>
#include "../../libs/tinyxml2/tinyxml2.h"
#include "ParseError.h"
#include "../../MusicData/Types.h"

#include "../../MusicData/Chords/Chord.h"
#include "../../MusicData/Notes/Lyric.h"
#include "../../MusicData/Directions/BracketDirection.h"

#include "../../Collisions/Vec2.h"

using namespace tinyxml2;

/**
 * A 'singleton' that houses helper functions for parsing musicxml using tinxml2.
 */
class MusicXMLHelper
{
public:

    static void FlipYInVec2(Vec2<float>& v);

    // ---- Conversions ----

    static Date FromStringToDate(const char* string);
    static Lyric::SyllabicType FromStringToSyllabicType(const std::string& value);

    // ---- From String ----

    static Chord::HarmonyType GetHarmonyTypeFromString(const std::string& string);
    static NoteValue GetNoteValueTypeFromString(const std::string& string);
    static BracketDirection::LineEndType GetLineEndTypeFromString(const std::string& string);

    // ---- Get Value Functions ----

    static StartStopType GetStartStopValue(XMLElement* element, StartStopType defaultValue = StartStopType::None, bool required = false);
    static StartStopType GetStartStopValue(const std::string& elementName, XMLElement* elementParent, StartStopType defaultValue = StartStopType::None, bool required = false);

    // ---- Get Attribute Functions ----

    static AboveBelowType GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue = AboveBelowType::None, bool required = false);
    static StartStopType GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue = StartStopType::None, bool required = false);
    static RightLeftType GetRightLeftAttribute(XMLElement* element, const char* s, RightLeftType defaultValue = RightLeftType::None, bool required = false);
    static FontFamily GetFontFamilyAttribute(XMLElement* element, const char* s, FontFamily defaultValue = FontFamily(), bool required = false);
    static FontSize GetFontSizeAttribute(XMLElement* element, const char* s, FontSize defaultValue = FontSize(), bool required = false);
    static FontStyle GetFontStyleAttribute(XMLElement* element, const char* s, FontStyle defaultValue = FontStyle::Normal, bool required = false);
    static FontWeight GetFontWeightAttribute(XMLElement* element, const char* s, FontWeight defaultValue = FontWeight::Normal, bool required = false);
    static Justify GetJustifyAttribute(XMLElement* element, const char* s, Justify defaultValue = Justify::None, bool required = false);
    static CurveOrientation GetCurveOrientationAttribute(XMLElement* element, const char* s, CurveOrientation defaultValue = CurveOrientation::None, bool required = false);

protected:
    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "MusicXMLHelper", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "MusicXMLHelper", errorLevel); }
};

#endif //MUSIQUE_MUSICXMLHELPER_H
