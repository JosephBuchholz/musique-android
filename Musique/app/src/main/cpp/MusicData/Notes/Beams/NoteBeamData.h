/**
 * This file contains the definition of the `NoteBeamData` class.
 */

#ifndef MUSIQUE_NOTEBEAMDATA_H
#define MUSIQUE_NOTEBEAMDATA_H

#include "../../../../../../../../../../../../AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/string"

class NoteBeamData
{
public:

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

#endif // MUSIQUE_NOTEBEAMDATA_H