#ifndef MUSIQUE_LOGGING_H
#define MUSIQUE_LOGGING_H

#if DEBUG

#include "AndroidDebug.h"

#else

#define LOGV_TAG(tag, ...)
#define LOGD_TAG(tag, ...)
#define LOGI_TAG(tag, ...)
#define LOGW_TAG(tag, ...)
#define LOGE_TAG(tag, ...)
#define LOGF_TAG(tag, ...)

#define LOGV(...)
#define LOGD(...)
#define LOGI(...)
#define LOGW(...)
#define LOGE(...)
#define LOGF(...)

#endif

#endif //MUSIQUE_LOGGING_H
