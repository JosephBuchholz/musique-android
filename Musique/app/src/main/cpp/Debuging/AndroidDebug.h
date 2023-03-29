#ifndef MUSIQUE_ANDROIDDEBUG_H
#define MUSIQUE_ANDROIDDEBUG_H

#include "../../../../../../../../../../AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/android/log.h"


#if !defined(NDEBUG)

#if !defined(LOG_TAG)
#define LOG_TAG "NativeMusique"
#endif

#define LOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) \
  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) \
  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) \
  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) \
  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) \
  __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#else

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)

#endif

#endif // MUSIQUE_ANDROIDDEBUG_H