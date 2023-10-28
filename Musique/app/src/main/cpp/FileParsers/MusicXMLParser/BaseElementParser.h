/**
 * This file defines the `BaseElementParser` class which parses base elements (classes like `BaseElement` or `LineElement`) for MusicXML files.
 */

#ifndef MUSIQUE_BASEELEMENTPARSER_H
#define MUSIQUE_BASEELEMENTPARSER_H

#include <string>
#include <memory>

#include "../../libs/tinyxml2/tinyxml2.h"
#include "ParseError.h"

#include "../../MusicData/BaseElements/BaseElement.h"
#include "../../MusicData/BaseElements/PrintableElement.h"
#include "../../MusicData/BaseElements/VisibleElement.h"
#include "../../MusicData/BaseElements/TextualElement.h"
#include "../../MusicData/BaseElements/LineElement.h"

using namespace tinyxml2;

/**
 * A 'singleton' that parses base element classes (like `BaseElement` or `LineElement`).
 */
class BaseElementParser
{
public:

    static void ParseBaseElement(XMLElement* element, std::shared_ptr<BaseElement> newElement);
    static void ParsePrintableElement(XMLElement* element, std::shared_ptr<PrintableElement> newElement);
    static void ParseVisibleElement(XMLElement* element, std::shared_ptr<VisibleElement> newElement);
    static void ParseTextualElement(XMLElement* element, std::shared_ptr<TextualElement> newElement);
    static void ParseLineElement(XMLElement* element, std::shared_ptr<LineElement> newElement);

    static void ParseBaseElement(XMLElement* element, BaseElement& newElement);
    static void ParsePrintableElement(XMLElement* element, PrintableElement& newElement);
    static void ParseVisibleElement(XMLElement* element, VisibleElement& newElement);
    static void ParseTextualElement(XMLElement* element, TextualElement& newElement);
    static void ParseLineElement(XMLElement* element, LineElement& newElement);

protected:
    static void AddError(std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { m_Errors.emplace_back(title, desc, "BaseElementParser", errorLevel); }
    static void AddErrorIf(bool condition, std::string title, std::string desc, ErrorLevel errorLevel = ErrorLevel::Error) { if (condition) m_Errors.emplace_back(title, desc, "BaseElementParser", errorLevel); }
};

#endif //MUSIQUE_BASEELEMENTPARSER_H
