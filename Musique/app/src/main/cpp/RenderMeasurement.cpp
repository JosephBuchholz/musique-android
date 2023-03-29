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

BoundingBox RenderMeasurement::GetTextBoundingBox(const Text& text)
{
    BoundingBox boundingBox = BoundingBox();

    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "measureText", "(Lcom/randsoft/apps/musique/renderdata/Text;)Landroid/graphics/RectF;");

    jclass textClass = env->FindClass("com/randsoft/apps/musique/renderdata/Text");
    jobject newText = JNIHelper::CreateNewObject(env, textClass);

    JNIHelper::SetFloatField(env, newText, "x", text.x);
    JNIHelper::SetFloatField(env, newText, "y", text.y);
    /*JNIHelper::SetObjectField(env, newText, "text", env->NewStringUTF(text.text.c_str()), "Ljava/lang/String;");

    jobject paintObject = ConvertPaintToObject(env, text.paint);
    JNIHelper::SetObjectField(env, newText, "paint", paintObject, "Lcom/randsoft/apps/musique/renderdata/Paint;");*/

    // calling callback
    jobject callbackObj = JNIHelper::GetCallbackObject();
    if (callbackObj != nullptr) {
        jobject rectBounds = env->CallObjectMethod(callbackObj, callback, newText);

        boundingBox.position.x = JNIHelper::GetFloatField(env, rectBounds, "left");
        boundingBox.position.y = JNIHelper::GetFloatField(env, rectBounds, "top");

        boundingBox.size.x = JNIHelper::GetFloatField(env, rectBounds, "right") - boundingBox.position.x;
        boundingBox.size.y = JNIHelper::GetFloatField(env, rectBounds, "bottom") - boundingBox.position.y;
    }
    else
    {
        LOGW("Callback Object is null");
    }

    return boundingBox;
}

float RenderMeasurement::MeasureSpannableText(const SpannableText& text)
{
    return 0.0f;
}