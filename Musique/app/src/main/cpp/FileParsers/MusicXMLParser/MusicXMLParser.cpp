#include "MusicXMLParser.h"
#include "../../Utils/Converters.h"
#include <memory>

#include "../../Collisions/Vec2.h"
#include "BaseElementParser.h"
#include "NoteElementParser.h"
#include "../../Exceptions/Exceptions.h"

static std::unordered_map<int, std::shared_ptr<DynamicWedge>> currentDynamicWedges;
static std::unordered_map<int, std::shared_ptr<BracketDirection>> currentDashes;
static std::unordered_map<int, std::shared_ptr<BracketDirection>> currentBrackets;

static std::unordered_map<int, std::shared_ptr<Arpeggio>> currentArpeggios;
static float currentArpeggiosBeatPosition = 0.0f;
static int currentArpeggiosMeasureIndex = 0;

static std::unordered_map<std::string, std::shared_ptr<Ending>> currentEndings;
static std::shared_ptr<EndingGroup> currentEndingGroup;

static std::unordered_map<int, std::shared_ptr<Slur>> currentSlurs;


// ---- Parse Functions ----

Rehearsal MusicXMLParser::ParseRehearsal(XMLElement* element)
{
    Rehearsal rehearsal = Rehearsal();

    if (element)
    {
        rehearsal = ParseWords(element);
    }

    return rehearsal;
}

Words MusicXMLParser::ParseWords(XMLElement* element)
{
    Words words = Words();

    if (element)
    {
        const char* s = element->GetText();
        if (s != nullptr)
            words.text.string = s;
        else
            LOGE("ERROR: string in WORDS is null <_____________________________________________________________________");

        words.defX = XMLHelper::GetFloatAttribute(element, "default-x", words.defX);
        words.defY = XMLHelper::GetFloatAttribute(element, "default-y", words.defY);

        if (words.defX == 0.0f)
            words.noDefX = true;
        else
            words.noDefX = false;

        if (words.defY == 0.0f)
            words.noDefY = true;
        else
            words.noDefY = false;

        words.relX = XMLHelper::GetFloatAttribute(element, "relative-x", words.relX);
        words.relY = XMLHelper::GetFloatAttribute(element, "relative-y", words.relY);

        std::string enclosureString = XMLHelper::GetStringAttribute(element, "enclosure", "");

        Words::EnclosureShape enclosure = Words::EnclosureShape::None;

        if (enclosureString == "rectangle")
            enclosure = Words::EnclosureShape::Rectangle;
        else if (enclosureString == "square")
            enclosure = Words::EnclosureShape::Square;
        else if (enclosureString == "oval")
            enclosure = Words::EnclosureShape::Oval;
        else if (enclosureString == "circle")
            enclosure = Words::EnclosureShape::Circle;
        else if (enclosureString == "bracket")
            enclosure = Words::EnclosureShape::Bracket;
        else if (enclosureString == "inverted-bracket")
            enclosure = Words::EnclosureShape::InvertedBracket;
        else if (enclosureString == "triangle")
            enclosure = Words::EnclosureShape::Triangle;
        else if (enclosureString == "diamond")
            enclosure = Words::EnclosureShape::Diamond;
        else if (enclosureString == "pentagon")
            enclosure = Words::EnclosureShape::Pentagon;
        else if (enclosureString == "hexagon")
            enclosure = Words::EnclosureShape::Hexagon;
        else if (enclosureString == "heptagon")
            enclosure = Words::EnclosureShape::Heptagon;
        else if (enclosureString == "octagon")
            enclosure = Words::EnclosureShape::Octagon;
        else if (enclosureString == "nonagon")
            enclosure = Words::EnclosureShape::Nonagon;
        else if (enclosureString == "decagon")
            enclosure = Words::EnclosureShape::Decagon;
        else if (enclosureString == "none")
            enclosure = Words::EnclosureShape::None;

        BaseElementParser::ParseTextualElement(element, words);
    }

    return words;
}

std::shared_ptr<MetronomeMark> MusicXMLParser::ParseMetronomeMark(XMLElement* element)
{
    std::shared_ptr<MetronomeMark> metronomeMark = std::make_shared<MetronomeMark>();

    if (element)
    {
        metronomeMark->defX = XMLHelper::GetFloatAttribute(element, "default-x", metronomeMark->defX);
        metronomeMark->defY = XMLHelper::GetFloatAttribute(element, "default-y", metronomeMark->defY);

        metronomeMark->noDefX = (metronomeMark->defX == 0.0f);
        metronomeMark->noDefY = (metronomeMark->defY == 0.0f);

        metronomeMark->relX = XMLHelper::GetFloatAttribute(element, "relative-x", metronomeMark->relX);
        metronomeMark->relY = XMLHelper::GetFloatAttribute(element, "relative-y", metronomeMark->relY);

        metronomeMark->hasParentheses = XMLHelper::GetBoolAttribute(element, "parentheses", metronomeMark->hasParentheses);

        std::string beatUnitString = XMLHelper::GetStringValue("beat-unit", element, "");
        NoteUnit mainNoteUnit = NoteUnit();
        mainNoteUnit.noteValue = MusicXMLHelper::GetNoteValueTypeFromString(beatUnitString);
        mainNoteUnit.isDotted = XMLHelper::DoesElementExist("beat-unit-dot", element);
        metronomeMark->mainNoteUnit = mainNoteUnit;

        metronomeMark->tempo = XMLHelper::GetStringValue("per-minute", element, metronomeMark->tempo);
    }

    return metronomeMark;
}

Dynamic MusicXMLParser::ParseDynamicElement(XMLElement* element)
{
    Dynamic dynamic = Dynamic();

    if (element)
    {
        BaseElementParser::ParseTextualElement(element, dynamic);

        dynamic.defaultPosition = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dynamic.defaultPosition);
        dynamic.relativePosition = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dynamic.relativePosition);

        // flip y
        if (dynamic.defaultPosition.y != INVALID_FLOAT_VALUE)
            dynamic.defaultPosition.y = -dynamic.defaultPosition.y;

        if (dynamic.relativePosition.y != INVALID_FLOAT_VALUE)
            dynamic.relativePosition.y = -dynamic.relativePosition.y;

        dynamic.placement = MusicXMLHelper::GetAboveBelowAttribute(element, "placement", dynamic.placement);

        // loop through all child elements
        int childElementsCount = 0;
        XMLNode* previousElement = element->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* childElement = previousElement->ToElement();
                const char* value = childElement->Value();

                if (childElementsCount == 0)
                {
                    if (strcmp(value, "p") == 0)
                        dynamic.type = Dynamic::DynamicType::Piano;
                    else if (strcmp(value, "pp") == 0)
                        dynamic.type = Dynamic::DynamicType::Pianissimo, dynamic.dynamicIntensity = 2;
                    else if (strcmp(value, "ppp") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherPiano, dynamic.dynamicIntensity = 3;
                    else if (strcmp(value, "pppp") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherPiano, dynamic.dynamicIntensity = 4;
                    else if (strcmp(value, "ppppp") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherPiano, dynamic.dynamicIntensity = 5;
                    else if (strcmp(value, "pppppp") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherPiano, dynamic.dynamicIntensity = 6;
                    else if (strcmp(value, "f") == 0)
                        dynamic.type = Dynamic::DynamicType::Forte;
                    else if (strcmp(value, "ff") == 0)
                        dynamic.type = Dynamic::DynamicType::Fortissimo, dynamic.dynamicIntensity = 2;
                    else if (strcmp(value, "fff") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherForte, dynamic.dynamicIntensity = 3;
                    else if (strcmp(value, "ffff") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherForte, dynamic.dynamicIntensity = 4;
                    else if (strcmp(value, "fffff") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherForte, dynamic.dynamicIntensity = 5;
                    else if (strcmp(value, "ffffff") == 0)
                        dynamic.type = Dynamic::DynamicType::OtherForte, dynamic.dynamicIntensity = 6;
                    else if (strcmp(value, "mp") == 0)
                        dynamic.type = Dynamic::DynamicType::MezzoPiano;
                    else if (strcmp(value, "mf") == 0)
                        dynamic.type = Dynamic::DynamicType::MezzoForte;
                    else if (strcmp(value, "sf") == 0)
                        dynamic.type = Dynamic::DynamicType::SF;
                    else if (strcmp(value, "sfp") == 0)
                        dynamic.type = Dynamic::DynamicType::SFP;
                    else if (strcmp(value, "sfpp") == 0)
                        dynamic.type = Dynamic::DynamicType::SFPP;
                    else if (strcmp(value, "fp") == 0)
                        dynamic.type = Dynamic::DynamicType::FP;
                    else if (strcmp(value, "rf") == 0)
                        dynamic.type = Dynamic::DynamicType::RF;
                    else if (strcmp(value, "rfz") == 0)
                        dynamic.type = Dynamic::DynamicType::RFZ;
                    else if (strcmp(value, "sfz") == 0)
                        dynamic.type = Dynamic::DynamicType::SFZ;
                    else if (strcmp(value, "sffz") == 0)
                        dynamic.type = Dynamic::DynamicType::SFFZ;
                    else if (strcmp(value, "fz") == 0)
                        dynamic.type = Dynamic::DynamicType::FZ;
                    else if (strcmp(value, "n") == 0)
                        dynamic.type = Dynamic::DynamicType::N;
                    else if (strcmp(value, "pf") == 0)
                        dynamic.type = Dynamic::DynamicType::PF;
                    else if (strcmp(value, "sfzp") == 0)
                        dynamic.type = Dynamic::DynamicType::SFZP;
                    else if (strcmp(value, "other-dynamics") == 0) // it is a different dynamic
                    {
                        dynamic.type = Dynamic::DynamicType::OtherDynamic;
                        AddError("Unsupported element", "This dynamic is not supported");
                    }
                    else
                    {
                        AddError("Didn't recognize element", "Didn't recognize element in dynamic");
                    }
                }
                else
                {
                    dynamic.type = Dynamic::DynamicType::OtherDynamic;
                }

                dynamic.displayString += value;

                childElementsCount++;
            }
            else
            {
                break;
            }

            previousElement = previousElement->NextSiblingElement();
        }
    }

    return dynamic;
}

std::shared_ptr<DynamicWedge> MusicXMLParser::ParseDynamicWedgeElement(XMLElement* element, float currentTimeInMeasure, int measureIndex)
{
    if (element)
    {
        int number = XMLHelper::GetNumberAttribute(element, "number", (int)currentDynamicWedges.size() + 1) - 1; // starts at 0
        std::string wedgeType = XMLHelper::GetStringAttribute(element, "type", "", true);


        if (wedgeType == "crescendo" || wedgeType == "diminuendo")
        {
            std::shared_ptr<DynamicWedge> dynamicWedge = std::make_shared<DynamicWedge>();

            //ParseVisibleElement(element, dynamicWedge);

            dynamicWedge->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dynamicWedge->defaultPositionStart);
            dynamicWedge->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dynamicWedge->relativePositionStart);

            // flip y
            if (dynamicWedge->defaultPositionStart.y != INVALID_FLOAT_VALUE)
                dynamicWedge->defaultPositionStart.y = -dynamicWedge->defaultPositionStart.y;
            if (dynamicWedge->relativePositionStart.y != INVALID_FLOAT_VALUE)
                dynamicWedge->relativePositionStart.y = -dynamicWedge->relativePositionStart.y;

            if (dynamicWedge->defaultPositionEnd.y != INVALID_FLOAT_VALUE)
                dynamicWedge->defaultPositionEnd.y = -dynamicWedge->defaultPositionEnd.y;
            if (dynamicWedge->relativePositionEnd.y != INVALID_FLOAT_VALUE)
                dynamicWedge->relativePositionEnd.y = -dynamicWedge->relativePositionEnd.y;

            if (wedgeType == "crescendo")
                dynamicWedge->type = DynamicWedge::WedgeType::Crescendo;
            else
                dynamicWedge->type = DynamicWedge::WedgeType::Diminuendo;

            if (dynamicWedge->type == DynamicWedge::WedgeType::Diminuendo) // the start of a diminuendo
                dynamicWedge->defaultSpread = XMLHelper::GetFloatAttribute(element, "spread", dynamicWedge->defaultSpread);

            dynamicWedge->beatPositionStart = currentTimeInMeasure;
            dynamicWedge->startMeasureIndex = measureIndex;

            currentDynamicWedges[number] = dynamicWedge;

            return dynamicWedge;

            /*if (number < currentDynamicWedges.size())
                currentDynamicWedges[number] = dynamicWedge;
            else
            {
                currentDynamicWedges.resize(number + 1);
                currentDynamicWedges[number] = dynamicWedge;
            }*/
        }
        else if (wedgeType == "stop")
        {
            auto dynamicWedge = currentDynamicWedges[number];

            dynamicWedge->defaultPositionEnd = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dynamicWedge->defaultPositionEnd);
            dynamicWedge->relativePositionEnd = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dynamicWedge->relativePositionEnd);

            if (dynamicWedge->type == DynamicWedge::WedgeType::Crescendo) // the end of a crescendo
                dynamicWedge->defaultSpread = XMLHelper::GetFloatAttribute(element, "spread", dynamicWedge->defaultSpread);

            dynamicWedge->beatPositionEnd = currentTimeInMeasure;
            dynamicWedge->endMeasureIndex = measureIndex;

            currentDynamicWedges.erase(number);
        }
        else if (wedgeType == "continue")
        {
            LOGE("wedge continue");
        }
        else
            AddError("Unrecognized type", "Did not recognize wedge type.");
    }

    return nullptr; // meaning that either the function failed or that the wedge was already added
}

std::shared_ptr<BracketDirection> MusicXMLParser::ParseDashesDirectionElement(XMLElement* element, float currentTimeInMeasure, int measureIndex)
{
    if (element)
    {
        int number = XMLHelper::GetNumberAttribute(element, "number", (int)currentDashes.size() + 1) - 1; // starts at 0
        std::string typeString = XMLHelper::GetStringAttribute(element, "type", "", true);


        if (typeString == "start")
        {
            LOGW("start of dashes");

            std::shared_ptr<BracketDirection> dashes = std::make_shared<BracketDirection>();

            BaseElementParser::ParseVisibleElement(element, *dashes);
            BaseElementParser::ParseLineElement(element, *dashes);

            dashes->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dashes->defaultPositionStart);
            dashes->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dashes->relativePositionStart);

            // flip y so that positive y is down (not up)
            MusicXMLHelper::FlipYInVec2(dashes->defaultPositionStart);
            MusicXMLHelper::FlipYInVec2(dashes->relativePositionStart);

            dashes->beatPositionStart = currentTimeInMeasure;
            dashes->startMeasureIndex = measureIndex;

            dashes->lineType = LineType::Dashed;
            dashes->isDashes = true;

            currentDashes[number] = dashes;

            return dashes;
        }
        else if (typeString == "stop" && currentDashes.size() > number)
        {
            LOGW("dashes stop");

            auto dashes = currentDashes[number];

            dashes->defaultPositionEnd = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dashes->defaultPositionEnd);
            dashes->relativePositionEnd = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dashes->relativePositionEnd);

            // flip y so that positive y is down (not up)
            MusicXMLHelper::FlipYInVec2(dashes->defaultPositionEnd);
            MusicXMLHelper::FlipYInVec2(dashes->relativePositionEnd);

            dashes->beatPositionEnd = currentTimeInMeasure;
            dashes->endMeasureIndex = measureIndex;

            currentDashes.erase(number);
        }
        else if (typeString == "continue")
        {
            LOGE("dashes continue");
        }
        else
            AddError("Unrecognized type", "Did not recognize dashes type: \'" + typeString + "\'.");
    }

    return nullptr; // meaning that either the function failed or that the object was already added
}

std::shared_ptr<BracketDirection> MusicXMLParser::ParseBracketDirectionElement(XMLElement* element, float currentTimeInMeasure, int measureIndex)
{
    if (element)
    {
        int number = XMLHelper::GetNumberAttribute(element, "number", (int)currentBrackets.size() + 1) - 1; // starts at 0
        std::string typeString = XMLHelper::GetStringAttribute(element, "type", "", true);


        if (typeString == "start")
        {
            LOGW("start of bracket");

            std::shared_ptr<BracketDirection> brackets = std::make_shared<BracketDirection>();

            BaseElementParser::ParseVisibleElement(element, *brackets);
            BaseElementParser::ParseLineElement(element, *brackets);

            brackets->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", brackets->defaultPositionStart);
            brackets->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", brackets->relativePositionStart);

            // flip y so that positive y is down (not up)
            MusicXMLHelper::FlipYInVec2(brackets->defaultPositionStart);
            MusicXMLHelper::FlipYInVec2(brackets->relativePositionStart);

            brackets->beatPositionStart = currentTimeInMeasure;
            brackets->startMeasureIndex = measureIndex;

            std::string lineEndString = XMLHelper::GetStringAttribute(element, "line-end", "none", true);
            brackets->lineEndTypeStart = MusicXMLHelper::GetLineEndTypeFromString(lineEndString);

            brackets->endLengthStart = XMLHelper::GetFloatAttribute(element, "end-length", brackets->endLengthStart);

            currentBrackets[number] = brackets;

            return brackets;
        }
        else if (typeString == "stop" && currentBrackets.size() > number)
        {
            LOGW("bracket stop");

            auto brackets = currentBrackets[number];

            brackets->defaultPositionEnd = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", brackets->defaultPositionEnd);
            brackets->relativePositionEnd = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", brackets->relativePositionEnd);

            // flip y so that positive y is down (not up)
            MusicXMLHelper::FlipYInVec2(brackets->defaultPositionEnd);
            MusicXMLHelper::FlipYInVec2(brackets->relativePositionEnd);

            brackets->beatPositionEnd = currentTimeInMeasure;
            brackets->endMeasureIndex = measureIndex;

            std::string lineEndString = XMLHelper::GetStringAttribute(element, "line-end", "none", true);
            brackets->lineEndTypeStop = MusicXMLHelper::GetLineEndTypeFromString(lineEndString);

            brackets->endLengthStop = XMLHelper::GetFloatAttribute(element, "end-length", brackets->endLengthStart);

            currentBrackets.erase(number);
        }
        else if (typeString == "continue")
        {
            LOGE("bracket continue");
        }
        else
            AddError("Unrecognized type", "Did not recognize bracket type: \'" + typeString + "\'.");
    }

    return nullptr; // meaning that either the function failed or that the object was already added
}

std::shared_ptr<DurationDirection> MusicXMLParser::ParseDurationDirection(XMLElement* directionElement, bool& isNewDirection, float currentTimeInMeasure, int measureIndex)
{
    std::shared_ptr<DurationDirection> durationDirection = nullptr;

    if (directionElement)
    {
        bool isWordsElement = false;
        XMLElement* wordsElement = nullptr;

        // loop through all elements
        XMLNode* previousElement = directionElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement)
            {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();

                if (strcmp(value, "direction-type") == 0) // direction-type
                {
                    XMLElement* directionTypeElement = element;

                    XMLElement* wordsEle = directionTypeElement->FirstChildElement("words");
                    if (wordsEle)
                    {
                        isWordsElement = true;
                        wordsElement = wordsEle;
                    }

                    XMLElement* dynamicWedgeElement = directionTypeElement->FirstChildElement("wedge");
                    if (dynamicWedgeElement)
                    {
                        durationDirection = ParseDynamicWedgeElement(dynamicWedgeElement, currentTimeInMeasure, measureIndex);
                    }

                    // note: dashes and brackets are the same class (the BracketDirection class)

                    // dashes
                    XMLElement* dashesDirectionElement = directionTypeElement->FirstChildElement("dashes");
                    if (dashesDirectionElement)
                    {
                        std::shared_ptr<BracketDirection> bracketDirection = ParseDashesDirectionElement(dashesDirectionElement, currentTimeInMeasure, measureIndex);

                        if (wordsElement && isWordsElement)
                        {
                            bracketDirection->hasText = true;
                            bracketDirection->textElement.text = wordsElement->GetText();
                        }

                        durationDirection = bracketDirection;
                    }

                    // bracket
                    XMLElement* bracketDirectionElement = directionTypeElement->FirstChildElement("bracket");
                    if (bracketDirectionElement)
                    {
                        std::shared_ptr<BracketDirection> bracketDirection = ParseBracketDirectionElement(bracketDirectionElement, currentTimeInMeasure, measureIndex);

                        if (wordsElement && isWordsElement)
                        {
                            bracketDirection->hasText = true;
                            bracketDirection->textElement.text = wordsElement->GetText();
                        }

                        durationDirection = bracketDirection;
                    }
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }

    if (durationDirection == nullptr)
    {
        isNewDirection = false;
    }
    else
    {
        isNewDirection = true;
    }

    return durationDirection;
}

Direction MusicXMLParser::ParseDirection(XMLElement* directionElement, bool& isNewDirection, float currentTimeInMeasure)
{
    isNewDirection = true;

    Direction direction = Direction();
    if (directionElement)
    {
        // loop through all elements
        XMLNode* previousElement = directionElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();

                if (strcmp(value, "direction-type") == 0) // direction-type
                {
                    XMLElement* directionTypeElement = element;

                    // rehearsal
                    XMLElement* rehearsalElement = directionTypeElement->FirstChildElement("rehearsal");
                    if (rehearsalElement)
                    {
                        direction.rehearsals.push_back(ParseRehearsal(rehearsalElement));
                    }

                    // words
                    XMLElement* wordsElement = directionTypeElement->FirstChildElement("words");
                    if (wordsElement)
                    {
                        direction.words.push_back(ParseWords(wordsElement));
                    }

                    // metronome
                    XMLElement* metronomeElement = directionTypeElement->FirstChildElement("metronome");
                    if (metronomeElement)
                    {
                        direction.metronomeMark = ParseMetronomeMark(metronomeElement);
                    }

                    // dynamic
                    XMLElement* dynamicElement = directionTypeElement->FirstChildElement("dynamics");
                    if (dynamicElement)
                    {
                        direction.dynamics.push_back(ParseDynamicElement(dynamicElement));
                    }

                    // dynamic wedge
                    /*XMLElement* dynamicWedgeElement = directionTypeElement->FirstChildElement("wedge");
                    if (dynamicWedgeElement)
                    {
                        direction.dynamicWedge = ParseDynamicWedgeElement(dynamicWedgeElement, currentTimeInMeasure);
                        if (direction.dynamicWedge == nullptr)
                            isNewDirection = false;
                    }*/

                    // note: dashes and brackets are the same class (the BracketDirection class)

                    // dashes
                    /*XMLElement* dashesDirectionElement = directionTypeElement->FirstChildElement("dashes");
                    if (dashesDirectionElement)
                    {
                        direction.bracketDirection = ParseDashesDirectionElement(dashesDirectionElement, currentTimeInMeasure);
                        if (direction.bracketDirection == nullptr)
                            isNewDirection = false;
                    }

                    // bracket
                    XMLElement* bracketDirectionElement = directionTypeElement->FirstChildElement("bracket");
                    if (bracketDirectionElement)
                    {
                        direction.bracketDirection = ParseBracketDirectionElement(bracketDirectionElement, currentTimeInMeasure);
                        if (direction.bracketDirection == nullptr)
                            isNewDirection = false;
                    }*/

                    // segno
                    XMLElement* segnoElement = directionTypeElement->FirstChildElement("segno");
                    if (segnoElement)
                    {
                        direction.marker = ParseCodaSegnoElement(segnoElement);
                    }

                    // coda
                    XMLElement* codaElement = directionTypeElement->FirstChildElement("coda");
                    if (codaElement)
                    {
                        direction.marker = ParseCodaSegnoElement(codaElement);
                    }
                }
                else if (strcmp(value, "offset") == 0) // offset
                {
                    // TODO: implement
                }
                else if (strcmp(value, "footnote") == 0) // footnote
                {
                    // TODO: implement
                }
                else if (strcmp(value, "level") == 0) // level
                {
                    // TODO: implement
                }
                else if (strcmp(value, "voice") == 0) // voice
                {
                    // TODO: implement
                }
                else if (strcmp(value, "staff") == 0) // staff
                {
                    // TODO: implement
                }
                else if (strcmp(value, "sound") == 0) // sound
                {
                    // TODO: implement
                }
                else if (strcmp(value, "listening") == 0) // listening
                {
                    // TODO: implement
                }
                else
                {
                    AddError("Didn't recognize element", "Didn't recognize element in Direction");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
    return direction;
}

void MusicXMLParser::ParseWorkElement(XMLElement* workElement, std::string& workTitle, int& workNumber)
{
    if (workElement)
    {
        // work title
        XMLElement* workTitleElement = workElement->FirstChildElement("work-title");
        workTitle = XMLHelper::GetStringValue(workTitleElement, workTitle);

        // work number
        XMLElement* workNumberElement = workElement->FirstChildElement("work-number");
        if (workNumberElement)
        {
            workNumber = XMLHelper::GetIntValue(workNumberElement, workNumber);
        }
    }
}

void MusicXMLParser::ParseEncodingElement(XMLElement* encodingElement, const std::shared_ptr<Song>& song)
{
    if (encodingElement)
    {
        // loop through all elements
        XMLNode* previousElement = encodingElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "encoding-date") == 0) // encoding-date
                {
                    song->encodingDate = MusicXMLHelper::FromStringToDate(XMLHelper::GetStringValue(element, "1900-01-01").c_str());
                }
                else if (strcmp(value, "encoder") == 0) // encoder
                {
                    XMLElement* encoderElement = element;
                    Song::Encoder encoder = Song::Encoder();
                    encoder.name = XMLHelper::GetStringValue(encoderElement, encoder.name);
                    encoder.strType = XMLHelper::GetStringAttribute(encoderElement, "type", encoder.strType);
                    if (encoder.strType == "music")
                        encoder.type = Song::Encoder::EncoderType::Music;
                    else if (encoder.strType == "words")
                        encoder.type = Song::Encoder::EncoderType::Words;
                    else if (encoder.strType == "arrangement")
                        encoder.type = Song::Encoder::EncoderType::Arrangement;
                    song->encoders.push_back(encoder);
                }
                else if (strcmp(value, "software") == 0) // software
                {
                    song->software = XMLHelper::GetStringValue(element, song->software);

                    // makes the assumption that the string is in the format: "SoftwareName MAJOR.0.0"
                    song->softwareName = song->software.substr(0, song->software.find(' '));
                    song->softwareMajorVersion = ToInt(song->software.substr(song->software.find(' ') + 1, song->software.find('.')));

                    LOGI("Software: %s, SoftwareName: %s, SoftwareMajorVersion: %d", song->software.c_str(), song->softwareName.c_str(), song->softwareMajorVersion);
                }
                else if (strcmp(value, "encoding-description") == 0) // encoding description
                {
                    song->encodingDescription = XMLHelper::GetStringValue(element, song->encodingDescription);
                }
                else if (strcmp(value, "supports") == 0) // supports
                {
                    // TODO: handle this
                }
                else
                {
                    LOGE("didn't recognize element in Encoding");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
}

void MusicXMLParser::ParseIdentificationElement(XMLElement* idElement, const std::shared_ptr<Song>& song)
{
    if (idElement)
    {
        // loop through all elements
        XMLNode* previousElement = idElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "creator") == 0) // creator
                {
                    XMLElement* creatorElement = element;
                    Song::Creator creator = Song::Creator();
                    creator.name = XMLHelper::GetStringValue(creatorElement, creator.name);
                    creator.strType = XMLHelper::GetStringAttribute(creatorElement, "type", creator.strType);
                    if (creator.strType == "composer")
                        creator.type = Song::Creator::CreatorType::Composer;
                    else if (creator.strType == "lyricist")
                        creator.type = Song::Creator::CreatorType::Lyricist;
                    else if (creator.strType == "arranger")
                        creator.type = Song::Creator::CreatorType::Arranger;
                    song->creators.push_back(creator);
                }
                else if (strcmp(value, "rights") == 0) // rights
                {
                    XMLElement* rightElement = element;
                    Song::Rights rights = Song::Rights();
                    rights.right = XMLHelper::GetStringValue(rightElement, rights.right);
                    rights.strType = XMLHelper::GetStringAttribute(rightElement, "type", rights.strType);
                    if (rights.strType == "music")
                        rights.type = Song::Rights::RightType::Music;
                    else if (rights.strType == "words")
                        rights.type = Song::Rights::RightType::Words;
                    else if (rights.strType == "arrangement")
                        rights.type = Song::Rights::RightType::Arrangement;
                    song->rights.push_back(rights);
                }
                else if (strcmp(value, "encoding") == 0) // encoding
                {
                    ParseEncodingElement(element, song);
                }
                else if (strcmp(value, "source") == 0) // source
                {
                    // TODO: handle this
                }
                else if (strcmp(value, "relation") == 0) // relation
                {
                    // TODO: handle this
                }
                else if (strcmp(value, "miscellaneous") == 0) // miscellaneous
                {
                    // TODO: handle this
                }
                else
                {
                    LOGE("didn't recognize element in Identification");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
}

MusicDisplayConstants MusicXMLParser::ParseDefaultsElement(XMLElement* defaultsElement)
{
    MusicDisplayConstants displayConstants = MusicDisplayConstants();

    if (defaultsElement)
    {
        XMLElement* pageLayoutElement = defaultsElement->FirstChildElement("page-layout");
        if (pageLayoutElement)
        {
            displayConstants.pageWidth = XMLHelper::GetFloatValue("page-width", pageLayoutElement, displayConstants.pageWidth, true); // required
            displayConstants.pageHeight = XMLHelper::GetFloatValue("page-height", pageLayoutElement, displayConstants.pageHeight, true); // required

            XMLElement* pageLayoutMarginsElement = pageLayoutElement->FirstChildElement("page-margins");
            if (pageLayoutMarginsElement)
            {
                displayConstants.leftMargin = XMLHelper::GetFloatValue("left-margin", pageLayoutMarginsElement, displayConstants.leftMargin, true); // required
                displayConstants.rightMargin = XMLHelper::GetFloatValue("right-margin", pageLayoutMarginsElement, displayConstants.rightMargin, true); // required
                displayConstants.topMargin = XMLHelper::GetFloatValue("top-margin", pageLayoutMarginsElement, displayConstants.topMargin, true); // required
                displayConstants.bottomMargin = XMLHelper::GetFloatValue("bottom-margin", pageLayoutMarginsElement, displayConstants.bottomMargin, true); // required
            }
        }

        XMLElement* systemLayoutElement = defaultsElement->FirstChildElement("system-layout");
        if (systemLayoutElement)
        {
            displayConstants.systemLayout.systemDistance = XMLHelper::GetFloatValue("system-distance", systemLayoutElement, displayConstants.systemLayout.systemDistance);
            displayConstants.systemLayout.topSystemDistance = XMLHelper::GetFloatValue("top-system-distance", systemLayoutElement, displayConstants.systemLayout.topSystemDistance);

            XMLElement* systemLayoutMarginsElement = systemLayoutElement->FirstChildElement("system-margins");
            if (systemLayoutMarginsElement)
            {
                displayConstants.systemLayout.systemLeftMargin = XMLHelper::GetFloatValue("left-margin", systemLayoutMarginsElement, displayConstants.systemLayout.systemLeftMargin, true); // required
                displayConstants.systemLayout.systemRightMargin = XMLHelper::GetFloatValue("right-margin", systemLayoutMarginsElement, displayConstants.systemLayout.systemRightMargin, true); // required
            }
        }

        XMLElement* staffLayoutElement = defaultsElement->FirstChildElement("staff-layout");
        if (staffLayoutElement)
        {
            displayConstants.staffDistance = XMLHelper::GetFloatValue("staff-distance", staffLayoutElement, displayConstants.staffDistance);
        }
    }

    return displayConstants;
}

Credit MusicXMLParser::ParseCreditElement(XMLElement* creditElement)
{
    Credit credit = Credit();

    if (creditElement)
    {
        BaseElementParser::ParseBaseElement(creditElement, credit);

        credit.pageNumber = XMLHelper::GetUnsignedIntAttribute(creditElement, "page", credit.pageNumber);

        XMLElement* creditWordsElement = creditElement->FirstChildElement("credit-words");
        if (creditWordsElement)
        {
            CreditWords words = CreditWords();
            BaseElementParser::ParseTextualElement(creditWordsElement, words);
            words.text = XMLHelper::GetStringValue(creditWordsElement, words.text);
            words.defaultX = XMLHelper::GetFloatAttribute(creditWordsElement, "default-x", words.defaultX);
            words.defaultY = XMLHelper::GetFloatAttribute(creditWordsElement, "default-y", words.defaultY);

            credit.words = words;
        }
    }

    return credit;
}

void MusicXMLParser::ParseFrameElement(XMLElement* frameElement, Chord& chord)
{
    if (frameElement)
    {
        std::shared_ptr<ChordDiagram> chordDiagram = std::make_shared<ChordDiagram>();

        XMLElement* rootStepElement = frameElement->FirstChildElement("root-step");

        chordDiagram->strings = XMLHelper::GetUnsignedIntValue("frame-strings", frameElement, 0);
        chordDiagram->frets = XMLHelper::GetUnsignedIntValue("frame-frets", frameElement, 0);

        XMLElement* firstFretElement = frameElement->FirstChildElement("first-fret");
        if (firstFretElement)
        {
            chordDiagram->firstFret = XMLHelper::GetUnsignedIntValue(firstFretElement, chordDiagram->firstFret);
            MusicXMLHelper::GetRightLeftAttribute(firstFretElement, "location", chordDiagram->firstFretLocation);
        }

        // loop through frame notes elements
        XMLNode* previousElement = frameElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "frame-note") == 0)
                {
                    XMLElement* frameNoteElement = element;
                    ChordDiagramNote chordDiagramNote = ChordDiagramNote();

                    chordDiagramNote.string = XMLHelper::GetUnsignedIntValue("string", frameNoteElement, chordDiagramNote.string);
                    chordDiagramNote.fret = XMLHelper::GetUnsignedIntValue("fret", frameNoteElement, chordDiagramNote.fret);
                    chordDiagramNote.fingering = XMLHelper::GetUnsignedIntValue("fingering", frameNoteElement, chordDiagramNote.fingering);
                    chordDiagramNote.barre = MusicXMLHelper::GetStartStopValue("barre", frameNoteElement, chordDiagramNote.barre);

                    chordDiagram->notes.push_back(chordDiagramNote);
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }

        chord.chordDiagram = chordDiagram;
    }
}

void MusicXMLParser::ParseHarmonyElement(XMLElement* harmonyElement, float& currentTimeInMeasure, std::vector<std::shared_ptr<Measure>> currentMeasures)
{
    Chord newChord;

    // root note
    XMLElement* rootElement = harmonyElement->FirstChildElement("root");
    if (rootElement)
    {
        XMLElement* rootStepElement = rootElement->FirstChildElement("root-step");
        if (rootStepElement)
        {
            newChord.rootPitch.step = DiatonicNoteFromString(XMLHelper::GetStringValue(rootStepElement, DiatonicNoteToString(newChord.rootPitch.step)));
        }
        else
            ; // TODO: error: this element is required

        newChord.rootPitch.alter = XMLHelper::GetFloatValue("root-alter", rootElement, newChord.rootPitch.alter);
    }

    // bass note
    XMLElement* bassElement = harmonyElement->FirstChildElement("bass");
    if (bassElement)
    {
        XMLElement* bassStepElement = bassElement->FirstChildElement("bass-step");
        if (bassStepElement)
        {
            newChord.bassPitch.step = DiatonicNoteFromString(XMLHelper::GetStringValue(bassStepElement, DiatonicNoteToString(newChord.bassPitch.step)));
        }
        else
            ; // TODO: error: this element is required

        newChord.bassPitch.alter = XMLHelper::GetFloatValue("bass-alter", bassElement, newChord.bassPitch.alter);

        newChord.bassSeparator.string = XMLHelper::GetStringValue("bass-separator", bassElement, newChord.bassSeparator.string);

        std::string bassPosStr = XMLHelper::GetStringAttribute(bassElement, "arrangement", "");
        if (bassPosStr == "horizontal")
            newChord.bassPos = Chord::BassPos::Horizontal;
        else if (bassPosStr == "vertical")
            newChord.bassPos = Chord::BassPos::Vertical;
        else if (bassPosStr == "diagonal")
            newChord.bassPos = Chord::BassPos::Diagonal;
        else if (bassPosStr.empty())
            newChord.bassPos = Chord::BassPos::None;
        else
            ; // TODO: error: this is not possible

        newChord.hasBassNote = true;
    }

    // kind note
    XMLElement* kindElement = harmonyElement->FirstChildElement("kind");
    if (kindElement)
    {
        std::string typeString = XMLHelper::GetStringValue(kindElement, "");

        newChord.harmonyType = MusicXMLHelper::GetHarmonyTypeFromString(typeString);

        newChord.brackets = XMLHelper::GetBoolAttribute(kindElement, "bracket-degrees", newChord.brackets);
        newChord.parentheses = XMLHelper::GetBoolAttribute(kindElement, "parentheses-degrees", newChord.parentheses);
        newChord.stackDegrees = XMLHelper::GetBoolAttribute(kindElement, "stack-degrees", newChord.stackDegrees);
        newChord.useSymbols = XMLHelper::GetBoolAttribute(kindElement, "use-symbols", newChord.useSymbols);
        newChord.harmonyTypeText = XMLHelper::GetStringAttribute(kindElement, "text", newChord.harmonyTypeText);
    }
    else
        ; // TODO: error: this element is required

    // loop through all degree elements
    XMLNode* previousElement = harmonyElement->FirstChildElement(); // first element
    while (true)
    {
        if (previousElement) {
            XMLElement* element = previousElement->ToElement();
            const char* value = element->Value();
            if (strcmp(value, "degree") == 0)
            {
                XMLElement* degreeElement = element;
                ChordDegree newDegree = ChordDegree();

                BaseElementParser::ParsePrintableElement(degreeElement, newDegree);

                // degree value element
                XMLElement* degreeValueElement = degreeElement->FirstChildElement("degree-value");
                if (degreeValueElement)
                {
                    BaseElementParser::ParseVisibleElement(degreeValueElement, newDegree.degree);
                    newDegree.degree.degree = XMLHelper::GetUnsignedIntValue(degreeValueElement, newDegree.degree.degree);
                }

                // degree type element
                XMLElement* degreeTypeElement = degreeElement->FirstChildElement("degree-type");
                if (degreeTypeElement)
                {
                    BaseElementParser::ParseVisibleElement(degreeTypeElement, newDegree.degreeType);
                    std::string s = XMLHelper::GetStringValue(degreeTypeElement, "");
                    if (s == "add")
                        newDegree.degreeType.type = DegreeType::Type::Add;
                    else if (s == "subtract")
                        newDegree.degreeType.type = DegreeType::Type::Subtract;
                    else if (s == "alter")
                        newDegree.degreeType.type = DegreeType::Type::Alter;
                    else if (s.empty())
                        newDegree.degreeType.type = DegreeType::Type::None;
                    else
                        ; // TODO: error not a valid value
                }

                // degree alter element
                XMLElement* degreeAlterElement = degreeElement->FirstChildElement("degree-alter");
                if (degreeAlterElement)
                {
                    BaseElementParser::ParseVisibleElement(degreeAlterElement, newDegree.degreeAlter);
                    newDegree.degreeAlter.alter = XMLHelper::GetFloatValue(degreeAlterElement, newDegree.degreeAlter.alter);
                }

                newChord.degrees.push_back(newDegree);
            }
        }
        else
        {
            break;
        }
        previousElement = previousElement->NextSiblingElement();
    }

    // frame element (chord diagram)
    XMLElement* frameElement = harmonyElement->FirstChildElement("frame");
    ParseFrameElement(frameElement, newChord);

    newChord.beatPosition = currentTimeInMeasure; // note: harmony elements don't increment the time
    //newChord.CalculateChordName();
    //LOGE("Chord (%s) is at %f", newChord.chordName.string.c_str(), newChord.beatPosition);
    currentMeasures[newChord.staff - 1]->chords.push_back(newChord);
}

Barline MusicXMLParser::ParseBarlineElement(XMLElement* barlineElement)
{
    Barline barline = Barline();

    if (barlineElement)
    {
        std::string barlineLocationString = XMLHelper::GetStringAttribute(barlineElement, "location", "right");

        if (barlineLocationString == "right")
            barline.location = Barline::Location::Right;
        else if (barlineLocationString == "left")
            barline.location = Barline::Location::Left;
        else if (barlineLocationString == "middle")
            barline.location = Barline::Location::Middle;

        XMLElement* barStyleElement = barlineElement->FirstChildElement("bar-style");
        if (barStyleElement)
        {
            std::string barStyleString = XMLHelper::GetStringValue(barStyleElement, "", true);

            if (barStyleString == "dashed")
                barline.barlineStyle = Barline::BarlineStyle::Dashed;
            else if (barStyleString == "dotted")
                barline.barlineStyle = Barline::BarlineStyle::Dotted;
            else if (barStyleString == "heavy")
                barline.barlineStyle = Barline::BarlineStyle::Heavy;
            else if (barStyleString == "heavy-heavy")
                barline.barlineStyle = Barline::BarlineStyle::HeavyHeavy;
            else if (barStyleString == "heavy-light")
                barline.barlineStyle = Barline::BarlineStyle::HeavyLight;
            else if (barStyleString == "light-heavy")
                barline.barlineStyle = Barline::BarlineStyle::LightHeavy;
            else if (barStyleString == "light-light")
                barline.barlineStyle = Barline::BarlineStyle::LightLight;
            else if (barStyleString == "none")
                barline.barlineStyle = Barline::BarlineStyle::NoBarline;
            else if (barStyleString == "regular")
                barline.barlineStyle = Barline::BarlineStyle::Regular;
            else if (barStyleString == "short")
                barline.barlineStyle = Barline::BarlineStyle::Short;
            else if (barStyleString == "tick")
                barline.barlineStyle = Barline::BarlineStyle::Tick;
            else
                barline.barlineStyle = Barline::BarlineStyle::None;
        }

        XMLElement* repeatElement = barlineElement->FirstChildElement("repeat");
        if (repeatElement)
        {
            std::string directionString = XMLHelper::GetStringAttribute(repeatElement, "direction", "", true);

            if (directionString == "backward")
                barline.facing = Barline::Direction::Backward;
            else if (directionString == "forward")
                barline.facing = Barline::Direction::Forward;
            else
                barline.facing = Barline::Direction::None;

            barline.repeatCount = XMLHelper::GetUnsignedIntAttribute(repeatElement, "times", barline.repeatCount);

            barline.isRepeatBarLine = true;
        }
    }

    return barline;
}

void MusicXMLParser::ParseArpeggioElement(XMLElement* element, const std::shared_ptr<Measure>& currentMeasure, const std::shared_ptr<Note>& currentNote)
{
    if (element)
    {
        if (currentNote->beatPosition != currentArpeggiosBeatPosition || currentNote->measureIndex != currentArpeggiosMeasureIndex)
            currentArpeggios.clear();

        int number = XMLHelper::GetNumberAttribute(element, "number", 1);

        if (currentArpeggios.find(number) != currentArpeggios.end()) // an arpeggio exists at 'number'
        {

            std::shared_ptr<Arpeggio> arpeggio = currentArpeggios[number];

            arpeggio->notes.push_back(currentNote);
        }
        else // create new arpeggio
        {
            std::shared_ptr<Arpeggio> arpeggio = std::make_shared<Arpeggio>();

            BaseElementParser::ParseVisibleElement(element, arpeggio);

            arpeggio->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", arpeggio->defaultPositionStart);
            arpeggio->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", arpeggio->relativePositionStart);

            std::string directionString = XMLHelper::GetStringAttribute(element, "direction", "");

            if (directionString.empty())
                arpeggio->type = Arpeggio::ArpeggioType::Arpeggio;
            else if (directionString == "up")
                arpeggio->type = Arpeggio::ArpeggioType::ArpeggioUpArrow;
            else if (directionString == "down")
                arpeggio->type = Arpeggio::ArpeggioType::ArpeggioDownArrow;

            arpeggio->notes.push_back(currentNote);

            currentMeasure->arpeggios.push_back(arpeggio);

            currentArpeggios[number] = arpeggio;
            currentArpeggiosBeatPosition = currentNote->beatPosition;
            currentArpeggiosMeasureIndex = currentNote->measureIndex;
        }
    }
}

std::shared_ptr<Marker> MusicXMLParser::ParseCodaSegnoElement(XMLElement* element)
{
    if (!element)
        throw IsNullException();

    std::shared_ptr<Marker> newMarker = std::make_shared<Marker>();

    BaseElementParser::ParseVisibleElement(element, newMarker);

    newMarker->defaultPosition = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", newMarker->defaultPosition);
    newMarker->relativePosition = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", newMarker->relativePosition);

    const char* value = element->Value();
    if (strcmp(value, "coda") == 0)
        newMarker->type = Marker::MarkerType::Coda;
    else if (strcmp(value, "segno") == 0)
        newMarker->type = Marker::MarkerType::Segno;

    return newMarker;
}

void MusicXMLParser::ParseSlurElement(XMLElement* element, const std::shared_ptr<Measure>& currentMeasure, const std::shared_ptr<Note>& currentNote)
{
    if (element)
    {
        StartStopType startStopType = MusicXMLHelper::GetStartStopAttribute(element, "type", StartStopType::None, true);
        int number = XMLHelper::GetNumberAttribute(element, "number", 1);

        if (startStopType == StartStopType::Start)
        {
            std::shared_ptr<Slur> newSlur = std::make_shared<Slur>();

            newSlur->placement = MusicXMLHelper::GetAboveBelowAttribute(element, "placement", newSlur->placement);

            CurveOrientation defaultOrientation = newSlur->orientation;
            if (newSlur->placement == AboveBelowType::Above)
                defaultOrientation = CurveOrientation::Over;
            else if (newSlur->placement == AboveBelowType::Below)
                defaultOrientation = CurveOrientation::Under;
            newSlur->orientation = MusicXMLHelper::GetCurveOrientationAttribute(element, "orientation", defaultOrientation);

            newSlur->notes.push_back(currentNote);

            newSlur->startMeasureIndex = currentMeasure->index;

            currentMeasure->slurs.push_back(newSlur);
            currentSlurs[number] = newSlur;
        }
        else if (startStopType == StartStopType::Stop)
        {
            std::shared_ptr<Slur> slur = currentSlurs[number];

            slur->notes.push_back(currentNote);

            slur->endMeasureIndex = currentMeasure->index;
        }
        else if (startStopType == StartStopType::Continue) // TODO: implement?
        {
            std::shared_ptr<Slur> slur = currentSlurs[number];
        }
    }
}

void MusicXMLParser::ParseEndingElement(XMLElement* element, const std::shared_ptr<Song>& song, std::vector<std::shared_ptr<Measure>> currentMeasures)
{
    if (!element)
        throw IsNullException();

    std::string typeString = XMLHelper::GetStringAttribute(element, "type", "", true);
    std::string endingNumbersString = XMLHelper::GetStringAttribute(element, "number", "", true);

    if (typeString == "start")
    {
        std::shared_ptr<Ending> newEnding = std::make_shared<Ending>();

        BaseElementParser::ParseTextualElement(element, newEnding);
        BaseElementParser::ParseLineElement(element, newEnding);

        newEnding->defaultPosition = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", newEnding->defaultPosition);
        newEnding->relativePosition = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", newEnding->relativePosition);
        newEnding->defaultTextPosition = XMLHelper::GetFloatVec2Attribute(element, "text-x", "text-y", newEnding->defaultTextPosition);

        newEnding->jogLength = XMLHelper::GetFloatAttribute(element, "end-length", newEnding->jogLength);

        std::string endingNumbersDisplayText = XMLHelper::GetStringValue(element, "");

        if (endingNumbersDisplayText.empty())
            newEnding->endingNumbersText = endingNumbersString;
        else
            newEnding->endingNumbersText = endingNumbersDisplayText;

        newEnding->endingNumbers = XMLHelper::ParseIntList(endingNumbersString);

        if (!currentMeasures.empty())
        {
            newEnding->startMeasureIndex = currentMeasures[0]->index;
            newEnding->endMeasureIndex = currentMeasures[0]->index;
        }
        else
        {
            newEnding->startMeasureIndex = 0;
            newEnding->endMeasureIndex = 0;
        }

        if (song)
        {
            if (currentEndingGroup == nullptr)
            {
                currentEndingGroup = std::make_shared<EndingGroup>();
                song->endingGroups.push_back(currentEndingGroup);
            }
            else
            {
                bool createNewEndingGroup = false;
                for (const auto& ending : currentEndingGroup->endings)
                {
                    for (int number1 : ending->endingNumbers)
                    {
                        for (int number2 : newEnding->endingNumbers)
                        {
                            if (number1 == number2)
                            {
                                createNewEndingGroup = true;
                            }
                        }
                    }
                }

                if (createNewEndingGroup)
                {
                    if (!currentEndingGroup->endings.empty())
                    {
                        currentEndingGroup->endings[currentEndingGroup->endings.size() - 1]->isLastEndingInGroup = true;
                    }

                    currentEndingGroup = std::make_shared<EndingGroup>();
                    song->endingGroups.push_back(currentEndingGroup);
                }
            }

            currentEndingGroup->endings.push_back(newEnding);
        }

        currentEndings[endingNumbersString] = newEnding;
    }
    else if (typeString == "stop")
    {
        std::shared_ptr<Ending> ending = currentEndings[endingNumbersString];

        if (ending)
        {
            ending->hasFinalDownwardJog = true;

            if (!currentMeasures.empty())
            {
                ending->endMeasureIndex = currentMeasures[0]->index;
            }
        }
    }
    else if (typeString == "discontinue")
    {
        std::shared_ptr<Ending> ending = currentEndings[endingNumbersString];

        if (ending)
        {
            ending->hasFinalDownwardJog = false;

            if (!currentMeasures.empty())
            {
                ending->endMeasureIndex = currentMeasures[0]->index;
            }
        }
    }
    else
    {
        throw InvalidValueException("The only valid values for a ending element are: 'start', 'stop', and 'discontinue'. This value is not in that list: '" + typeString + "'.");
    }
}

void MusicXMLParser::ParsePart(const std::shared_ptr<Song>& song, XMLElement* part, bool isFirstPart, MusicDisplayConstants displayConstants)
{
    std::string id = part->Attribute("id");
    std::shared_ptr<Instrument> currentInst = song->GetInstrument(id);
    if (currentInst == nullptr) {
        AddError("Error", "Instrument " + id + " does not exist");
        throw std::exception();
    }

    // measures

    int measureIndex = 0;
    int systemIndex = 0;
    int previousSystemIndex = -1;

    bool firstMeasure = true;
    std::vector<bool> staffIsTabInfo;
    bool stavesCreated = false;

//                    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
//                    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
//                    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure res


    XMLNode* previousMeasureElement = part->FirstChildElement("measure");
    std::vector<std::shared_ptr<Measure>> previousMeasures;

    while (true)
    {
        if (previousMeasureElement)
        {
            XMLElement* measure = previousMeasureElement->ToElement();

            int measureNumber = XMLHelper::GetNumberAttribute(measure, "number", 0, true);
            float measureWidth = XMLHelper::GetFloatAttribute(measure, "width", 1.0f);
            //LOGW("number: %d, measureWidth: %f", measureNumber, measureWidth);

            bool implicitMeasure = XMLHelper::GetBoolAttribute(measure, "implicit", false);

            bool startNewSystem = false;
            bool startNewPage = false;

            std::vector<std::shared_ptr<Measure>> currentMeasures;

            // adding staves
            if (firstMeasure)
            {
                XMLElement* attributes = measure->FirstChildElement("attributes");
                if (attributes)
                {
                    // staves
                    int numStaves = 1;
                    XMLElement* stavesElement = attributes->FirstChildElement("staves");
                    if (stavesElement)
                    {
                        numStaves = ToInt(stavesElement->GetText());
                    }

                    // adding staves
                    for (int i = 0; i < numStaves; i++)
                    {
                        currentInst->staves.push_back(std::make_shared<Staff>());

                        staffIsTabInfo.push_back(false);
                    }

                    stavesCreated = true;
                }
            }

            if (currentInst->staves.size() >= 2)
            {
                currentInst->instrumentBracket = std::make_shared<InstrumentBracket>();
                currentInst->instrumentBracket->type = InstrumentBracket::InstrumentBracketType::Brace;
            }

            // creating measures for each staff
            for (int i = 0; i < currentInst->staves.size(); i++)
            {
                std::shared_ptr<Measure> newMeasure = std::make_shared<Measure>();
                newMeasure->measureNumber = MeasureNumber(measureNumber);
                newMeasure->implicit = implicitMeasure;
                newMeasure->index = measureIndex;
                newMeasure->staff = i+1;
                newMeasure->defaultMeasureWidth = measureWidth;

                if (stavesCreated)
                {
                    if (staffIsTabInfo[i])
                        newMeasure->type = Measure::MeasureType::Tab;
                }

                if (i < previousMeasures.size())
                {
                    if (previousMeasures[i]->isMeasureRepeat && previousMeasures[i]->measureRepeat != nullptr)
                    {
                        newMeasure->isMeasureRepeat = true;
                        newMeasure->measureRepeat = std::make_shared<MeasureRepeat>();
                        newMeasure->measureRepeat->measureRepeatLength = previousMeasures[i]->measureRepeat->measureRepeatLength;
                        newMeasure->measureRepeat->measureRepeatSlashes = previousMeasures[i]->measureRepeat->measureRepeatSlashes;
                    }
                }

                currentMeasures.push_back(newMeasure);
            }

            /*if (multiMeasureRest)
            {
                if (measureIndex - measureThatStartedMultiMeasureRest < numberOfMeasuresInMultiMeasureRest)

            }*/

            // print
            XMLElement* print = measure->FirstChildElement("print");
            if (print)
            {
                startNewSystem = XMLHelper::GetBoolAttribute(print, "new-system", startNewSystem);
                startNewPage = XMLHelper::GetBoolAttribute(print, "new-page", startNewPage);
                startNewSystem |= startNewPage;

                if (firstMeasure)
                    startNewSystem = true, startNewPage = true;

                for (const auto& m : currentMeasures) { m->startNewSystem = startNewSystem; m->startNewPage = startNewPage; }

                if ((firstMeasure || startNewSystem) && isFirstPart)
                {
                    std::shared_ptr<System> system = std::make_shared<System>();
                    System::SystemLayout systemLayout = displayConstants.systemLayout;

                    XMLElement* systemLayoutElement = print->FirstChildElement("system-layout");
                    if (systemLayoutElement)
                    {
                        systemLayout.systemDistance = XMLHelper::GetFloatValue("system-distance", systemLayoutElement, systemLayout.systemDistance);
                        systemLayout.topSystemDistance = XMLHelper::GetFloatValue("top-system-distance", systemLayoutElement, systemLayout.topSystemDistance);

                        XMLElement* systemLayoutMarginsElement = systemLayoutElement->FirstChildElement("system-margins");
                        if (systemLayoutMarginsElement)
                        {
                            systemLayout.systemLeftMargin = XMLHelper::GetFloatValue("left-margin", systemLayoutMarginsElement, systemLayout.systemLeftMargin, true); // required
                            systemLayout.systemRightMargin = XMLHelper::GetFloatValue("right-margin", systemLayoutMarginsElement, systemLayout.systemRightMargin, true); // required
                        }
                    }

                    //LOGE("systemLayout; i: %d, leftMargin: %f", song->systemLayouts.size(), systemLayout.systemLeftMargin);

                    system->layout = systemLayout;

                    if (firstMeasure)
                        system->showBeginningTimeSignature = true;

                    system->showBeginningClef = true;
                    system->showBeginningKeySignature = true;

                    system->beginningMeasureIndex = measureIndex;

                    if (previousSystemIndex != -1 && previousSystemIndex < song->systems.size())
                    {
                        song->systems[previousSystemIndex]->endingMeasureIndex = measureIndex - 1;

                        for (int i = song->systems[previousSystemIndex]->beginningMeasureIndex; i <= measureIndex - 1; i++)
                        {
                            SystemMeasure newSystemMeasure;
                            newSystemMeasure.measureIndex = i;
                            song->systems[previousSystemIndex]->systemMeasures.push_back(newSystemMeasure);
                        }
                    }

                    song->systems.push_back(system);

                    previousSystemIndex = systemIndex;
                    systemIndex++;
                }

                XMLElement* staffLayoutElement = print->FirstChildElement("staff-layout");
                if (staffLayoutElement)
                {
                    unsigned int staffNumber = XMLHelper::GetUnsignedIntAttribute(staffLayoutElement, "number", 1); // the number of the staff that this layout applies to

                    // staff distance (the distance from the bottom line of the previous staff to the top line of the current staff)
                    currentMeasures[staffNumber-1]->defStaffDistance = XMLHelper::GetFloatValue("staff-distance", staffLayoutElement, currentMeasures[staffNumber-1]->defStaffDistance);
                }
            }

            float currentTimeInMeasure = 0.0f; // keeps track of the current time that has passed in the current measure in beats(quarter notes)

            // attributes
            XMLElement* attributes = measure->FirstChildElement("attributes");
            if (attributes)
            {
                // divisions
                XMLElement* divisions = attributes->FirstChildElement("divisions");
                if (divisions)
                {
                    for (const auto& m : currentMeasures) { m->divisions = ToInt(divisions->GetText()); } // setting divisions for all current measures
                }
                else
                {
                    if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                    {
                        int i = 0;
                        for (const auto& m : currentMeasures)
                        {
                            m->divisions = previousMeasures[i]->divisions;
                            i++;
                        }
                    }
                }

                // key signature
                XMLElement* keySignatureElement = attributes->FirstChildElement("key");
                if (keySignatureElement)
                {
                    KeySignature keySignature = KeySignature(0);
                    // print object
                    keySignature.print = XMLHelper::GetBoolAttribute(keySignatureElement, "print-object", true);

                    // fifths
                    XMLElement* fifths = keySignatureElement->FirstChildElement("fifths");
                    if (fifths)
                        keySignature.fifths = ToInt(fifths->GetText());

                    // mode
                    XMLElement* mode = keySignatureElement->FirstChildElement("mode");
                    if (mode)
                        keySignature.mode = KeySignature::GetModeFromString(mode->GetText());

                    for (const auto& m : currentMeasures) { m->keySignature = keySignature; m->showKeySignature = true; }
                }
                else
                {
                    if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                    {
                        int i = 0;
                        for (const auto& m : currentMeasures)
                        {
                            m->keySignature = previousMeasures[i]->keySignature;
                            i++;
                        }
                    }
                }

                // time signature
                XMLElement* timeSignatureElement = attributes->FirstChildElement("time");
                if (timeSignatureElement)
                {
                    TimeSignature timeSignature = TimeSignature();
                    // print object
                    timeSignature.printObject = XMLHelper::GetBoolAttribute(timeSignatureElement, "print-object", true);

                    const char* symbol = timeSignatureElement->Attribute("symbol");
                    if (symbol)
                        timeSignature.displayType = TimeSignature::GetDisplayTypeFromString(symbol);

                    XMLElement* beats = timeSignatureElement->FirstChildElement("beats");
                    if (beats)
                        timeSignature.notes = ToInt(beats->GetText());
                    XMLElement* beatType = timeSignatureElement->FirstChildElement("beat-type");
                    if (beatType)
                        timeSignature.noteType = ToInt(beatType->GetText());

                    for (const auto& m : currentMeasures) { m->timeSignature = timeSignature; m->showTimeSignature = true; m->CalculateDuration(); }
                }
                else
                {
                    if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                    {
                        int i = 0;
                        for (const auto& m : currentMeasures)
                        {
                            m->timeSignature = previousMeasures[i]->timeSignature;
                            m->CalculateDuration();
                            i++;
                        }
                    }
                }

                // clef
                XMLNode* previousClefElement = attributes->FirstChildElement("clef");
                if (previousClefElement)
                {
                    while (true)
                    {
                        if (previousClefElement)
                        {
                            XMLElement* clefElement = previousClefElement->ToElement();
                            int staffNumber = XMLHelper::GetNumberAttribute(clefElement, "number", 1);

                            XMLElement* signElement = clefElement->FirstChildElement("sign");
                            if (signElement)
                            {
                                currentMeasures[staffNumber-1]->clef.sign = signElement->GetText();
                                if (currentMeasures[staffNumber-1]->clef.sign == "TAB")
                                {
                                    if (firstMeasure) {
                                        currentInst->staves[staffNumber-1]->type = Staff::StaffType::Tab;
                                        staffIsTabInfo[staffNumber-1] = true;
                                        currentMeasures[staffNumber-1]->type = Measure::MeasureType::Tab;

                                        if (currentInst->staves.size() > 1) // if there is more than one staff
                                            currentInst->staves[staffNumber-1]->tablatureDisplayType = TablatureDisplayType::NoRhythm;
                                    }
                                }
                            }

                            XMLElement* line = clefElement->FirstChildElement("line");
                            if (line)
                                currentMeasures[staffNumber-1]->clef.line = ToInt(line->GetText());

                            currentMeasures[staffNumber-1]->clef.octaveChange = XMLHelper::GetIntValue("clef-octave-change", clefElement, currentMeasures[staffNumber-1]->clef.octaveChange);
                        }
                        else // no more clefs
                        {
                            break;
                        }
                        previousClefElement = previousClefElement->NextSiblingElement("clef");
                    }
                }

                // change the measure's clef the the previous measure's clef if a new clef was not specified (and set the clef changed attribute)
                int i = 0;
                for (const auto& m : currentMeasures)
                {
                    if (i < previousMeasures.size())
                    {
                        if (m->clef.sign.empty())
                        {
                            m->clef = previousMeasures[i]->clef;
                            m->clef.clefChanged = false;
                        }
                        else if (m->clef != previousMeasures[i]->clef)
                        {
                            m->clef.clefChanged = true;
                            m->clef.showClef = true;
                        }
                    }

                    i++;
                }

                // transpose
                XMLElement* transposeElement = attributes->FirstChildElement("transpose");
                if (transposeElement)
                {
                    Transpose transpose = Transpose();
                    // diatonic
                    XMLElement* diatonic = transposeElement->FirstChildElement("diatonic");
                    if (diatonic)
                    {
                        transpose.diatonic = ToInt(diatonic->GetText());
                    }

                    // chromatic
                    XMLElement* chromatic = transposeElement->FirstChildElement("chromatic");
                    if (chromatic)
                    {
                        transpose.chromatic = ToInt(chromatic->GetText());
                    }

                    // octave change
                    XMLElement* octaveChange = transposeElement->FirstChildElement("octave-change");
                    if (octaveChange)
                    {
                        transpose.octaveChange = ToInt(octaveChange->GetText());
                    }

                    for (const auto& m : currentMeasures) { m->transpose = transpose; }
                }
                else
                {
                    if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                    {
                        int j = 0;
                        for (const auto& m : currentMeasures)
                        {
                            m->transpose = previousMeasures[j]->transpose;
                            j++;
                        }
                    }
                }

                // staff details
                XMLElement* staffDetails = attributes->FirstChildElement("staff-details");
                if (staffDetails)
                {
                    // staff lines
                    XMLElement* staffLines = staffDetails->FirstChildElement("staff-lines");
                    if (staffLines)
                        currentInst->staves.back()->lines = ToInt(staffLines->GetText());

                    // staff tunings (TAB only)
                    if (currentInst->staves.back()->type == Staff::StaffType::Tab) {
                        XMLNode *previousStaffTuning = staffDetails->FirstChildElement("staff-tuning");

                        std::shared_ptr<Staff> tabStaff = currentInst->staves.back();

                        while (true) {
                            if (previousStaffTuning) {
                                XMLElement *staffTuning = previousStaffTuning->ToElement();
                                XMLElement *tuningStep = staffTuning->FirstChildElement(
                                        "tuning-step");
                                StaffTuning tuning;
                                if (tuningStep)
                                    tuning.pitch.step = DiatonicNoteFromString(tuningStep->GetText());
                                XMLElement *tuningOctave = staffTuning->FirstChildElement(
                                        "tuning-octave");
                                if (tuningOctave)
                                    tuning.pitch.octave = ToInt(
                                            tuningOctave->GetText());
                                XMLElement *tuningAlter = staffTuning->FirstChildElement(
                                        "tuning-alter");
                                if (tuningAlter)
                                    tuning.pitch.alter = (float)ToInt(
                                            tuningAlter->GetText());
                                tabStaff->tunings.push_back(tuning);
                            } else {
                                break;
                            }
                            previousStaffTuning = previousStaffTuning->NextSiblingElement("staff-tuning");
                        }

                        tabStaff->capo = XMLHelper::GetUnsignedIntValue("capo", staffDetails, tabStaff->capo);
                    }
                } // staff details

                // measure style
                XMLElement* measureStyleElement = attributes->FirstChildElement("measure-style");
                if (measureStyleElement)
                {
                    int staffNumber = XMLHelper::GetNumberAttribute(measureStyleElement, "number", -1);

                    // measure rest
                    XMLElement* multipleRestElement = measureStyleElement->FirstChildElement("multiple-rest");
                    if (multipleRestElement)
                    {
                        unsigned int numberOfMeasures = XMLHelper::GetUnsignedIntValue(multipleRestElement, 1);
                        bool useSymbols = XMLHelper::GetBoolAttribute(multipleRestElement, "use-symbols", false);

                        if (staffNumber == -1) // applies to all the staves
                        {
                            for (const auto& m : currentMeasures)
                            {
                                m->startsMultiMeasureRest = true;
                                m->isPartOfMultiMeasureRest = true;
                                m->multiMeasureRestSymbol = std::make_shared<MultiMeasureRestSymbol>();
                                m->multiMeasureRestSymbol->useSymbols = useSymbols;
                                m->multiMeasureRestSymbol->numberOfMeasures = numberOfMeasures;
                            }
                        }
                        else // applies only to the specified staff
                        {
                            currentMeasures[staffNumber-1]->startsMultiMeasureRest = true;
                            currentMeasures[staffNumber-1]->isPartOfMultiMeasureRest = true;
                            currentMeasures[staffNumber-1]->multiMeasureRestSymbol = std::make_shared<MultiMeasureRestSymbol>();
                            currentMeasures[staffNumber-1]->multiMeasureRestSymbol->useSymbols = useSymbols;
                            currentMeasures[staffNumber-1]->multiMeasureRestSymbol->numberOfMeasures = numberOfMeasures;
                        }
                    }

                    XMLElement* measureRepeatElement = measureStyleElement->FirstChildElement("measure-repeat");
                    if (measureRepeatElement)
                    {
                        std::string typeString = XMLHelper::GetStringAttribute(measureRepeatElement, "type", "", true);

                        if (typeString == "start")
                        {
                            currentMeasures[staffNumber-1]->isMeasureRepeat = true;
                            currentMeasures[staffNumber-1]->measureRepeat = std::make_shared<MeasureRepeat>();

                            currentMeasures[staffNumber-1]->measureRepeat->measureRepeatLength = XMLHelper::GetIntValue(measureRepeatElement, currentMeasures[staffNumber-1]->measureRepeat->measureRepeatLength);
                            currentMeasures[staffNumber-1]->measureRepeat->measureRepeatSlashes = XMLHelper::GetNumberAttribute(measureRepeatElement, "slashes", currentMeasures[staffNumber-1]->measureRepeat->measureRepeatSlashes);
                        }
                        else if (typeString == "stop")
                        {
                            currentMeasures[staffNumber-1]->isMeasureRepeat = false;
                            currentMeasures[staffNumber-1]->measureRepeat = nullptr;
                        }
                        else
                        {
                            throw InvalidValueException();
                        }
                    }
                }
            }
            else
            {
                if (firstMeasure)
                    currentInst->staves.push_back(std::make_shared<Staff>());

                if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                {
                    int i = 0;
                    for (const auto& m : currentMeasures)
                    {
                        m->timeSignature = previousMeasures[i]->timeSignature;
                        m->CalculateDuration();
                        m->keySignature = previousMeasures[i]->keySignature;
                        m->clef = previousMeasures[i]->clef;
                        m->divisions = previousMeasures[i]->divisions;
                        m->transpose = previousMeasures[i]->transpose;
                        i++;
                    }
                }
            }

            // notes and backup and forward elements

            XMLNode* previousElement = measure->FirstChildElement(); // the first element
            //XMLNode* previousNoteElement = measure->FirstChildElement("note");
            std::shared_ptr<Note> previousNote = nullptr;
            while (true)
            {
                if (previousElement)
                {
                    XMLElement* element = previousElement->ToElement();
                    const char* value = element->Value();
                    if (strcmp(value, "note") == 0) // note
                    {
                        std::shared_ptr<Note> currentNote = std::make_shared<Note>();
                        NoteElementParser::ParseNoteElement(element, currentTimeInMeasure, staffIsTabInfo, currentNote, previousNote, currentMeasures, measureNumber);
                        previousNote = currentNote;
                    }
                    else if (strcmp(value, "backup") == 0) // backup time in measure
                    {
                        // duration
                        XMLElement* durationElement = element->FirstChildElement("duration");
                        if (durationElement)
                        {
                            int divisions = currentMeasures[0]->divisions;
                            float duration = 0.0f;
                            if (divisions != 0)
                            {
                                duration = (1.0f / (float)divisions) * (float)ToInt(durationElement->GetText());
                            }
                            else
                            {
                                AddError("Invalid value", "Divisions is zero");
                            }

                            currentTimeInMeasure -= duration;
                        }
                        else
                        {
                            AddError("Missing Element", "Backup element has no duration");
                        }
                    }
                    else if (strcmp(value, "harmony") == 0) // harmony element (i.e. chords)
                    {
                        ParseHarmonyElement(element, currentTimeInMeasure, currentMeasures);
                    }
                    else if (strcmp(value, "forward") == 0) // increment time in measure
                    {
                        // duration
                        XMLElement* durationElement = element->FirstChildElement("duration");
                        if (durationElement)
                        {
                            int divisions = currentMeasures[0]->divisions;
                            float duration = 0.0f;
                            if (divisions != 0)
                            {
                                duration = (1.0f / (float)divisions) * (float)ToInt(durationElement->GetText());
                            }
                            else
                            {
                                AddError("Invalid value", "Divisions is zero");
                            }

                            currentTimeInMeasure += duration;
                        }
                        else
                        {
                            AddError("Missing Element", "Forward element has no duration");
                        }
                    }
                    else if (strcmp(value, "direction") == 0) // direction
                    {
                        // duration direction
                        bool isNewDurationDirection;
                        std::shared_ptr<DurationDirection> durationDirection = ParseDurationDirection(element, isNewDurationDirection, currentTimeInMeasure, measureIndex);
                        if (isNewDurationDirection && durationDirection)
                        {
                            /*durationDirection->beatPositionStart = currentTimeInMeasure;
                            durationDirection->startMeasureIndex = measureIndex;*/
                            currentInst->staves[0]->durationDirections.push_back(durationDirection);
                        }

                        if (!isNewDurationDirection)
                        {
                            // direction
                            bool isNewDirection;
                            Direction direction = ParseDirection(element, isNewDirection, currentTimeInMeasure);
                            if (isNewDirection)
                            {
                                direction.beatPosition = currentTimeInMeasure;
                                currentMeasures[0]->directions.push_back(direction);
                            }
                        }

                        // sound
                        XMLElement* soundElement = element->FirstChildElement("sound");
                        if (soundElement)
                        {
                            float dynamics = XMLHelper::GetFloatAttribute(soundElement, "dynamics", -1.0f);
                            float tempo = XMLHelper::GetFloatAttribute(soundElement, "tempo", -1.0f);
                            int pan = XMLHelper::GetNumberAttribute(soundElement, "pan", 0);

                            // getting beat position of sound event
                            float eventBeatPosition = currentTimeInMeasure;

                            if (dynamics != -1.0f)
                            {
                                std::shared_ptr<DynamicsSoundEvent> dynamicsSoundEvent = std::make_shared<DynamicsSoundEvent>();

                                dynamicsSoundEvent->beatPosition = eventBeatPosition;
                                dynamicsSoundEvent->SetDynamics(dynamics / 100.0f);

                                for (const auto& m : currentMeasures) { m->soundEvents.push_back(dynamicsSoundEvent); }
                            }

                            if (tempo != -1.0f)
                            {
                                std::shared_ptr<TempoSoundEvent> event = std::make_shared<TempoSoundEvent>();

                                event->beatPosition = eventBeatPosition;
                                event->tempo = tempo;

                                for (const auto& m : currentMeasures) { m->soundEvents.push_back(event); }
                            }
                        }
                    }
                    else if (strcmp(value, "barline") == 0) // barline
                    {
                        Barline barline = ParseBarlineElement(element);

                        for (const auto& m : currentMeasures) { m->barlines.push_back(barline); }

                        // ending
                        XMLElement* endingElement = element->FirstChildElement("ending");
                        if (endingElement)
                        {
                            ParseEndingElement(endingElement, song, currentMeasures);
                        }
                    }
                }
                else
                {
                    break;
                }
                previousElement = previousElement->NextSibling(); // get the next element
                //previousNoteElement = measure->IterateChildren("note", previousNoteElement);
            }

            // adding measure to staff
            /*LOGD("adding measures");
            int staffIndex = 0;
            for (Staff* staff : currentInst->staves)
            {
                LOGD("staff %i", staffIndex);
                Measure* newMeasure = new Measure();
                newMeasure->staff = staffIndex + 1;
                newMeasure->CopyData(currentMeasure);
                LOGD("measure %i, %i", newMeasure->notes.size(), currentMeasure->notes.size());
                currentInst->staves[staffIndex]->measures.push_back(newMeasure);
                staffIndex++;
            }
            LOGD("done adding meausres");
            if (previousMeasure != nullptr) {
                LOGD("deleting previous measure");
                previousMeasure->notes.clear();
                delete previousMeasure; // deleting previous measure (but not it's notes because they were all given to other measures)
                previousMeasure = nullptr;
            }
            LOGD("setting previous measure");*/

            // adding measures to correct staves
            for (const auto& m : currentMeasures)
            {
                currentInst->staves[m->staff-1]->measures.push_back(m);
            }

            previousMeasures = currentMeasures;
            firstMeasure = false;
            measureIndex++;
        }
        else
        {
            /*if (previousMeasure != nullptr) // delete
            {
                LOGD("deleting last previous measure");
                previousMeasure->notes.clear();
                delete previousMeasure; // deleting previous measure (but not it's notes because they were all given to other measures)
                previousMeasure = nullptr;
                LOGD("done deleting last previous measure");
            }*/
            break;
        }
        previousMeasureElement = previousMeasureElement->NextSiblingElement("measure");
    }
}

// main file parsing
void MusicXMLParser::ParseMusicXML(const std::string& data, const std::shared_ptr<Song>& song)
{
    LOGI_TAG("MusicXMLParser", "Starting to parse MusicXML file");

    m_Errors.clear();
    MusicDisplayConstants displayConstants;

    tinyxml2::XMLDocument doc;
    const char* d = data.c_str();
    tinyxml2::XMLError xmlError = doc.Parse(d);

    if (xmlError != tinyxml2::XMLError::XML_SUCCESS) {
        LOGE_TAG("MusicXMLParser", "error: %i", xmlError);
        AddError("XML Error", ToString(int(xmlError)));
    }

    if (doc.Error())
    {
        LOGE_TAG("MusicXMLParser", "doc error: %s: %s", doc.ErrorName(), doc.ErrorStr());
        AddError(doc.ErrorName(), doc.ErrorStr());
        LOGD_TAG("MusicXMLParser", "---------------------------Document:\n\n%s\n\n", data.c_str());
        throw ParseException("Document error");
    }

    // TODO: will create problems if it is socre-timewise
    XMLElement* root = doc.FirstChildElement("score-partwise");
    if (root)
    {
        song->musicXMLVersion = root->Attribute("version");

        // work
        XMLElement* workElement = root->FirstChildElement("work");
        if (workElement)
        {
            ParseWorkElement(workElement, song->songData.songTitle, song->songData.workNumber);
        }

        song->songData.movementNumber = XMLHelper::GetStringValue("movement-number", root, song->songData.movementNumber); // movement number
        song->songData.movementTitle = XMLHelper::GetStringValue("movement-title", root, song->songData.movementTitle); // movement title

        // identification
        XMLElement* identificationElement = root->FirstChildElement("identification");
        if (identificationElement)
        {
            ParseIdentificationElement(identificationElement, song);
        }

        // defaults
        XMLElement* defaultsElement = root->FirstChildElement("defaults");
        if (defaultsElement)
        {
            displayConstants = ParseDefaultsElement(defaultsElement);
        }

        // credits
        XMLNode* previousCreditNode = root->FirstChildElement("credit");
        while (true)
        {
            if (previousCreditNode)
            {
                XMLElement* creditElement = previousCreditNode->ToElement();
                song->credits.push_back(ParseCreditElement(creditElement));
            }
            else
            {
                break;
            }

            previousCreditNode = previousCreditNode->NextSiblingElement("credit");
        }

        // part list
        XMLElement* partList = root->FirstChildElement("part-list");
        if (partList)
        {
            // score parts
            XMLNode* previousScorePartElement = partList->FirstChildElement("score-part");
            while (true)
            {
                if (previousScorePartElement)
                {
                    XMLElement* scorePart = previousScorePartElement->ToElement();
                    std::shared_ptr<Instrument> instrument = std::make_shared<Instrument>();
                    song->instruments.push_back(instrument);

                    // part id
                    instrument->id = scorePart->Attribute("id");

                    // part name
                    XMLElement* partName = scorePart->FirstChildElement("part-name");
                    if (partName)
                    {
                        instrument->name.string = partName->GetText();
                        instrument->name.print = XMLHelper::GetBoolAttribute(partName, "print-object", true);
                    }

                    // part name abbreviation
                    XMLElement* partAbbreviation = scorePart->FirstChildElement("part-abbreviation");
                    if (partAbbreviation)
                    {
                        instrument->nameAbbreviation.string = partAbbreviation->GetText();
                        instrument->nameAbbreviation.print = XMLHelper::GetBoolAttribute(partAbbreviation, "print-object", true);
                    }

                    // midi instrument
                    XMLElement* midiInstrumentElement = scorePart->FirstChildElement("midi-instrument");
                    if (midiInstrumentElement)
                    {
                        instrument->midiInstrument.id = XMLHelper::GetStringAttribute(midiInstrumentElement, "id", "");

                        // midi channel
                        XMLElement* channelElement = midiInstrumentElement->FirstChildElement("midi-channel");
                        if (channelElement)
                        {
                            instrument->midiInstrument.channel = ToInt(channelElement->GetText()) - 1;
                        }

                        // midi program
                        XMLElement* programElement = midiInstrumentElement->FirstChildElement("midi-program");
                        if (programElement)
                        {
                            instrument->midiInstrument.program = ToInt(programElement->GetText()) - 1;
                        }

                        // volume
                        XMLElement* volumeElement = midiInstrumentElement->FirstChildElement("volume");
                        if (volumeElement)
                        {
                            instrument->midiInstrument.volume = ToInt(volumeElement->GetText());
                        }

                        // pan
                        XMLElement* panElement = midiInstrumentElement->FirstChildElement("pan");
                        if (panElement)
                        {
                            instrument->midiInstrument.pan = ToInt(panElement->GetText());
                        }
                    }
                }
                else
                {
                    break;
                }
                previousScorePartElement = previousScorePartElement->NextSiblingElement("score-part");
            }
        }
        else
        {
            AddError("Parse Failed", "No part-list");
        }


        // parts
        bool isFirstPart = true;
        XMLNode* previous = root->FirstChildElement("part");
        while (true) // looping through all parts
        {
            if (previous)
            {
                XMLElement* part = previous->ToElement();

                ParsePart(song, part, isFirstPart, displayConstants);

                isFirstPart = false;
            }
            else
            {
                break;
            }

            previous = previous->NextSiblingElement("part");
        }
    }
    else
    {
        AddError("Parse Error", "Failed to parse root element");
        throw ParseException("Failed to parse root element");
    }

    song->displayConstants = displayConstants;

    // TODO: fix dangerous line of code
    song->systems[song->systems.size() - 1]->endingMeasureIndex = song->instruments[0]->staves[0]->measures[song->instruments[0]->staves[0]->measures.size() - 1]->index;

    // update multi measure rests
    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure rest
    for (const auto& instrument : song->instruments)
    {
        for (const auto& staff : instrument->staves)
        {
            int measureIndex = 0;
            for (const auto& measure : staff->measures)
            {
                if (multiMeasureRest)
                {
                    if (measureIndex - measureThatStartedMultiMeasureRest < numberOfMeasuresInMultiMeasureRest) // this measure is part of the multi measure rest
                    {
                        measure->isPartOfMultiMeasureRest = true; // update
                    }
                    else // this measure is not part of the multi measure rest (so it has ended)
                    {
                        multiMeasureRest = false;
                        numberOfMeasuresInMultiMeasureRest = 0;
                        measureThatStartedMultiMeasureRest = 0;
                    }
                }

                if (measure->startsMultiMeasureRest)
                {
                    multiMeasureRest = true;
                    numberOfMeasuresInMultiMeasureRest = measure->multiMeasureRestSymbol->numberOfMeasures;
                    measureThatStartedMultiMeasureRest = measureIndex;
                }

                measureIndex++;
            }
        }
    }

    if (currentEndingGroup)
    {
        if (!currentEndingGroup->endings.empty())
        {
            currentEndingGroup->endings[currentEndingGroup->endings.size() - 1]->isLastEndingInGroup = true;
        }
    }

    LOGD("Finished");
    doc.Clear();

    currentDynamicWedges.clear();
    currentBrackets.clear();
    currentDashes.clear();

    ParseError::PrintErrors();
}