/**
 * This files contains the definition for the 'MusicRenderer' class.
 */

#ifndef MUSIQUE_MUSICRENDERER_H
#define MUSIQUE_MUSICRENDERER_H

#include "MusicData/Song.h"
#include "RenderData/RenderData.h"

/**
 * This class handles all the rendering calculations.
 */
class MusicRenderer
{
public:

    MusicRenderer();

    void RenderMusicToPage(std::shared_ptr<Song> song, int page, RenderData& pageRenderData, float pageX, float pageY);

    void Render(std::shared_ptr<Song> song, Settings settings);
    void CalculateRenderForPagedLayout(std::shared_ptr<Song> song, Settings settings);
    void RenderWithRenderData();

    int OnCalculateNumPages(std::shared_ptr<Song> song);

private:

    Vec2<float> RenderSystem(RenderData& renderData, std::shared_ptr<Song> song, unsigned int startMeasure, unsigned int endMeasure, int systemIndex, Vec2<float> systemPosition, Vec2<float> pagePosition, bool drawFullInstNames);

    void RenderLineOfMeasures(RenderData& renderData, std::shared_ptr<Song> song, unsigned int startMeasure, unsigned int endMeasure, const System& system, Staff* staff, float systemPositionX, float staffPositionY, float lineSpacing, bool isTopMeasureLine);

    void RenderBarlines(RenderData& renderData, const std::vector<Barline>& barlines, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing);
    void RenderBarline(RenderData& renderData, const Barline& barline, float barlinePositionX, float barlinePositionY, float barlineHeight, int lineCount, float lineSpacing);

    void RenderMultiMeasureRest(RenderData& renderData, unsigned int measureRestCount, float measurePositionX, float measurePositionY, float measureWidth, int lineCount, float lineSpacing);

    void RenderCredits(RenderData& renderData, std::shared_ptr<Song> song, const MusicDisplayConstants& displayConstants, const std::vector<Credit>& credits, float pageX, float pageY);

public:

    Paint LinePaint;
    Paint NoteStemPaint;
    Paint NoteBeamPaint;
    Paint BarLinePaint;
    Paint HeavyBarLinePaint;

    Paint TabSlurPaint;
    Paint TiePaint;

    Paint TextPaint;
    Paint TabTextPaint;
    Paint InstNameTextPaint;

    int normalColor = 0xff000000;
    int highlightedColor = 0xff1188ee;
    int playedColor = 0xff1188ee;

    bool layoutCalculated = false;

    RenderData m_RenderData = RenderData();

    std::vector<Vec2<float>> pagePositions;
    std::vector<Vec2<float>> systemPositions;

    bool updateRenderData = true; // weather the sheet music needs to be updated(rendered) again
};

#endif //MUSIQUE_MUSICRENDERER_H