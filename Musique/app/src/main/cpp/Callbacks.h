#pragma once
#include "RenderData/RenderData.h"
#include "FrameData/FrameData.h"
#include "ViewModelData.h"

void UpdateRender(const RenderData& renderData);

void UpdateFrameData(const FrameData& frameData);

void UpdateViewModelData(const ViewModelData& viewModelData);

void WriteMidi(char* bytes, size_t size);
void SetMidiVolume(int volume);
void SetMidiReverb(int reverb);