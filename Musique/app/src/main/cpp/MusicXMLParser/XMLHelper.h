/**
 * This file defines the `XMLHelper` class which is a helper class for parsing XML files.
 */

#ifndef MUSIQUE_XMLHELPER_H
#define MUSIQUE_XMLHELPER_H

#include <string>
#include "../libs/tinyxml2/tinyxml2.h"
#include "../Vec2.h"
#include "ParseError.h"

using namespace tinyxml2;

/**
 * A 'singleton' that houses helper functions for parsing xml using tinxml2.
 */
class XMLHelper
{
public:

    static bool DoesElementExist(const std::string& elementName, XMLElement* elementParent);

    // ---- Conversions ----

    static bool FromYesNoToBool(const char* string);

    // ---- Get Value Functions ----

    static std::string GetStringValue(XMLElement* element, std::string defaultValue = "", bool required = false);
    static std::string GetStringValue(const std::string& elementName, XMLElement* elementParent, std::string defaultValue = "", bool required = false);
    static float GetFloatValue(XMLElement* element, float defaultValue = 0.0f, bool required = false);
    static float GetFloatValue(const std::string& elementName, XMLElement* elementParent, float defaultValue = 0.0f, bool required = false);
    static int GetIntValue(XMLElement* element, int defaultValue = 0, bool required = false);
    static int GetIntValue(const std::string& elementName, XMLElement* elementParent, int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(XMLElement* element, unsigned int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(const std::string& elementName, XMLElement* elementParent, unsigned int defaultValue = 0, bool required = false);

    // ---- Get Attribute Functions ----

    static bool GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue = false, bool required = false);
    static float GetFloatAttribute(XMLElement* element, const char* s, float defaultValue = 0.0f, bool required = false);
    static std::string GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue = "", bool required = false);
    static int GetNumberAttribute(XMLElement* element, const char* s, int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntAttribute(XMLElement* element, const char* s, unsigned int defaultValue = 0, bool required = false);

    static Vec2<float> GetFloatVec2Attribute(XMLElement* element, const char* s1, const char* s2, Vec2<float> defaultValue = { 0.0f, 0.0f }, bool required = false);

protected:
    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "XMLHelper", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "XMLHelper", errorLevel); }
};

#endif // MUSIQUE_XMLHELPER_H