#ifndef MUSIQUE_FRAMEDATA_H
#define MUSIQUE_FRAMEDATA_H

class FrameData {

public:

    float playLinePosition = 0.0f;
    float playLinePositionY = 0.0f;
    float playLineHeight = 0.0f;
    float playProgress = 0.0f;

    bool isPlaying = false;
};

#endif // MUSIQUE_FRAMEDATA_H