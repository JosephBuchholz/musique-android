#pragma once

struct TimeSignature {

    enum class TimeSignatureDisplayType {
        Note = 0, Standard, Common, Cut
    };

    static TimeSignatureDisplayType GetDisplayTypeFromString(const std::string& string) {
        if (string == "common") {
            return TimeSignatureDisplayType::Common;
        } else if (string == "cut") {
            return TimeSignatureDisplayType::Cut;
        }
        return TimeSignatureDisplayType::Standard;
    }

    TimeSignatureDisplayType displayType = TimeSignatureDisplayType::Standard;

    int notes = 4;
    int noteType = 4;
    bool print = true;
};