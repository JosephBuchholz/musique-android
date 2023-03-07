#ifndef MUSIQUE_VIEWMODELDATA_H
#define MUSIQUE_VIEWMODELDATA_H

struct ViewModelData {

public:
    bool playing = false;
    float playLineBeatPosition = 0.0f;
    int currentMeasure = 0;
};

#endif // MUSIQUE_VIEWMODELDATA_H