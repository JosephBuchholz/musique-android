#pragma once
#include <string>

struct NoteBeamData {

    NoteBeamData() {}

    enum class BeamType {
        None = 0, Begin, End, Continue, ForwardHook, BackwardHook
    };

    static BeamType CalculateBeamTypeFromString(const std::string& string) {
        if (string == "begin") {
            return BeamType::Begin;
        } else if (string == "end") {
            return BeamType::End;
        } else if (string == "continue") {
            return BeamType::Continue;
        } else if (string == "forward hook") {
            return BeamType::ForwardHook;
        } else if (string == "backward hook") {
            return BeamType::BackwardHook;
        }
        return BeamType::None;
    }

    BeamType beamType = BeamType::None;
    int beamLevel = 1;
};