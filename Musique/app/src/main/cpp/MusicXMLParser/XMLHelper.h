#ifndef MUSIQUE_XMLHELPER_H
#define MUSIQUE_XMLHELPER_H

#include <string>
#include "../libs/tinyxml2/tinyxml2.h"

using namespace tinyxml2;

/**
 * A 'singleton' that houses helper functions for parsing xml using tinxml2.
 */
class XMLHelper
{
public:

    // ---- Get Value Functions ----

    static std::string GetStringValue(XMLElement* element, std::string defaultValue = "", bool required = false);
    static std::string GetStringValue(const std::string& elementName, XMLElement* elementParent, std::string defaultValue = "", bool required = false);
    static float GetFloatValue(XMLElement* element, float defaultValue = 0.0f, bool required = false);
    static float GetFloatValue(const std::string& elementName, XMLElement* elementParent, float defaultValue = 0.0f, bool required = false);
    static int GetIntValue(XMLElement* element, int defaultValue = 0, bool required = false);
    static int GetIntValue(const std::string& elementName, XMLElement* elementParent, int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(XMLElement* element, unsigned int defaultValue = 0, bool required = false);
    static unsigned int GetUnsignedIntValue(const std::string& elementName, XMLElement* elementParent, unsigned int defaultValue = 0, bool required = false);
};

#endif // MUSIQUE_XMLHELPER_H