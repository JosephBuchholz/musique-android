#include "Words.h"

#include "../../RenderMeasurement.h"

void Words::Render(RenderData& renderData, Vec2<float> measurePosition) const
{
    Paint paint = Paint();
    paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    renderData.AddText(Text(text.string, position.x + measurePosition.x, position.y + measurePosition.y, Paint(color.color, paint)));
}

void Words::RenderDebug(RenderData& renderData, Vec2<float> measurePosition) const
{
    BoundingBox bb = GetBoundingBoxRelativeToParent();

    bb.position += measurePosition;

    bb.AddPadding(3.0f);

    bb.Render(renderData, 0xFFFF0000);
}

Vec2<float> Words::GetDimensions() const
{
    Vec2<float> dimensions;
    dimensions.x = fontSize.size * (float)text.string.size();
    dimensions.y = fontSize.size * 2.0f;
    return dimensions;
}

BoundingBox Words::GetBoundingBoxRelativeToParent() const
{
    Paint paint = Paint();
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size;

    BoundingBox bb = RenderMeasurement::GetTextBoundingBox(Text(text.string, 0.0f, 0.0f, paint));

    bb.position += position;

    bb.position.x -= bb.size.x / 2.0f;
    bb.position.y += bb.size.y / 2.0f;

    return bb;
}

void Words::UpdateBoundingBox(const Vec2<float>& parentPosition)
{
    /*Paint paint = Paint();
    //paint.textSize = 16.0f;
    if (fontStyle == FontStyle::Italic)
        paint.isItalic = true;
    if (fontWeight == FontWeight::Bold)
        paint.isBold = true;
    paint.textSize = fontSize.size; // ERROROROROROR

    //LOGV("paint.textSize: %f!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1", paint.textSize);

#define TOGGLE 0

#if TOGGLE
    BoundingBox bb = RenderMeasurement::GetTextBoundingBox(Text(text.string, positionX, positionY, paint));
#else
    BoundingBox bb = BoundingBox();
    bb.position.x = 0.0f;
    bb.position.y = -paint.textSize * 2.0f;
    //bb.position.y = 0.0f;
    bb.size.x = paint.textSize * (float)text.string.size();
    bb.size.y = paint.textSize * 2.0f;
#endif

    boundingBox.position.x = bb.position.x + position.x + parentPosition.x;
    boundingBox.position.y = bb.position.y + position.y + parentPosition.y;
    boundingBox.position.x -= bb.size.x / 2.0f;
    boundingBox.position.y += bb.size.y / 2.0f;
    boundingBox.size.x = bb.size.x;
    boundingBox.size.y = bb.size.y;*/

    boundingBox = GetBoundingBoxRelativeToParent();
    boundingBox.position += parentPosition;

    boundingBox.AddPadding(3.0f);

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::None);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif

    /*LOGV("Updated bounding box for WORDS: %s", boundingBox.GetPrintableString().c_str());
    LOGV("parentPosition: %s", parentPosition.GetPrintableString().c_str());
    LOGD("boundingBoxPosition: %s", bb.GetPrintableString().c_str());*/
}

void Words::Move(Vec2<float> positionOffset, Vec2<float> sizeOffset, float rotationOffset)
{
    position.x += positionOffset.x;
    position.y += positionOffset.y;
}

void Words::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float defaultX, float defaultY)
{
    /*if (noDefX)
        positionX = defaultX + relX;
    else
        positionX = defX + relX;*/

    //LOGE("calculate position: x: %f, y: %f", defaultX, defaultY);

    if (noDefY)
        position.y = defaultY + -relY;
    else
        position.y = -defY + -relY;

    position.x = defaultX;
    //positionY = defaultY;
}

std::shared_ptr<SoundEvent> Words::GetSoundEvent() const
{
    if (text.string == "Swing" || text.string == "Straight" ||
        text.string == "Swing 16ths")
    {
        std::shared_ptr<SwingSoundEvent> soundEvent = std::make_shared<SwingSoundEvent>();

        if (text.string == "Swing")
            soundEvent->swing.swingType = SwingTempo::SwingType::Eighth;
        else if (text.string == "Swing 16ths")
            soundEvent->swing.swingType = SwingTempo::SwingType::Sixteenth;
        else if (text.string == "Straight")
            soundEvent->swing.swingType = SwingTempo::SwingType::Straight;

        return soundEvent;
    }
    if (text.string.find("Capo") != std::string::npos || text.string.find("capo") != std::string::npos)
    {
        std::shared_ptr<CapoSoundEvent> soundEvent = std::make_shared<CapoSoundEvent>();

        // extract capo fret number
        size_t found = text.string.find_last_not_of("0123456789");
        std::string numberString;
        if (found != std::string::npos)
            numberString = text.string.substr(found + 1);

        soundEvent->capo = ToInt(numberString);

        LOGV_TAG("Words", "soundEvent->capo: %d", soundEvent->capo);
        return soundEvent;
    }

    return nullptr;
}

void Words::OnTranspose(const TranspositionRequest& transposeRequest)
{
    if (transposeRequest.transposeTablatureType == TranspositionRequest::TransposeTablatureType::UseCapo)
    {
        if (text.string.find("Capo") != std::string::npos || text.string.find("capo") != std::string::npos)
        {
            std::shared_ptr<CapoSoundEvent> soundEvent = std::make_shared<CapoSoundEvent>();

            // extract capo fret number
            size_t found = text.string.find_last_not_of("0123456789");
            std::string numberString;
            if (found != std::string::npos)
                numberString = text.string.substr(found + 1);

            int capo = ToInt(numberString);

            capo += transposeRequest.GetInterval();

            text.string = "Capo " + ToString(capo);
        }
    }
}