#include "BaseElementParser.h"

#include "MusicXMLHelper.h"
#include "XMLHelper.h"

void BaseElementParser::ParseBaseElement(XMLElement* element, std::shared_ptr<BaseElement> newElement)
{
    if (element)
    {
        newElement->id = XMLHelper::GetStringAttribute(element, "id", newElement->id);
    }
}

void BaseElementParser::ParsePrintableElement(XMLElement* element, std::shared_ptr<PrintableElement> newElement)
{
    if (element)
    {
        // TODO: implement
        ParseBaseElement(element, newElement);
    }
}

void BaseElementParser::ParseVisibleElement(XMLElement* element, std::shared_ptr<VisibleElement> newElement)
{
    if (element)
    {
        // TODO: implement
        ParsePrintableElement(element, newElement);
    }
}

void BaseElementParser::ParseTextualElement(XMLElement* element, std::shared_ptr<TextualElement> newElement)
{
    if (element)
    {
        newElement->fontFamily = MusicXMLHelper::GetFontFamilyAttribute(element, "font-family");
        newElement->fontSize = MusicXMLHelper::GetFontSizeAttribute(element, "font-size");
        newElement->fontStyle = MusicXMLHelper::GetFontStyleAttribute(element, "font-style");
        newElement->fontWeight = MusicXMLHelper::GetFontWeightAttribute(element, "font-weight");

        newElement->linesThrough = XMLHelper::GetUnsignedIntAttribute(element, "line-through");
        newElement->overline = XMLHelper::GetUnsignedIntAttribute(element, "overline");
        newElement->underline = XMLHelper::GetUnsignedIntAttribute(element, "underline");

        newElement->justify = MusicXMLHelper::GetJustifyAttribute(element, "justify");

        ParseVisibleElement(element, newElement);
    }
}

void BaseElementParser::ParseLineElement(XMLElement* element, std::shared_ptr<LineElement> newElement)
{
    if (element)
    {
        std::string lineTypeString = XMLHelper::GetStringAttribute(element, "line-type", "");

        if (lineTypeString == "dashed")
            newElement->lineType = LineType::Dashed;
        else if (lineTypeString == "dotted")
            newElement->lineType = LineType::Dotted;
        else if (lineTypeString == "solid")
            newElement->lineType = LineType::Solid;
        else if (lineTypeString == "wavy")
            newElement->lineType = LineType::Wavy;
        else if (lineTypeString.empty())
            ; // use default value
        else
            AddError("Unrecognized type", "Did not recognize line type");

        newElement->dashLength = XMLHelper::GetFloatAttribute(element, "dash-length", newElement->dashLength);
        newElement->dashSpaceLength = XMLHelper::GetFloatAttribute(element, "space-length", newElement->dashSpaceLength);

        ParseBaseElement(element, newElement);
    }
}

void BaseElementParser::ParseBaseElement(XMLElement* element, BaseElement& newElement)
{
    if (element)
    {
        newElement.id = XMLHelper::GetStringAttribute(element, "id", newElement.id);
    }
}

void BaseElementParser::ParsePrintableElement(XMLElement* element, PrintableElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParseBaseElement(element, newElement);
    }
}

void BaseElementParser::ParseVisibleElement(XMLElement* element, VisibleElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParsePrintableElement(element, newElement);
    }
}

void BaseElementParser::ParseTextualElement(XMLElement* element, TextualElement& newElement)
{
    if (element)
    {
        newElement.fontFamily = MusicXMLHelper::GetFontFamilyAttribute(element, "font-family");
        newElement.fontSize = MusicXMLHelper::GetFontSizeAttribute(element, "font-size");
        newElement.fontStyle = MusicXMLHelper::GetFontStyleAttribute(element, "font-style");
        newElement.fontWeight = MusicXMLHelper::GetFontWeightAttribute(element, "font-weight");

        newElement.linesThrough = XMLHelper::GetUnsignedIntAttribute(element, "line-through");
        newElement.overline = XMLHelper::GetUnsignedIntAttribute(element, "overline");
        newElement.underline = XMLHelper::GetUnsignedIntAttribute(element, "underline");

        newElement.justify = MusicXMLHelper::GetJustifyAttribute(element, "justify");

        ParseVisibleElement(element, newElement);
    }
}

void BaseElementParser::ParseLineElement(XMLElement* element, LineElement& newElement)
{
    if (element)
    {
        std::string lineTypeString = XMLHelper::GetStringAttribute(element, "line-type", "");

        if (lineTypeString == "dashed")
            newElement.lineType = LineType::Dashed;
        else if (lineTypeString == "dotted")
            newElement.lineType = LineType::Dotted;
        else if (lineTypeString == "solid")
            newElement.lineType = LineType::Solid;
        else if (lineTypeString == "wavy")
            newElement.lineType = LineType::Wavy;
        else if (lineTypeString.empty())
            ; // use default value
        else
            AddError("Unrecognized type", "Did not recognize line type");

        newElement.dashLength = XMLHelper::GetFloatAttribute(element, "dash-length", newElement.dashLength);
        newElement.dashSpaceLength = XMLHelper::GetFloatAttribute(element, "space-length", newElement.dashSpaceLength);

        ParseBaseElement(element, newElement);
    }
}