#include "XMLHelper.h"
#include "MusicXMLParser.h"
#include "../Utils/Converters.h"

std::string XMLHelper::GetStringValue(XMLElement* element, std::string defaultValue, bool required)
{
    if (element) {
        return element->GetText();
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get string value when required");
    return defaultValue;
}

std::string XMLHelper::GetStringValue(const std::string& elementName, XMLElement* elementParent, std::string defaultValue, bool required)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetStringValue(element, defaultValue);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get string value when required");
    return defaultValue;
}

float XMLHelper::GetFloatValue(XMLElement* element, float defaultValue, bool required)
{
    if (element) {
        const char* s = element->GetText();
        float value;
        try {
            value = std::stof(s);
        }
        catch (std::exception& e) {
            // TODO: error
            LOGE("NOT A FLOAT");
            return defaultValue;
        }
        return value;
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get float value when required");
    return defaultValue;
}

float XMLHelper::GetFloatValue(const std::string& elementName, XMLElement* elementParent, float defaultValue, bool required)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetFloatValue(element, defaultValue);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get float value when required");
    return defaultValue;
}

int XMLHelper::GetIntValue(XMLElement* element, int defaultValue, bool required)
{
    if (element) {
        std::string c = element->GetText();
        if (IsInt(c))
            return ToInt(c);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get int value when required");
    return defaultValue;
}

int XMLHelper::GetIntValue(const std::string& elementName, XMLElement* elementParent, int defaultValue, bool required)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetIntValue(element, defaultValue);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get int value when required");
    return defaultValue;
}

unsigned int XMLHelper::GetUnsignedIntValue(XMLElement* element, unsigned int defaultValue, bool required)
{
    if (element) {
        std::string c = element->GetText();
        if (IsUnsignedInt(c))
            return ToUnsignedInt(c);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get unsigned int value when required");
    return defaultValue;
}

unsigned int XMLHelper::GetUnsignedIntValue(const std::string& elementName, XMLElement* elementParent, unsigned int defaultValue, bool required)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());
    if (element)
    {
        return GetUnsignedIntValue(element, defaultValue);
    }

    MusicXMLParser::AddErrorIf(required, "Required parse value error", "Failed to get unsigned int value when required");
    return defaultValue;
}