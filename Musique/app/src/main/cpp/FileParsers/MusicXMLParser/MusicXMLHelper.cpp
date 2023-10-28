#include "MusicXMLHelper.h"

#include "XMLHelper.h"
#include "../../Utils/Converters.h"

#include "../../Exceptions/Exceptions.h"

void MusicXMLHelper::FlipYInVec2(Vec2<float>& v)
{
    if (v.y != INVALID_FLOAT_VALUE)
        v.y = -v.y;
}

// ---- Conversions ----

Date MusicXMLHelper::FromStringToDate(const char* string)
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

Lyric::SyllabicType MusicXMLHelper::FromStringToSyllabicType(const std::string& value)
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

BracketDirection::LineEndType MusicXMLHelper::GetLineEndTypeFromString(const std::string& string)
{
    if (string == "up")
        return BracketDirection::LineEndType::Up;
    else if (string == "down")
        return BracketDirection::LineEndType::Down;
    else if (string == "both")
        return BracketDirection::LineEndType::Both;
    else if (string == "arrow")
        return BracketDirection::LineEndType::Arrow;
    else if (string == "none")
        return BracketDirection::LineEndType::NoEnd;
    else if (string.empty())
        return BracketDirection::LineEndType::None;
    else
        AddError("Did not recognize type", "Did not recognize line-end type");
}

// ---- From String ----

Chord::HarmonyType MusicXMLHelper::GetHarmonyTypeFromString(const std::string& string)
{
    if (string == "none")
        return Chord::HarmonyType::NoHarmony;
    else if (string == "other")
        return Chord::HarmonyType::Other;
    else if (string == "augmented")
        return Chord::HarmonyType::Augmented;
    else if (string == "augmented-seventh")
        return Chord::HarmonyType::AugmentedSeventh;
    else if (string == "diminished")
        return Chord::HarmonyType::Diminished;
    else if (string == "diminished-seventh")
        return Chord::HarmonyType::DiminishedSeventh;
    else if (string == "dominant")
        return Chord::HarmonyType::Dominant;
    else if (string == "dominant-11th")
        return Chord::HarmonyType::Dominant11th;
    else if (string == "dominant-13th")
        return Chord::HarmonyType::Dominant13th;
    else if (string == "dominant-ninth")
        return Chord::HarmonyType::DominantNinth;
    else if (string == "French")
        return Chord::HarmonyType::French;
    else if (string == "German")
        return Chord::HarmonyType::German;
    else if (string == "half-diminished")
        return Chord::HarmonyType::HalfDiminished;
    else if (string == "Italian")
        return Chord::HarmonyType::Italian;
    else if (string == "major")
        return Chord::HarmonyType::Major;
    else if (string == "major-11th")
        return Chord::HarmonyType::Major11th;
    else if (string == "major-13th")
        return Chord::HarmonyType::Major13th;
    else if (string == "major-minor")
        return Chord::HarmonyType::MajorMinor;
    else if (string == "major-ninth")
        return Chord::HarmonyType::MajorNinth;
    else if (string == "major-seventh")
        return Chord::HarmonyType::MajorSeventh;
    else if (string == "major-sixth")
        return Chord::HarmonyType::MajorSixth;
    else if (string == "minor")
        return Chord::HarmonyType::Minor;
    else if (string == "minor-11th")
        return Chord::HarmonyType::Minor11th;
    else if (string == "minor-13th")
        return Chord::HarmonyType::Minor13th;
    else if (string == "minor-ninth")
        return Chord::HarmonyType::MinorNinth;
    else if (string == "minor-seventh")
        return Chord::HarmonyType::MinorSeventh;
    else if (string == "minor-sixth")
        return Chord::HarmonyType::MinorSixth;
    else if (string == "Neapolitan")
        return Chord::HarmonyType::Neapolitan;
    else if (string == "pedal")
        return Chord::HarmonyType::Pedal;
    else if (string == "power")
        return Chord::HarmonyType::Power;
    else if (string == "suspended-fourth")
        return Chord::HarmonyType::SuspendedFourth;
    else if (string == "suspended-second")
        return Chord::HarmonyType::SuspendedSecond;
    else if (string == "Tristan")
        return Chord::HarmonyType::Tristan;
    else
        ; // TODO: error: this is not possible
    return Chord::HarmonyType::None;
}

NoteValue MusicXMLHelper::GetNoteValueTypeFromString(const std::string& string)
{
    if (string == "1024th")
        return NoteValue::_1024th;
    else if (string == "512th")
        return NoteValue::_512th;
    else if (string == "256th")
        return NoteValue::_256th;
    else if (string == "128th")
        return NoteValue::_128th;
    else if (string == "64th")
        return NoteValue::_64th;
    else if (string == "32nd")
        return NoteValue::ThirtySecond;
    else if (string == "16th")
        return NoteValue::Sixteenth;
    else if (string == "eighth")
        return NoteValue::Eighth;
    else if (string == "quarter")
        return NoteValue::Quarter;
    else if (string == "half")
        return NoteValue::Half;
    else if (string == "whole")
        return NoteValue::Whole;
    else if (string == "breve")
        return NoteValue::Breve;
    else if (string == "long")
        return NoteValue::Long;
    else if (string == "maxima")
        return NoteValue::Maxima;
    else
        ; // TODO: error: this is not possible
    return NoteValue::None;
}

// ---- Get Value Functions ----

StartStopType MusicXMLHelper::GetStartStopValue(XMLElement* element, StartStopType defaultValue, bool required)
{
    if (element) {
        const char* c = element->GetText();
        if (strcmp(c, "start") == 0) {
            return StartStopType::Start;
        } else if (strcmp(c, "stop") == 0) {
            return StartStopType::Stop;
        } else if (strcmp(c, "continue") == 0) {
            return StartStopType::Continue;
        }
    }

    AddErrorIf(required, "Required parse value error", "Failed to get value when required");
    return defaultValue;
}

StartStopType MusicXMLHelper::GetStartStopValue(const std::string& elementName, XMLElement* elementParent, StartStopType defaultValue, bool required)
{
    if (!elementParent)
        throw IsNullException("'elementParent' in MusicXMLHelper::GetStartStopValue is null");

    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());

    if (element)
    {
        const char* c = element->GetText();

        if (c)
        {
            if (strcmp(c, "start") == 0)
                return StartStopType::Start;
            else if (strcmp(c, "stop") == 0)
                return StartStopType::Stop;
            else if (strcmp(c, "continue") == 0)
                return StartStopType::Continue;
        }
    }

    AddErrorIf(required, "Required parse value error", "Failed to get value when required");
    return defaultValue;
}

// ---- Get Attribute Functions ----

AboveBelowType MusicXMLHelper::GetAboveBelowAttribute(XMLElement* element, const char* s, AboveBelowType defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "above") == 0) {
            return AboveBelowType::Above;
        } else if (strcmp(attribute, "below") == 0) {
            return AboveBelowType::Below;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

StartStopType MusicXMLHelper::GetStartStopAttribute(XMLElement* element, const char* s, StartStopType defaultValue, bool required)
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

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

RightLeftType MusicXMLHelper::GetRightLeftAttribute(XMLElement* element, const char* s, RightLeftType defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "right") == 0) {
            return RightLeftType::Right;
        } else if (strcmp(attribute, "left") == 0) {
            return RightLeftType::Left;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

FontFamily MusicXMLHelper::GetFontFamilyAttribute(XMLElement* element, const char* s, FontFamily defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    FontFamily fontFamily = defaultValue;
    if (attribute) {
        fontFamily.fonts.push_back(attribute); // TODO: need to parse out comma sperated values
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return fontFamily;
}

FontSize MusicXMLHelper::GetFontSizeAttribute(XMLElement* element, const char* s, FontSize defaultValue, bool required)
{
    FontSize fontSize = defaultValue;

    fontSize.size = XMLHelper::GetFloatAttribute(element, s, fontSize.size);
    //fontSize.size = ToFloat(value);

    /*if (IsFloat(value)) // is a decimal value
    {
        fontSize.size = ToFloat(value);
    }
    else // is a css value
    {
        fontSize.SetCSSSize(value);
    }*/

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return fontSize;
}

FontStyle MusicXMLHelper::GetFontStyleAttribute(XMLElement* element, const char* s, FontStyle defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "normal") == 0) {
            return FontStyle::Normal;
        } else if (strcmp(attribute, "italic") == 0) {
            return FontStyle::Italic;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

FontWeight MusicXMLHelper::GetFontWeightAttribute(XMLElement* element, const char* s, FontWeight defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "normal") == 0) {
            return FontWeight::Normal;
        } else if (strcmp(attribute, "bold") == 0) {
            return FontWeight::Bold;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

Justify MusicXMLHelper::GetJustifyAttribute(XMLElement* element, const char* s, Justify defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "left") == 0) {
            return Justify::Left;
        } else if (strcmp(attribute, "center") == 0) {
            return Justify::Center;
        } else if (strcmp(attribute, "right") == 0) {
            return Justify::Right;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

CurveOrientation MusicXMLHelper::GetCurveOrientationAttribute(XMLElement* element, const char* s, CurveOrientation defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        if (strcmp(attribute, "over") == 0) {
            return CurveOrientation::Over;
        } else if (strcmp(attribute, "under") == 0) {
            return CurveOrientation::Under;
        }
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}