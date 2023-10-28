#include "XMLHelper.h"
#include "MusicXMLParser.h"
#include "../../Utils/Converters.h"

// ---- Other ----

bool XMLHelper::DoesElementExist(const std::string& elementName, XMLElement* elementParent)
{
    XMLElement* element = elementParent->FirstChildElement(elementName.c_str());

    if (element)
    {
        return true;
    }

    return false;
}

// ---- Conversions ----

bool XMLHelper::FromYesNoToBool(const char* string)
{
    if (strcmp(string, "yes") == 0) {
        return true;
    }
    return false;
}

// ---- Get Value Functions ----

std::string XMLHelper::GetStringValue(XMLElement* element, std::string defaultValue, bool required)
{
    if (element) {
        const char* s = element->GetText();
        if (s == nullptr)
            return "";
        return s;
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

// ---- Get Attribute Functions ----

bool XMLHelper::GetBoolAttribute(XMLElement* element, const char* s, bool defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return FromYesNoToBool(attribute);
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

float XMLHelper::GetFloatAttribute(XMLElement* element, const char* s, float defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToFloat(attribute);
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

std::string XMLHelper::GetStringAttribute(XMLElement* element, const char* s, std::string defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return attribute;
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

int XMLHelper::GetNumberAttribute(XMLElement* element, const char* s, int defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToInt(attribute);
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

unsigned int XMLHelper::GetUnsignedIntAttribute(XMLElement* element, const char* s, unsigned int defaultValue, bool required)
{
    const char* attribute = element->Attribute(s);
    if (attribute) {
        return ToUnsignedInt(attribute);
    }

    AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    return defaultValue;
}

Vec2<float> XMLHelper::GetFloatVec2Attribute(XMLElement* element, const char* s1, const char* s2, Vec2<float> defaultValue, bool required)
{
    Vec2<float> value = defaultValue;

    const char* attribute1 = element->Attribute(s1);
    if (attribute1) {
        value.x = ToFloat(attribute1);
    }
    else {
        AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    }

    const char* attribute2 = element->Attribute(s2);
    if (attribute2) {
        value.y = ToFloat(attribute2);
    }
    else {
        AddErrorIf(required, "Required parse attribute error", "Failed to get attribute when required");
    }

    return value;
}

// ---- Lists ----

std::vector<int> XMLHelper::ParseIntList(const std::string& stringList)
{
    std::vector<int> list;

    std::string num;
    for (char c : stringList)
    {
        if (c != ',' && c != ' ')
        {
            num += c;
        }
        else if (!num.empty())
        {
            list.push_back(ToInt(num));
            num = "";
        }
    }

    if (!num.empty())
    {
        list.push_back(ToInt(num));
    }

    return list;
}