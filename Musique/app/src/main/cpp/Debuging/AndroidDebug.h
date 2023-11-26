#ifndef MUSIQUE_ANDROIDDEBUG_H
#define MUSIQUE_ANDROIDDEBUG_H

#include "../../../../../../../../../../AppData/Local/Android/Sdk/ndk/23.1.7779620/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/android/log.h"

#include <assert.h>


#if !defined(NDEBUG)

#if !defined(LOG_TAG)
#define LOG_TAG "NativeMusique"
#endif

#define LOGV_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_VERBOSE, tag, __VA_ARGS__)
#define LOGD_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LOGI_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_INFO, tag, __VA_ARGS__)
#define LOGW_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_WARN, tag, __VA_ARGS__)
#define LOGE_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#define LOGF_TAG(tag, ...) \
  __android_log_print(ANDROID_LOG_FATAL, tag, __VA_ARGS__)
#define ASSERT_MSG(condition, msg) \
  assert((msg, condition))

#define LOGV(...) \
  LOGV_TAG(LOG_TAG, __VA_ARGS__)
#define LOGD(...) \
  LOGD_TAG(LOG_TAG, __VA_ARGS__)
#define LOGI(...) \
  LOGI_TAG(LOG_TAG, __VA_ARGS__)
#define LOGW(...) \
  LOGW_TAG(LOG_TAG, __VA_ARGS__)
#define LOGE(...) \
  LOGE_TAG(LOG_TAG, __VA_ARGS__)
#define LOGF(...) \
  LOGF_TAG(LOG_TAG, __VA_ARGS__)
#define ASSERT(condition) \
  assert(condition)

#else

#define LOGV_TAG(tag, ...)
#define LOGD_TAG(tag, ...)
#define LOGI_TAG(tag, ...)
#define LOGW_TAG(tag, ...)
#define LOGE_TAG(tag, ...)
#define LOGF_TAG(tag, ...)
#define ASSERT_MSG(condition, msg)

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)
#define ASSERT(condition)

#endif

#endif // MUSIQUE_ANDROIDDEBUG_H