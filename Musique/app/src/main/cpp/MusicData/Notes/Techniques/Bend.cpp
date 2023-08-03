#include "Bend.h"

#include "../../../RenderMeasurement.h"

void Bend::Render(RenderData& renderData, Vec2<float> notePosition) const
{
    Paint paint = Paint(color.color);

    // arrow
    renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { arrowPosition.x + position.x + notePosition.x - (RenderMeasurement::MeasureGlyph(GetSMuFLID()) / 2.0f), arrowPosition.y + position.y + notePosition.y }, paint));

    // tail
    paint.strokeWidth = 1.8f;
    float offsetY = 3.0f;

    if (bendType == BendType::PreBend)
    {
        renderData.AddLine(Line({ tailPosition.x + position.x + notePosition.x, tailPosition.y + position.y + notePosition.y - offsetY }, { arrowPosition.x + position.x + notePosition.x, arrowPosition.y + position.y + notePosition.y - offsetY }, paint));
    }
    else
    {
        CubicCurve curve = CubicCurve();

        Vec2<float> curveStart = tailPosition + position + notePosition;
        Vec2<float> curveEnd = arrowPosition + position + notePosition - Vec2<float>{ 0.0f, offsetY };

        // start
        curve.point1 = curveStart;

        // curve points
        curve.point2.x = curveStart.x + (arrowPosition.x - tailPosition.x);
        curve.point2.y = curveStart.y;

        curve.point3.x = curve.point2.x;
        curve.point3.y = curve.point2.y - 10.0f;

        // end
        curve.point4.x = curveEnd.x;
        curve.point4.y = curveEnd.y;

        curve.paint = paint;

        renderData.AddCubicCurve(curve);
    }

    // text
    text.Render(renderData, arrowPosition + position + notePosition, paint);

    if (bendType == BendType::Release)
    {
        // arrow
        paint.rotateDegrees = 180.0f;
        renderData.AddGlyph(SMuFLGlyph(GetSMuFLID(), { arrowReleasePosition.x + position.x + notePosition.x + (RenderMeasurement::MeasureGlyph(GetSMuFLID()) / 2.0f),
                                       arrowReleasePosition.y + position.y + notePosition.y }, paint));
        paint.rotateDegrees = 0.0f;

        // tail
        CubicCurve curve = CubicCurve();

        Vec2<float> curveStart = arrowPosition + position + notePosition - Vec2<float>{ 0.0f, RenderMeasurement::GetGlyphBoundingBox(SMuFLGlyph(GetSMuFLID(), { 0.0f, 0.0f }, Paint())).size.y };
        Vec2<float> curveEnd = arrowReleasePosition + position + notePosition + Vec2<float>{ 0.0f, offsetY };

        // start
        curve.point1.x = curveStart.x;
        curve.point1.y = curveStart.y;

        // curve points
        curve.point2.x = curveStart.x + (arrowReleasePosition.x - arrowPosition.x);
        curve.point2.y = curveStart.y;

        curve.point3.x = curve.point2.x;
        curve.point3.y = curve.point2.y + 10.0f;

        // end
        curve.point4.x = curveEnd.x;
        curve.point4.y = curveEnd.y;

        curve.paint = paint;

        renderData.AddCubicCurve(curve);
    }
}

Vec2<float> Bend::GetDimensions() const
{
    SMuFLID glyphID = GetSMuFLID();
    return { RenderMeasurement::MeasureGlyph(glyphID), 0.0f };
}

SMuFLID Bend::GetSMuFLID() const
{
    return SMuFLID::arrowheadBlackUp;
}

void Bend::CalculatePositionAsPaged(const MusicDisplayConstants& displayConstants, float topStaffLineDistNote, bool isTab, const NoteStem& noteStem, float topNotePositionY, float bottomNotePositionY)
{
    text.fontSize.size = 8.0f;
    text.position = { 0.0f, -20.0f }; // relative to the arrow position
    text.align = TextualElement::TextAlign::Center;
    text.justify = Justify::Center;

    if (alterSemitones == 0.5f)
        text.text = "1/4";
    else if (alterSemitones == 1.0f)
        text.text = "1/2";
    else if (alterSemitones == 1.5f)
        text.text = "3/4";
    else if (alterSemitones == 2.0f)
        text.text = "full";
    else if (alterSemitones == 2.5f)
        text.text = "1 1/4";
    else if (alterSemitones == 3.0f)
        text.text = "1 1/2";
    else if (alterSemitones == 3.5f)
        text.text = "1 3/4";
    else if (alterSemitones == 4.0f)
        text.text = "2";
    else if (alterSemitones == 4.5f)
        text.text = "2 1/4";
    else if (alterSemitones == 5.0f)
        text.text = "2 1/2";
    else if (alterSemitones == 5.5f)
        text.text = "2 3/4";
    else if (alterSemitones == 6.0f)
        text.text = "3";

    if (isTab)
    {
        position = { 0.0f, 0.0f };

        arrowPosition.y -= topStaffLineDistNote;
        arrowPosition.y -= 10.0f;

        arrowPosition.x += 10.0f;

        tailPosition = {5.0f, -5.0f };

        if (bendType == BendType::PreBend)
        {
            tailPosition.x = arrowPosition.x;
        }

        if (bendType == BendType::Release)
        {
            arrowReleasePosition.x = 25.0f;
            arrowReleasePosition.y = -topStaffLineDistNote;
            arrowReleasePosition.y += 7.0f;
        }
    }
    else // is standard
    {
        /*if (placement == AboveBelowType::Above)
        {
            position.y -= 10.0f;
        }
        else if (placement == AboveBelowType::Below)
        {
            position.y += 10.0f;
        }*/
    }
}

void Bend::OnPlay(std::shared_ptr<Player> player, int channel)
{

}

void Bend::OnStop(std::shared_ptr<Player> player, int channel)
{
    player->PitchBendNote(PlayableNote(), 0.0f, channel);
}

void Bend::OnUpdate(std::shared_ptr<Player> player, float beatPositionRelativeToNote, float noteDuration, int channel)
{
    float semitonesAtEnd = alterSemitones;
    float percentage = beatPositionRelativeToNote / (noteDuration * firstBeat);
    float alterSemi = percentage * semitonesAtEnd;

    if (alterSemi > semitonesAtEnd)
        alterSemi = semitonesAtEnd;

    player->PitchBendNote(PlayableNote(), alterSemi, channel);
}