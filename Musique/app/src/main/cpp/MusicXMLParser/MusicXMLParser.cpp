#include "MusicXMLParser.h"
#include "../Utils/Converters.h"
#include <memory>

#include "../Vec2.h"

static std::unordered_map<int, std::shared_ptr<DynamicWedge>> currentDynamicWedges;
static std::unordered_map<int, std::shared_ptr<DashesDirection>> currentDashes;
static std::unordered_map<int, std::shared_ptr<BracketDirection>> currentBrackets;

// ---- Parse Functions ----

Lyric::SyllabicType MusicXMLParser::ParseSyllabicType(const std::string& value)
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

// parses a single lyric element
Lyric MusicXMLParser::ParseLyric(XMLElement* lyricElement)
{
    Lyric lyric = Lyric();
    if (lyricElement)
    {
        lyric.defX = XMLHelper::GetFloatAttribute(lyricElement, "default-x", lyric.defX);
        lyric.defY = XMLHelper::GetFloatAttribute(lyricElement, "default-y", lyric.defY);

        lyric.relX = XMLHelper::GetFloatAttribute(lyricElement, "relative-x", lyric.relX);
        lyric.relY = XMLHelper::GetFloatAttribute(lyricElement, "relative-y", lyric.relY);

        lyric.number = XMLHelper::GetNumberAttribute(lyricElement, "number", lyric.number);

        // loop through all syllabic and text elements
        XMLNode* previousElement = lyricElement->FirstChildElement(); // first element
        while (true)
        {
            if (previousElement) {
                XMLElement* element = previousElement->ToElement();
                const char* value = element->Value();
                if (strcmp(value, "syllabic") == 0) // syllabic
                {
                    XMLElement* syllabicElement = element;
                    lyric.syllabics.push_back(ParseSyllabicType(syllabicElement->GetText())); // parse type and add to syllabic vector
                }
                else if (strcmp(value, "text") == 0) // text
                {
                    XMLElement* textElement = element;
                    LyricText text = LyricText();
                    text.text = textElement->GetText();
                    lyric.text.push_back(text);
                }
                else
                {
                    AddError("Unrecognized element", "Didn't recognize element in LYRIC");
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }
    }
    else
    {
        LOGE("lyricElement is null");
    }
    return lyric;
}

void MusicXMLParser::ParseBaseElement(XMLElement* element, BaseElement& newElement)
{
    if (element)
    {
        // TODO: implement
    }
}

void MusicXMLParser::ParsePrintableElement(XMLElement* element, PrintableElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParseBaseElement(element, newElement);
    }
}

void MusicXMLParser::ParseVisibleElement(XMLElement* element, VisibleElement& newElement)
{
    if (element)
    {
        // TODO: implement
        ParsePrintableElement(element, newElement);
    }
}

void MusicXMLParser::ParseTextualElement(XMLElement* element, TextualElement& newElement)
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

void MusicXMLParser::ParseLineElement(XMLElement* element, LineElement& newElement)
{
    if (element)
    {
        std::string lineTypeString = XMLHelper::GetStringAttribute(element, "line-type", "solid");

        if (lineTypeString == "dashed")
            newElement.lineType = LineType::Dashed;
        else if (lineTypeString == "dotted")
            newElement.lineType = LineType::Dotted;
        else if (lineTypeString == "solid")
            newElement.lineType = LineType::Solid;
        else if (lineTypeString == "wavy")
            newElement.lineType = LineType::Wavy;
        else
            AddError("Unrecognized type", "Did not recognize line type");

        newElement.dashLength = XMLHelper::GetFloatAttribute(element, "dash-length", newElement.dashLength);
        newElement.dashSpaceLength = XMLHelper::GetFloatAttribute(element, "space-length", newElement.dashSpaceLength);

        ParseBaseElement(element, newElement);
    }
}

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
        words.text.string = element->GetText();

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

        ParseTextualElement(element, words);
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
        ParseTextualElement(element, dynamic);

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

std::shared_ptr<DynamicWedge> MusicXMLParser::ParseDynamicWedgeElement(XMLElement* element, float currentTimeInMeasure)
{
    if (element)
    {
        int number = XMLHelper::GetNumberAttribute(element, "number", currentDynamicWedges.size() + 1) - 1; // starts at 0
        std::string wedgeType = XMLHelper::GetStringAttribute(element, "type", "", true);


        if (wedgeType == "crescendo" || wedgeType == "diminuendo")
        {
            LOGW("wedge crescendo or diminuendo");

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
            LOGW("wedge stop");

            auto dynamicWedge = currentDynamicWedges[number];

            dynamicWedge->defaultPositionEnd = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dynamicWedge->defaultPositionEnd);
            dynamicWedge->relativePositionEnd = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dynamicWedge->relativePositionEnd);

            if (dynamicWedge->type == DynamicWedge::WedgeType::Crescendo) // the end of a crescendo
                dynamicWedge->defaultSpread = XMLHelper::GetFloatAttribute(element, "spread", dynamicWedge->defaultSpread);

            dynamicWedge->beatPositionEnd = currentTimeInMeasure;

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

std::shared_ptr<DashesDirection> MusicXMLParser::ParseDashesDirectionElement(XMLElement* element, float currentTimeInMeasure)
{
    if (element)
    {
        int number = XMLHelper::GetNumberAttribute(element, "number", currentDashes.size() + 1) - 1; // starts at 0
        std::string typeString = XMLHelper::GetStringAttribute(element, "type", "", true);


        if (typeString == "start")
        {
            LOGW("start of dashes");

            std::shared_ptr<DashesDirection> dashes = std::make_shared<DashesDirection>();

            ParseVisibleElement(element, *dashes);
            ParseLineElement(element, *dashes);

            dashes->defaultPositionStart = XMLHelper::GetFloatVec2Attribute(element, "default-x", "default-y", dashes->defaultPositionStart);
            dashes->relativePositionStart = XMLHelper::GetFloatVec2Attribute(element, "relative-x", "relative-y", dashes->relativePositionStart);

            // flip y so that positive y is down (not up)
            MusicXMLHelper::FlipYInVec2(dashes->defaultPositionStart);
            MusicXMLHelper::FlipYInVec2(dashes->relativePositionStart);

            dashes->beatPositionStart = currentTimeInMeasure;

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

Direction MusicXMLParser::ParseDirection(XMLElement* directionElement, bool& isNewDirection, float currentTimeInMeasure)
{
    isNewDirection = true;

    Direction direction = Direction();
    if (directionElement)
    {
        XMLElement* directionTypeElement = directionElement->FirstChildElement("direction-type");
        if (directionTypeElement)
        {
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
            XMLElement* dynamicWedgeElement = directionTypeElement->FirstChildElement("wedge");
            if (dynamicWedgeElement)
            {
                direction.dynamicWedge = ParseDynamicWedgeElement(dynamicWedgeElement, currentTimeInMeasure);
                if (direction.dynamicWedge == nullptr)
                    isNewDirection = false;
            }

            // dashes
            XMLElement* dashesDirectionElement = directionTypeElement->FirstChildElement("dashes");
            if (dashesDirectionElement)
            {
                direction.dashesDirection = ParseDashesDirectionElement(dashesDirectionElement, currentTimeInMeasure);
                if (direction.dashesDirection == nullptr)
                    isNewDirection = false;
            }
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

void MusicXMLParser::ParseEncodingElement(XMLElement* encodingElement, Song* song)
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

void MusicXMLParser::ParseIdentificationElement(XMLElement* idElement, Song* song)
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
        ParseBaseElement(creditElement, credit);

        credit.pageNumber = XMLHelper::GetUnsignedIntAttribute(creditElement, "page", credit.pageNumber);

        XMLElement* creditWordsElement = creditElement->FirstChildElement("credit-words");
        if (creditWordsElement)
        {
            CreditWords words = CreditWords();
            ParseTextualElement(creditWordsElement, words);
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
        /*ChordDiagram chordDiagram;

        XMLElement* rootStepElement = frameElement->FirstChildElement("root-step");

        chordDiagram.string = GetUnsignedIntValue("frame-strings", frameElement, 0);
        chordDiagram.frets = GetUnsignedIntValue("frame-frets", frameElement, 0);

        XMLElement* firstFretElement = frameElement->FirstChildElement("first-fret");
        if (firstFretElement)
        {
            chordDiagram.firstFret = GetUnsignedIntValue(firstFretElement, chordDiagram.firstFret);
            GetRightLeftAttribute(firstFretElement, "location", chordDiagram.firstFretLocation);
        }

        // loop through all degree elements
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

                    chordDiagramNote.string = GetUnsignedIntValue("string", frameNoteElement, chordDiagramNote.string);
                    chordDiagramNote.fret = GetUnsignedIntValue("fret", frameNoteElement, chordDiagramNote.fret);
                    chordDiagramNote.fingering = GetUnsignedIntValue("fingering", frameNoteElement, chordDiagramNote.fingering);
                    chordDiagramNote.barre = GetStartStopValue("barre", frameNoteElement, chordDiagramNote.barre);
                }
            }
            else
            {
                break;
            }
            previousElement = previousElement->NextSiblingElement();
        }

        chord.chordDiagram = chordDiagram;*/
    }
}

void MusicXMLParser::ParseHarmonyElement(XMLElement* harmonyElement, float& currentTimeInMeasure, std::vector<Measure*> currentMeasures)
{
    Chord newChord;

    // root note
    XMLElement* rootElement = harmonyElement->FirstChildElement("root");
    if (rootElement)
    {
        XMLElement* rootStepElement = rootElement->FirstChildElement("root-step");
        if (rootStepElement)
        {
            newChord.rootPitch.step = XMLHelper::GetStringValue(rootStepElement, newChord.rootPitch.step);
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
            newChord.bassPitch.step = XMLHelper::GetStringValue(bassStepElement, newChord.bassPitch.step);
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
    XMLNode* previousElement = kindElement->FirstChildElement(); // first element
    while (true)
    {
        if (previousElement) {
            XMLElement* element = previousElement->ToElement();
            const char* value = element->Value();
            if (strcmp(value, "degree") == 0)
            {
                XMLElement* degreeElement = element;
                ChordDegree newDegree = ChordDegree();

                ParsePrintableElement(degreeElement, newDegree);

                // degree value element
                XMLElement* degreeValueElement = degreeElement->FirstChildElement("degree-value");
                if (degreeValueElement)
                {
                    ParseVisibleElement(degreeValueElement, newDegree.degree);
                    newDegree.degree.degree = XMLHelper::GetUnsignedIntValue(degreeValueElement, newDegree.degree.degree);
                }

                // degree type element
                XMLElement* degreeTypeElement = degreeElement->FirstChildElement("degree-type");
                if (degreeTypeElement)
                {
                    ParseVisibleElement(degreeTypeElement, newDegree.degreeType);
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
                    ParseVisibleElement(degreeAlterElement, newDegree.degreeAlter);
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

    // bass note
    //XMLElement* frameElement = harmonyElement->FirstChildElement("frame");
    //ParseFrameElement(frameElement, newChord);

    newChord.beatPosition = currentTimeInMeasure; // note: harmony elements don't increment the time
    newChord.CalculateChordName();
    //LOGE("Chord (%s) is at %f", newChord.chordName.string.c_str(), newChord.beatPosition);
    currentMeasures[newChord.staff - 1]->chords.push_back(newChord);
}

void MusicXMLParser::ParseNoteElement(XMLElement* noteElement, float& currentTimeInMeasure, std::vector<bool> staffIsTabInfo, Note* currentNote, Note* previousNote, std::vector<Measure*> currentMeasures, int measureNumber, std::string& error)
{
    currentNote->defX = XMLHelper::GetFloatAttribute(noteElement, "default-x", currentNote->defX);
    currentNote->defY = XMLHelper::GetFloatAttribute(noteElement, "default-y", currentNote->defY);

    currentNote->relX = XMLHelper::GetFloatAttribute(noteElement, "relative-x", currentNote->relX);
    currentNote->relY = XMLHelper::GetFloatAttribute(noteElement, "relative-y", currentNote->relY);

    // staff
    XMLElement* staffElement = noteElement->FirstChildElement("staff");
    if (staffElement)
    {
        currentNote->staff = ToInt(staffElement->GetText());
    }
    else
    {
        currentNote->staff = 1;
    }

    if (staffIsTabInfo[currentNote->staff-1]) {
        currentNote->type = Note::NoteType::Tab;
    }

    // chord
    XMLElement* chordElement = noteElement->FirstChildElement("chord");
    if (chordElement)
    {
        currentNote->isChord = true;
    }

    // rest
    XMLElement* restElement = noteElement->FirstChildElement("rest");
    if (restElement)
    {
        currentNote->isRest = true;
        currentNote->isFullMeasureRest = XMLHelper::GetBoolAttribute(restElement, "measure", false);
    }

    // pitch
    XMLElement* pitch = noteElement->FirstChildElement("pitch");
    if (pitch)
    {
        // step
        XMLElement* step = pitch->FirstChildElement("step");
        if (step)
        {
            currentNote->pitch.step = step->GetText();
        }
        else
        {
            error = "no step element in pitch element";
        }

        // octave
        XMLElement* octave = pitch->FirstChildElement("octave");
        if (step)
        {
            currentNote->pitch.octave = ToInt(octave->GetText());
        }
        else
        {
            error = "no octave element in pitch element";
        }

        // alter
        XMLElement* alter = pitch->FirstChildElement("alter");
        if (alter)
        {
            currentNote->pitch.alter = float(ToInt(alter->GetText()));
        }
    }

    // duration
    XMLElement* duration = noteElement->FirstChildElement("duration");
    if (duration)
    {
        int divisions = currentMeasures[currentNote->staff-1]->divisions;
        if (divisions != 0)
        {
            currentNote->duration.duration = (1.0f / (float)divisions) * (float)ToInt(duration->GetText());
        }
        else
        {
            error = "Error divisions is zero";
        }

        // calculating beat position of note
        if (currentNote->isChord) { // is a chord so beat position is the same as the previous note's beat position
            if (previousNote)
                currentNote->beatPosition = currentTimeInMeasure - previousNote->duration.duration;
            else
                currentNote->beatPosition = 0.0f;
        } else { // not a chord so beat position is equal to the current time in the measure
            currentNote->beatPosition = currentTimeInMeasure;
        }

        // increment time in measure
        if (!currentNote->isChord)
        {
            currentTimeInMeasure += currentNote->duration.duration;
        }
    }
    /*else
    {
        error = "note has no duration";
    }*/

    // tie
    XMLElement* tieElement = noteElement->FirstChildElement("tie");
    if (tieElement)
    {
        std::string typeString = XMLHelper::GetStringAttribute(tieElement, "type", "", true);

        if (typeString == "start")
            currentNote->tie.type = NoteTie::TieType::Start;
        else if (typeString == "stop")
            currentNote->tie.type = NoteTie::TieType::Stop;
        else
            currentNote->tie.type = NoteTie::TieType::None;
    }

    // voice
    XMLElement* voiceElement = noteElement->FirstChildElement("voice");
    if (voiceElement)
    {
        currentNote->voice = ToInt(voiceElement->GetText());
    }

    // type
    XMLElement* noteType = noteElement->FirstChildElement("type");
    if (noteType)
    {
        currentNote->CalculateDurationTypeFromString(noteType->GetText());
    }

    // lyrics
    XMLNode* previousLyricElement = noteElement->FirstChildElement("lyric");
    while (true)
    {
        if (previousLyricElement) {
            XMLElement* lyricElement = previousLyricElement->ToElement();
            Lyric lyric = ParseLyric(lyricElement);
            currentNote->lyrics.push_back(lyric);
        }
        else
        {
            break;
        }
        previousLyricElement = previousLyricElement->NextSiblingElement("lyric");
    }

    // accidental
    XMLElement* accidentalElement = noteElement->FirstChildElement("accidental");
    if (accidentalElement)
    {
        currentNote->accidental.accidentalType = Accidental::CalculateAccidentalTypeFromString(accidentalElement->GetText());

        currentNote->accidental.isCautionary = XMLHelper::GetBoolAttribute(accidentalElement, "cautionary", false);
        currentNote->accidental.isEditorial = XMLHelper::GetBoolAttribute(accidentalElement, "editorial", false);
        currentNote->accidental.hasBrackets = XMLHelper::GetBoolAttribute(accidentalElement, "bracket", false);
        currentNote->accidental.hasParentheses = XMLHelper::GetBoolAttribute(accidentalElement, "parentheses", false);
    }

    // stem
    XMLElement* stemElement = noteElement->FirstChildElement("stem");
    if (stemElement)
    {
        currentNote->noteStem.stemType = currentNote->noteStem.CalculateStemTypeFromString(stemElement->GetText());
    }

    // beam
    XMLNode* previousBeamElement = noteElement->FirstChildElement("beam");
    while (true)
    {
        if (previousBeamElement) {
            XMLElement* beamElement = previousBeamElement->ToElement();
            if (beamElement)
            {
                NoteBeamData noteBeamData = NoteBeamData();

                noteBeamData.beamType = NoteBeamData::CalculateBeamTypeFromString(beamElement->GetText());
                noteBeamData.beamLevel = XMLHelper::GetNumberAttribute(beamElement, "number", 1);

                currentNote->beamData.push_back(noteBeamData);
            }
        }
        else
        {
            break;
        }

        previousBeamElement = previousBeamElement->NextSiblingElement("beam");
    }

    // notations
    XMLElement* notations = noteElement->FirstChildElement("notations");
    if (notations)
    {
        // slurs
        XMLElement* slurElement = notations->FirstChildElement("slur");
        if (slurElement)
        {
            Slur slur = Slur();
            slur.id = XMLHelper::GetNumberAttribute(slurElement, "number");
            slur.placement = MusicXMLHelper::GetAboveBelowAttribute(slurElement, "placement");
            slur.type = MusicXMLHelper::GetStartStopAttribute(slurElement, "type");
            currentNote->slurs.push_back(slur);
        }

        // technical
        XMLElement* technical = notations->FirstChildElement("technical");
        ParseTechnicalElement(technical, currentNote, staffIsTabInfo[currentNote->staff-1]);
    }

    // dot
    XMLElement* dotElement = noteElement->FirstChildElement("dot"); // TODO: modify to allow the parsing of multiple dots
    if (dotElement)
    {
        AugmentationDot dot = AugmentationDot();

        ParseVisibleElement(dotElement, dot);

        dot.placement = MusicXMLHelper::GetAboveBelowAttribute(dotElement, "placement", dot.placement);

        currentNote->dots.push_back(dot);
    }

    currentNote->measureIndex = measureNumber - 1;
    currentMeasures[currentNote->staff - 1]->notes.push_back(currentNote);
}

void MusicXMLParser::ParseTechnicalElement(XMLElement* technicalElement, Note* currentNote, bool isTab)
{
    if (technicalElement)
    {
        // hammer ons
        XMLNode* previousHammerOnElement = technicalElement->FirstChildElement("hammer-on");
        while (true)
        {
            if (previousHammerOnElement) {
                XMLElement* hammerOnElement = previousHammerOnElement->ToElement();
                if (hammerOnElement)
                {
                    TABSlur tabSlur = TABSlur();
                    tabSlur.slurType = TABSlur::SlurType::HammerOn;
                    tabSlur.id = XMLHelper::GetNumberAttribute(hammerOnElement, "number");
                    tabSlur.placement = MusicXMLHelper::GetAboveBelowAttribute(hammerOnElement, "placement");
                    tabSlur.type = MusicXMLHelper::GetStartStopAttribute(hammerOnElement, "type");
                    if (hammerOnElement->GetText()) {
                        tabSlur.text = hammerOnElement->GetText();
                    }
                    currentNote->tabSlurs.push_back(tabSlur);
                }
            }
            else
            {
                break;
            }
            previousHammerOnElement = previousHammerOnElement->NextSiblingElement("hammer-on");
        }

        // pull offs
        XMLNode* previousPullOffElement = technicalElement->FirstChildElement("pull-off");
        while (true)
        {
            if (previousPullOffElement) {
                XMLElement* pullOffElement = previousPullOffElement->ToElement();
                if (pullOffElement)
                {
                    TABSlur tabSlur = TABSlur();
                    tabSlur.slurType = TABSlur::SlurType::PullOff;
                    tabSlur.id = XMLHelper::GetNumberAttribute(pullOffElement, "number");
                    tabSlur.placement = MusicXMLHelper::GetAboveBelowAttribute(pullOffElement, "placement");
                    tabSlur.type = MusicXMLHelper::GetStartStopAttribute(pullOffElement, "type");
                    if (pullOffElement->GetText()) {
                        tabSlur.text = pullOffElement->GetText();
                    }
                    currentNote->tabSlurs.push_back(tabSlur);
                }
            }
            else
            {
                break;
            }
            previousPullOffElement = previousPullOffElement->NextSiblingElement("pull_off");
        }

        // - TAB only -
        if (currentNote->type == Note::NoteType::Tab && isTab) {

            // string
            XMLElement* string = technicalElement->FirstChildElement("string");
            if (string)
            {
                currentNote->string = ToInt(string->GetText());
            }

            // fret
            XMLElement* fret = technicalElement->FirstChildElement("fret");
            if (fret)
            {
                currentNote->fret = ToInt(fret->GetText());
            }
        }
    }
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

// main file parsing
Song* MusicXMLParser::ParseMusicXML(const std::string& data, std::string& error)
{
    m_Errors.clear();
    LOGI("STARTING TO PARSE");
    Song* song = new Song();
    MusicDisplayConstants displayConstants;
    LOGI("Created song data");

    tinyxml2::XMLDocument doc;
    const char *d = data.c_str();
    tinyxml2::XMLError xmlError = doc.Parse(d);
    LOGD("parced document with tinyxml2");
    if (xmlError != tinyxml2::XMLError::XML_SUCCESS) {
        LOGE("error: %i", xmlError);
        error = "XMLERROR: " + ToString(int(xmlError));
        AddError("XML Error", ToString(int(xmlError)));
    }

    if (doc.Error())
    {
        LOGE("doc error: %s: %s", doc.ErrorName(), doc.ErrorStr());
        error = "XML DOC ERROR";
        AddError(doc.ErrorName(), doc.ErrorStr());
        LOGD("---------------------------Document:\n\n%s\n\n", data.c_str());
        //error = doc.ErrorDesc();
    }
    else
    {
        // will create problems if it is socre-timewise
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
                        Instrument* instrument = new Instrument();
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
                error = "ERROR no part-list";
                AddError("Parse Failed", "No part-list");
            }


            // parts
            XMLNode* previous = root->FirstChildElement("part");
            while (true) // looping through all parts
            {
                if (previous)
                {
                    XMLElement* part = previous->ToElement();
                    std::string id = part->Attribute("id");
                    Instrument* currentInst = song->GetInstrument(id);
                    if (currentInst == nullptr) {
                        error = "Instrument " + id + " does not exist";
                        AddError("Error", "Instrument " + id + " does not exist");
                        return song;
                    }

                    // measures

                    int measureIndex = 0;

                    bool firstMeasure = true;
                    std::vector<bool> staffIsTabInfo;
                    bool stavesCreated = false;

//                    bool multiMeasureRest = false; // whether the measure is part of a multi measure rest
//                    unsigned int numberOfMeasuresInMultiMeasureRest = 0; // number of measures left in multi measure rest
//                    unsigned int measureThatStartedMultiMeasureRest = 0; // the index of the measure that started the multi measure res


                    XMLNode* previousMeasureElement = part->FirstChildElement("measure");
                    std::vector<Measure*> previousMeasures;

                    while (true)
                    {
                        if (previousMeasureElement)
                        {
                            XMLElement* measure = previousMeasureElement->ToElement();

                            int measureNumber = XMLHelper::GetNumberAttribute(measure, "number", 0);
                            float measureWidth = XMLHelper::GetFloatAttribute(measure, "width", 1.0f);
                            //LOGW("number: %d, measureWidth: %f", measureNumber, measureWidth);

                            bool startNewSystem = false;
                            bool startNewPage = false;

                            std::vector<Measure*> currentMeasures;

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
                                        currentInst->staves.push_back(new Staff());

                                        staffIsTabInfo.push_back(false);
                                    }

                                    stavesCreated = true;
                                }
                            }

                            // creating measures for each staff
                            for (int i = 0; i < currentInst->staves.size(); i++)
                            {
                                Measure* newMeasure = new Measure();
                                newMeasure->number = measureNumber;
                                newMeasure->index = measureNumber - 1;
                                newMeasure->staff = i+1;
                                newMeasure->defaultMeasureWidth = measureWidth;

                                if (stavesCreated)
                                {
                                    if (staffIsTabInfo[i])
                                        newMeasure->type = Measure::MeasureType::Tab;
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
                                for (auto m : currentMeasures) { m->startNewSystem = startNewSystem; m->startNewPage = startNewPage; }

                                if (firstMeasure || startNewSystem)
                                {
                                    System system = System();
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

                                    system.layout = systemLayout;

                                    if (firstMeasure)
                                        system.showTimeSignature = true;

                                    system.showClef = true;
                                    system.showKeySignature = true;

                                    song->systems.push_back(system);
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
                                if (divisions) {
                                    for (auto m : currentMeasures) { m->divisions = ToInt(divisions->GetText()); } // setting divisions for all current measures
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

                                    for (auto m : currentMeasures) { m->keySignature = keySignature; m->showKeySignature = true; }
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

                                    for (auto m : currentMeasures) { m->timeSignature = timeSignature; m->showTimeSignature = true; m->CalculateDuration(); }
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
                                            int clefNumber = XMLHelper::GetNumberAttribute(clefElement, "number", 1);

                                            XMLElement* signElement = clefElement->FirstChildElement("sign");
                                            if (signElement)
                                            {
                                                currentMeasures[clefNumber-1]->clef.sign = signElement->GetText();
                                                if (currentMeasures[clefNumber-1]->clef.sign == "TAB")
                                                {
                                                    if (firstMeasure) {
                                                        currentInst->staves[clefNumber-1]->type = Staff::StaffType::Tab;
                                                        staffIsTabInfo[clefNumber-1] = true;
                                                        currentMeasures[clefNumber-1]->type = Measure::MeasureType::Tab;

                                                        if (currentInst->staves.size() > 1) // if there is more than one staff
                                                            currentInst->staves[clefNumber-1]->tablatureDisplayType = Staff::TablatureDisplayType::NoRhythm;
                                                    }
                                                }
                                            }

                                            XMLElement* line = clefElement->FirstChildElement("line");
                                            if (line)
                                                currentMeasures[clefNumber-1]->clef.line = ToInt(line->GetText());

                                            currentMeasures[clefNumber-1]->showClef = true;
                                        }
                                        else // no more clefs
                                        {
                                            break;
                                        }
                                        previousClefElement = previousClefElement->NextSiblingElement("clef");
                                    }
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

                                    for (auto m : currentMeasures) { m->transpose = transpose; }
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

                                        Staff* tabStaff = currentInst->staves.back();

                                        while (true) {
                                            if (previousStaffTuning) {
                                                XMLElement *staffTuning = previousStaffTuning->ToElement();
                                                XMLElement *tuningStep = staffTuning->FirstChildElement(
                                                        "tuning-step");
                                                StaffTuning tuning;
                                                if (tuningStep)
                                                    tuning.pitch.step = tuningStep->GetText();
                                                XMLElement *tuningOctave = staffTuning->FirstChildElement(
                                                        "tuning-octave");
                                                if (tuningOctave)
                                                    tuning.pitch.octave = ToInt(
                                                            tuningOctave->GetText());
                                                XMLElement *tuningAlter = staffTuning->FirstChildElement(
                                                        "tuning-alter");
                                                if (tuningAlter)
                                                    tuning.pitch.alter = ToInt(
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

                                    // measure style
                                    XMLElement* multipleRestElement = measureStyleElement->FirstChildElement("multiple-rest");
                                    if (multipleRestElement)
                                    {
                                        unsigned int numberOfMeasures = XMLHelper::GetUnsignedIntValue(multipleRestElement, 1);
                                        bool useSymbols = XMLHelper::GetBoolAttribute(multipleRestElement, "use-symbols", false);

                                        if (staffNumber == -1) // applies to all the staves
                                        {
                                            for (auto m : currentMeasures)
                                            {
                                                m->startsMultiMeasureRest = true;
                                                m->isPartOfMultiMeasureRest = true;
                                                m->useSymbolsForMultiMeasureRest = useSymbols;
                                                m->numberOfMeasuresInMultiMeasureRest = numberOfMeasures;
                                            }
                                        }
                                        else // applies only to the specified staff
                                        {
                                            currentMeasures[staffNumber-1]->startsMultiMeasureRest = true;
                                            currentMeasures[staffNumber-1]->isPartOfMultiMeasureRest = true;
                                            currentMeasures[staffNumber-1]->useSymbolsForMultiMeasureRest = useSymbols;
                                            currentMeasures[staffNumber-1]->numberOfMeasuresInMultiMeasureRest - numberOfMeasures;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (firstMeasure)
                                    currentInst->staves.push_back(new Staff());

                                if (!previousMeasures.empty() && previousMeasures.size() == currentMeasures.size())
                                {
                                    int i = 0;
                                    for (auto m : currentMeasures)
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
                            Note* previousNote = nullptr;
                            while (true)
                            {
                                if (previousElement)
                                {
                                    XMLElement* element = previousElement->ToElement();
                                    const char* value = element->Value();
                                    if (strcmp(value, "note") == 0) // note
                                    {
                                        Note* currentNote = new Note();
                                        ParseNoteElement(element, currentTimeInMeasure, staffIsTabInfo, currentNote, previousNote, currentMeasures, measureNumber, error);
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
                                                error = "Error divisions is zero";
                                                AddError("Invalid value", "Divisions is zero");
                                            }

                                            currentTimeInMeasure -= duration;
                                        }
                                        else
                                        {
                                            error = "backup element has no duration";
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
                                                error = "Error divisions is zero";
                                                AddError("Invalid value", "Divisions is zero");
                                            }

                                            currentTimeInMeasure += duration;
                                        }
                                        else
                                        {
                                            error = "backup element has no duration";
                                            AddError("Missing Element", "Forward element has no duration");
                                        }
                                    }
                                    else if (strcmp(value, "direction") == 0) // direction
                                    {
                                        // direction
                                        bool isNewDirection;
                                        Direction direction = ParseDirection(element, isNewDirection, currentTimeInMeasure);
                                        if (isNewDirection)
                                        {
                                            direction.beatPosition = currentTimeInMeasure;
                                            currentMeasures[0]->directions.push_back(direction);
                                        }

                                        // sound
                                        XMLElement* soundElement = element->FirstChildElement("sound");
                                        if (soundElement)
                                        {
                                            SoundEvent event = SoundEvent();

                                            event.dynamics = XMLHelper::GetFloatAttribute(soundElement, "dynamics", -1.0f);
                                            event.tempo = XMLHelper::GetFloatAttribute(soundElement, "tempo", -1.0f);
                                            event.pan = XMLHelper::GetNumberAttribute(soundElement, "pan", event.pan);

                                            // getting beat position of sound event
                                            event.beatPosition = currentTimeInMeasure;

                                            currentMeasures[0]->soundEvents.push_back(event);
                                            for (auto m : currentMeasures) { m->soundEvents.push_back(event); }
                                        }
                                    }
                                    else if (strcmp(value, "barline") == 0) // barline
                                    {
                                        Barline barline = ParseBarlineElement(element);

                                        for (auto m : currentMeasures) { m->barlines.push_back(barline); }
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
                            for (Measure* m : currentMeasures)
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
                else
                {
                    break;
                }

                previous = previous->NextSiblingElement("part");
            }
        }
        else
        {
            error = "ERROR no root";
            AddError("Parse Error", "Failed to parse root element");
        }
    }

    song->displayConstants = displayConstants;

    LOGD("Finished");
    doc.Clear();

    currentDynamicWedges.clear();
    currentDashes.clear();
    currentBrackets.clear();

    ParseError::PrintErrors();

    return song;
}