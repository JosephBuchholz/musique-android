#include "NoteStem.h"

void NoteStem::Render(RenderData& renderData, Vec2<float> notePosition, std::shared_ptr<TremoloSingle> tremoloSingle, bool isGraceNote, bool hasSlash, float noteHeadWidth, Vec2<float> offset) const
{
    // note stem
    if (stemType != NoteStem::StemType::None)
    {
        Paint linePaint = renderData.paints.noteStemPaint;
        VisibleElement::ModifyPaint(linePaint);

        float lineX = notePosition.x + stemPositionX + offset.x;
        renderData.AddLine(std::make_shared<Line>(lineX, notePosition.y + stemStartY, lineX, notePosition.y + stemEndY, linePaint));

        if (tremoloSingle)
        {
            float mid = ((notePosition.y + stemEndY) - (notePosition.y + stemStartY)) / 2.0f;
            tremoloSingle->Render(renderData, { lineX, notePosition.y + stemStartY + mid });
        }
    }

    // render grace note slash
    if (isGraceNote && hasSlash)
    {
        Vec2<float> positionStart = { stemPositionX - (noteHeadWidth / 2.0f), (stemEndY - stemStartY) / 3.0f };
        Vec2<float> positionEnd = { positionStart.x + noteHeadWidth + 2.0f, positionStart.y + ((stemEndY - stemStartY) / 4.0f) };
        renderData.AddLine(std::make_shared<Line>(notePosition.x + positionStart.x, notePosition.y + positionStart.y + stemStartY, notePosition.x + positionEnd.x, notePosition.y + positionEnd.y + stemStartY, renderData.paints.noteStemPaint));
    }
}

void NoteStem::UpdateBoundingBox(const MusicDisplayConstants& displayConstants, Vec2<float> parentPosition)
{
    boundingBox.position.x = stemPositionX + position.x + parentPosition.x;
    boundingBox.position.y = stemStartY + position.y + parentPosition.y;
    boundingBox.size.x = displayConstants.stemLineWidth;
    boundingBox.size.y = stemEndY - stemStartY;

    boundingBox.MakeDimensionsPositive(); // since the height may be negative

    boundingBox.constraints.emplace_back(Constraint::ConstraintType::Static);

#if DEBUG_BOUNDING_BOXES
    debugBoundingBox = boundingBox;
#endif
}