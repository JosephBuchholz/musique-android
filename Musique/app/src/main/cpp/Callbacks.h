#ifndef MUSIQUE_CALLBACKS_H
#define MUSIQUE_CALLBACKS_H

#include "RenderData/RenderData.h"
#include "RenderData/PrintRenderData.h"
#include "FrameData/FrameData.h"
#include "ViewModelData.h"
#include "MusicData/SongData.h"
#include <jni.h>

JNIEnv* GetEnv();
jobject ConvertPaintToObject(JNIEnv* env, const Paint& paint);

void UpdateRender(const RenderData& renderData);

void UpdateFrameData(const FrameData& frameData);

void UpdateSongData(const SongData& songData);

void UpdatePrintRenderData(const PrintRenderData& printRenderData);

void UpdateViewModelData(const ViewModelData& viewModelData);

void WriteMidi(char* bytes, size_t size);
void SetMidiVolume(int volume);
void SetMidiReverb(int reverb);

#endif // MUSIQUE_CALLBACKS_H