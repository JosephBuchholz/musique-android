#include "ParseError.h"

#include "../Debuging/AndroidDebug.h"

void ParseError::PrintErrors()
{
    for (const auto& error : m_Errors)
    {
        if (error.errorLevel == ErrorLevel::Error)
            LOGE("[%s] %s: %s", error.parentName.c_str(), error.title.c_str(), error.desc.c_str());
        else if (error.errorLevel == ErrorLevel::Warning)
            LOGW("[%s] %s: %s", error.parentName.c_str(), error.title.c_str(), error.desc.c_str());
        else if (error.errorLevel == ErrorLevel::Fatal)
            LOGF("[%s] %s: %s", error.parentName.c_str(), error.title.c_str(), error.desc.c_str());
        else
            LOGV("[%s] %s: %s", error.parentName.c_str(), error.title.c_str(), error.desc.c_str());
    }
}