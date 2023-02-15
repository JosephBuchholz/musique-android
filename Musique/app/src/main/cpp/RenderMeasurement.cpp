#include "RenderMeasurement.h"

#include <jni.h>
#include "JNIHelpers/JNIHelper.h"
#include "Callbacks.h"

float RenderMeasurement::MeasureGlyph(const SMuFLGlyph& glyph)
{
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "measureGlyph", "(Lcom/randsoft/apps/musique/renderdata/SMuFLGlyph;)F");

    jclass glyphClass = env->FindClass("com/randsoft/apps/musique/renderdata/SMuFLGlyph");
    jobject newGlyph = JNIHelper::CreateNewObject(env, glyphClass);

    JNIHelper::SetFloatField(env, newGlyph, "x", glyph.x);
    JNIHelper::SetFloatField(env, newGlyph, "y", glyph.y);
    JNIHelper::SetIntField(env, newGlyph, "codePoint", (int)glyph.codePoint);

    jobject paintObject = ConvertPaintToObject(env, glyph.paint);
    JNIHelper::SetObjectField(env, newGlyph, "paint", paintObject, "Lcom/randsoft/apps/musique/renderdata/Paint;");

    // calling callback
    jobject callbackObj = JNIHelper::GetCallbackObject();
    if (callbackObj != nullptr)
        return env->CallFloatMethod(callbackObj, callback, newGlyph);
    else
    {
        LOGW("Callback Object is null");
        return 0.0f;
    }
}

float RenderMeasurement::MeasureGlyph(SMuFLID codePoint)
{
    SMuFLGlyph glyph = SMuFLGlyph(codePoint, 0.0f, 0.0f);
    return MeasureGlyph(glyph);
}

float RenderMeasurement::MeasureText(const Text& text)
{
    return 0.0f;
}

float RenderMeasurement::MeasureSpannableText(const SpannableText& text)
{
    return 0.0f;
}