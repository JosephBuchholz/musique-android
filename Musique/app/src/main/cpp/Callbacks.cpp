#include "Callbacks.h"

#include <jni.h>
#include "AndroidDebug.h"

extern JavaVM* theJvm;

extern jobject updateRenderCallbackObj;
extern jmethodID updateRenderCallback;

extern jobject mainActivityRefObj;

jmethodID updateViewModelCallback;
jmethodID writeMidiCallback;

static JNIEnv* GetEnv() {
    JNIEnv* env;
    theJvm->AttachCurrentThread(&env, nullptr);
    if (env == nullptr) {
        LOGE("Error retrieving JNI Env");
    }

    return env;
}

void UpdateRender(const RenderData& renderData) {
    JNIEnv* env = GetEnv();

    // Paint class
    jclass paintClass = env->FindClass("com/randsoft/apps/musique/renderdata/Paint");
    jmethodID paintConstructor = env->GetMethodID(paintClass, "<init>", "(I)V");
    jfieldID paintFieldIdStrokeWidth = env->GetFieldID(paintClass, "strokeWidth", "F");
    jfieldID paintFieldIdStrokeCap = env->GetFieldID(paintClass, "strokeCap", "I");
    jfieldID paintFieldIdTextSize = env->GetFieldID(paintClass, "textSize", "F");

    // creating new RenderData object
    jclass renderDataClass = env->FindClass("com/randsoft/apps/musique/renderdata/RenderData");
    jmethodID constructor = env->GetMethodID(renderDataClass, "<init>", "()V");
    jobject renderDataObject = env->NewObject(renderDataClass, constructor);

    // LINES

    // getting line array field in renderDataObject
    jfieldID fieldIdLines = env->GetFieldID(renderDataClass, "lines",
                                       "[Lcom/randsoft/apps/musique/renderdata/Line;");

    // creating new Line Array
    jclass lineClass = env->FindClass("com/randsoft/apps/musique/renderdata/Line");
    jmethodID lineConstructor = env->GetMethodID(lineClass, "<init>", "()V");
    jobject lineObject = env->NewObject(lineClass, lineConstructor);
    jobjectArray linesArray = env->NewObjectArray((jsize)renderData.Lines.size(), lineClass, lineObject);


    {
        jfieldID fieldIdStartX = env->GetFieldID(lineClass, "startX", "F");
        jfieldID fieldIdStartY = env->GetFieldID(lineClass, "startY", "F");
        jfieldID fieldIdEndX = env->GetFieldID(lineClass, "endX", "F");
        jfieldID fieldIdEndY = env->GetFieldID(lineClass, "endY", "F");
        jfieldID fieldIdPaint = env->GetFieldID(lineClass, "paint",
                                                "Lcom/randsoft/apps/musique/renderdata/Paint;");
        int index = 0;
        for (auto line : renderData.Lines) {
            jobject newLine = env->NewObject(lineClass, lineConstructor);

            env->SetFloatField(newLine, fieldIdStartX, line.startX);
            env->SetFloatField(newLine, fieldIdStartY, line.startY);
            env->SetFloatField(newLine, fieldIdEndX, line.endX);
            env->SetFloatField(newLine, fieldIdEndY, line.endY);

            jobject paintObject = env->NewObject(paintClass, paintConstructor, line.paint.color);
            env->SetFloatField(paintObject, paintFieldIdStrokeWidth, line.paint.strokeWidth);
            env->SetIntField(paintObject, paintFieldIdStrokeCap, (int)line.paint.strokeCap);
            env->SetFloatField(paintObject, paintFieldIdTextSize, line.paint.textSize);

            env->SetObjectField(newLine, fieldIdPaint, paintObject);

            env->SetObjectArrayElement(linesArray, index, newLine);
            index++;
        }
    }

    // set lines array field in renderDataObject
    env->SetObjectField(renderDataObject, fieldIdLines, linesArray);

    // TEXTS

    // getting text array field in renderDataObject
    jfieldID fieldIdTexts = env->GetFieldID(renderDataClass, "texts",
                                            "[Lcom/randsoft/apps/musique/renderdata/Text;");

    // creating new Text Array
    jclass textClass = env->FindClass("com/randsoft/apps/musique/renderdata/Text");
    jmethodID textConstructor = env->GetMethodID(textClass, "<init>", "()V");
    jobject textObject = env->NewObject(textClass, textConstructor);
    jobjectArray textsArray = env->NewObjectArray((jsize)renderData.Texts.size(), textClass, textObject);

    {
        jfieldID fieldIdText = env->GetFieldID(textClass, "text", "Ljava/lang/String;");
        jfieldID fieldIdX = env->GetFieldID(textClass, "x", "F");
        jfieldID fieldIdY = env->GetFieldID(textClass, "y", "F");
        jfieldID fieldIdPaint = env->GetFieldID(textClass, "paint",
                                                "Lcom/randsoft/apps/musique/renderdata/Paint;");

        int index = 0;
        for (auto text : renderData.Texts) {
            jobject newText = env->NewObject(textClass, textConstructor);

            env->SetObjectField(newText, fieldIdText, env->NewStringUTF(text.text.c_str()));
            env->SetFloatField(newText, fieldIdX, text.x);
            env->SetFloatField(newText, fieldIdY, text.y);

            jobject paintObject = env->NewObject(paintClass, paintConstructor, text.paint.color);
            env->SetFloatField(paintObject, paintFieldIdStrokeWidth, text.paint.strokeWidth);
            env->SetIntField(paintObject, paintFieldIdStrokeCap, (int)text.paint.strokeCap);
            env->SetFloatField(paintObject, paintFieldIdTextSize, text.paint.textSize);

            env->SetObjectField(newText, fieldIdPaint, paintObject);

            env->SetObjectArrayElement(textsArray, index, newText);
            index++;
        }
    }

    // set texts array field in renderDataObject
    env->SetObjectField(renderDataObject, fieldIdTexts, textsArray);

    // BITMAPS

    // getting bitmaps array field in renderDataObject
    jfieldID fieldIdBitmaps = env->GetFieldID(renderDataClass, "bitmaps",
                                            "[Lcom/randsoft/apps/musique/renderdata/RenderBitmap;");

    // creating new Bitmap Array
    jclass bitmapClass = env->FindClass("com/randsoft/apps/musique/renderdata/RenderBitmap");
    jmethodID bitmapConstructor = env->GetMethodID(bitmapClass, "<init>", "()V");
    jobject bitmapObject = env->NewObject(bitmapClass, bitmapConstructor);
    jobjectArray bitmapsArray = env->NewObjectArray((jsize)renderData.Bitmaps.size(), bitmapClass, bitmapObject);


    {
        jfieldID fieldIdX = env->GetFieldID(bitmapClass, "x", "F");
        jfieldID fieldIdY = env->GetFieldID(bitmapClass, "y", "F");
        jfieldID fieldIdSx = env->GetFieldID(bitmapClass, "sx", "F");
        jfieldID fieldIdSy = env->GetFieldID(bitmapClass, "sy", "F");
        jfieldID fieldIdAssetId = env->GetFieldID(bitmapClass, "assetId", "I");
        jfieldID fieldIdPaint = env->GetFieldID(bitmapClass, "paint",
                                                "Lcom/randsoft/apps/musique/renderdata/Paint;");
        int index = 0;
        for (auto bitmap : renderData.Bitmaps) {
            jobject newBitmap = env->NewObject(bitmapClass, bitmapConstructor);

            env->SetFloatField(newBitmap, fieldIdX, bitmap.x);
            env->SetFloatField(newBitmap, fieldIdY, bitmap.y);
            env->SetFloatField(newBitmap, fieldIdSx, bitmap.sx);
            env->SetFloatField(newBitmap, fieldIdSy, bitmap.sy);
            env->SetIntField(newBitmap, fieldIdAssetId, (int)bitmap.assetId);

            jobject paintObject = env->NewObject(paintClass, paintConstructor, bitmap.paint.color);
            env->SetFloatField(paintObject, paintFieldIdStrokeWidth, bitmap.paint.strokeWidth);
            env->SetIntField(paintObject, paintFieldIdStrokeCap, (int)bitmap.paint.strokeCap);
            env->SetFloatField(paintObject, paintFieldIdTextSize, bitmap.paint.textSize);

            env->SetObjectField(newBitmap, fieldIdPaint, paintObject);

            env->SetObjectArrayElement(bitmapsArray, index, newBitmap);
            index++;
        }
    }

    // set bitmaps array field in renderDataObject
    env->SetObjectField(renderDataObject, fieldIdBitmaps, bitmapsArray);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, updateRenderCallback, renderDataObject);
}

void UpdateFrameData(const FrameData& frameData)
{
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "onUpdateFrameData",
                                          "(Lcom/randsoft/apps/musique/framedata/FrameData;)V");

    // creating FrameData class
    jclass frameDataClass = env->FindClass("com/randsoft/apps/musique/framedata/FrameData");
    jmethodID constructor = env->GetMethodID(frameDataClass, "<init>", "()V");
    jobject frameDataObject = env->NewObject(frameDataClass, constructor);

    // setting 'playLinePosition' field
    jfieldID fieldId = env->GetFieldID(frameDataClass, "playLinePosition", "F");
    env->SetFloatField(frameDataObject, fieldId, frameData.playLinePosition);

    // setting 'playLinePositionY' field
    fieldId = env->GetFieldID(frameDataClass, "playLinePositionY", "F");
    env->SetFloatField(frameDataObject, fieldId, frameData.playLinePositionY);

    // setting 'playLineHeight' field
    fieldId = env->GetFieldID(frameDataClass, "playLineHeight", "F");
    env->SetFloatField(frameDataObject, fieldId, frameData.playLineHeight);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, frameDataObject);
}

void UpdateViewModelData(const ViewModelData& viewModelData) {
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    updateViewModelCallback = env->GetMethodID(classMainActivity, "onUpdateViewModelData",
                                               "(Lcom/randsoft/apps/musique/ViewModelData;)V");

    // creating ViewModelData class
    jclass viewModelDataClass = env->FindClass("com/randsoft/apps/musique/ViewModelData");
    jmethodID constructor = env->GetMethodID(viewModelDataClass, "<init>", "()V");
    jobject viewModelDataObject = env->NewObject(viewModelDataClass, constructor);

    // -- Setting Fields --

    // setting 'playing' field
    jfieldID fieldId = env->GetFieldID(viewModelDataClass, "playing","Z");
    env->SetBooleanField(viewModelDataObject, fieldId, viewModelData.playing);

    // setting 'playLineBeatPosition' field
    fieldId = env->GetFieldID(viewModelDataClass, "playLineBeatPosition", "F");
    env->SetFloatField(viewModelDataObject, fieldId, viewModelData.playLineBeatPosition);

    // setting 'currentMeasure' field
    fieldId = env->GetFieldID(viewModelDataClass, "currentMeasure", "I");
    env->SetIntField(viewModelDataObject, fieldId, viewModelData.currentMeasure);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, updateViewModelCallback, viewModelDataObject);
}

void WriteMidi(char* bytes, size_t size) {
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    writeMidiCallback = env->GetMethodID(classMainActivity, "writeMidi",
                                               "([B)V");

    jbyteArray byteArray = env->NewByteArray((jsize)size);
    //jbyte* b[size];
    //memcpy(b, bytes, size);
    env->SetByteArrayRegion(byteArray, 0, size, reinterpret_cast<const jbyte *>(bytes));
    //byteArray[0] = b;

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, writeMidiCallback, byteArray);
}

void SetMidiVolume(int volume) {
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "setMidiVolume",
                                         "(I)V");

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, (jint)volume);
}

void SetMidiReverb(int reverb) {
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "setMidiReverb",
                                          "(I)V");

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, (jint)reverb);
}