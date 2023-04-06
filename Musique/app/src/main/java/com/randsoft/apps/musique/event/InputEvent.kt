package com.randsoft.apps.musique.event

class InputEvent {

    enum class InputEventType {
        INPUT_EVENT_NONE,
        INPUT_EVENT_TAP
    }

    var type: InputEventType = InputEventType.INPUT_EVENT_NONE

    var x = 0.0f
    var y = 0.0f
}