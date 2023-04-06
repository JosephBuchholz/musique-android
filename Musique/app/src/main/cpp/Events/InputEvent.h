#ifndef MUSIQUE_INPUTEVENT_H
#define MUSIQUE_INPUTEVENT_H

#include "../Collisions/Vec2.h"

class InputEvent
{
public:
    enum class InputEventType {
        None, Tap
    } type = InputEventType::None;

    Vec2<float> position = { 0.0f, 0.0f };
};

#endif //MUSIQUE_INPUTEVENT_H
