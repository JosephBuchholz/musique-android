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

    // setting 'playProgress' field
    fieldId = env->GetFieldID(frameDataClass, "playProgress", "F");
    env->SetFloatField(frameDataObject, fieldId, frameData.playProgress);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, frameDataObject);
}

void UpdateSongData(const SongData& songData)
{
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "onUpdateSongData",
                                          "(Lcom/randsoft/apps/musique/songdata/SongData;)V");

    // creating SongData class
    jclass songDataClass = env->FindClass("com/randsoft/apps/musique/songdata/SongData");
    jmethodID constructor = env->GetMethodID(songDataClass, "<init>", "()V");
    jobject songDataObject = env->NewObject(songDataClass, constructor);

    // String class
    jclass stringClass = env->FindClass("java/lang/String");
    jmethodID stringConstructor = env->GetMethodID(stringClass, "<init>", "()V");

    // setting 'songTitle' field
    jfieldID fieldId = env->GetFieldID(songDataClass, "songTitle", "Ljava/lang/String;");
    std::string title = songData.songTitle + " " + songData.movementTitle + " " + songData.movementNumber;
    env->SetObjectField(songDataObject, fieldId, env->NewStringUTF(title.c_str()));

    // setting 'instruments' field
    fieldId = env->GetFieldID(songDataClass, "instruments", "[Lcom/randsoft/apps/musique/songdata/InstrumentInfo;");

    // creating new InstrumentInfo Array
    jclass instrumentInfoClass = env->FindClass("com/randsoft/apps/musique/songdata/InstrumentInfo");
    jmethodID instrumentInfoConstructor = env->GetMethodID(instrumentInfoClass, "<init>", "()V");
    jobject instrumentInfoObject = env->NewObject(instrumentInfoClass, instrumentInfoConstructor);
    jobjectArray instrumentInfoArray = env->NewObjectArray((jsize)songData.instrumentInfos.size(), instrumentInfoClass, instrumentInfoObject);

    {
        jfieldID fieldIdName = env->GetFieldID(instrumentInfoClass, "name", "Ljava/lang/String;");
        jfieldID fieldIdVolume = env->GetFieldID(instrumentInfoClass, "volume", "I");
        jfieldID fieldIdVisible = env->GetFieldID(instrumentInfoClass, "visible", "Z");

        int index = 0;
        for (const auto& instrumentInfo : songData.instrumentInfos) {
            jobject newInstrumentInfo = env->NewObject(instrumentInfoClass, instrumentInfoConstructor);

            env->SetObjectField(newInstrumentInfo, fieldIdName, env->NewStringUTF(instrumentInfo.name.c_str()));
            env->SetIntField(newInstrumentInfo, fieldIdVolume, instrumentInfo.volume);
            env->SetBooleanField(newInstrumentInfo, fieldIdVisible, instrumentInfo.visible);

            env->SetObjectArrayElement(instrumentInfoArray, index, newInstrumentInfo);
            index++;
        }
    }

    // set instruments array field in songDataObject
    env->SetObjectField(songDataObject, fieldId, instrumentInfoArray);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, songDataObject);
}

jobject ConvertRenderDataToObject(JNIEnv* env, const RenderData& renderData)
{
    // Paint class
    jclass paintClass = env->FindClass("com/randsoft/apps/musique/renderdata/Paint");
    jmethodID paintConstructor = env->GetMethodID(paintClass, "<init>", "(I)V");
    jfieldID paintFieldIdStrokeWidth = env->GetFieldID(paintClass, "strokeWidth", "F");
    jfieldID paintFieldIdStrokeCap = env->GetFieldID(paintClass, "strokeCap", "I");
    jfieldID paintFieldIdTextSize = env->GetFieldID(paintClass, "textSize", "F");
    jfieldID paintFieldIdIsItalic = env->GetFieldID(paintClass, "isItalic", "Z");
    jfieldID paintFieldIdIsBold = env->GetFieldID(paintClass, "isBold", "Z");
    jfieldID paintFieldIdAlign = env->GetFieldID(paintClass, "align", "I");

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
        for (const auto& text : renderData.Texts) {
            jobject newText = env->NewObject(textClass, textConstructor);

            env->SetObjectField(newText, fieldIdText, env->NewStringUTF(text.text.c_str()));
            env->SetFloatField(newText, fieldIdX, text.x);
            env->SetFloatField(newText, fieldIdY, text.y);

            jobject paintObject = env->NewObject(paintClass, paintConstructor, text.paint.color);
            env->SetFloatField(paintObject, paintFieldIdStrokeWidth, text.paint.strokeWidth);
            env->SetIntField(paintObject, paintFieldIdStrokeCap, (int)text.paint.strokeCap);
            env->SetFloatField(paintObject, paintFieldIdTextSize, text.paint.textSize);
            env->SetBooleanField(paintObject, paintFieldIdIsItalic, text.paint.isItalic);
            env->SetBooleanField(paintObject, paintFieldIdIsBold, text.paint.isBold);
            env->SetIntField(paintObject, paintFieldIdAlign, (int)text.paint.align);

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

    // CUBIC CURVES

    // getting cubic curve array field in renderDataObject
    jfieldID fieldIdCubicCurves = env->GetFieldID(renderDataClass, "cubicCurves",
                                                  "[Lcom/randsoft/apps/musique/renderdata/CubicCurve;");

    // creating new CubicCurve Array
    jclass cubicCurveClass = env->FindClass("com/randsoft/apps/musique/renderdata/CubicCurve");
    jmethodID cubicCurveConstructor = env->GetMethodID(cubicCurveClass, "<init>", "()V");
    jobject cubicCurveObject = env->NewObject(cubicCurveClass, cubicCurveConstructor);
    jobjectArray cubicCurvesArray = env->NewObjectArray((jsize)renderData.CubicCurves.size(), cubicCurveClass, cubicCurveObject);

    {
        jfieldID fieldIdX1 = env->GetFieldID(cubicCurveClass, "x1", "F");
        jfieldID fieldIdY1 = env->GetFieldID(cubicCurveClass, "y1", "F");
        jfieldID fieldIdX2 = env->GetFieldID(cubicCurveClass, "x2", "F");
        jfieldID fieldIdY2 = env->GetFieldID(cubicCurveClass, "y2", "F");
        jfieldID fieldIdX3 = env->GetFieldID(cubicCurveClass, "x3", "F");
        jfieldID fieldIdY3 = env->GetFieldID(cubicCurveClass, "y3", "F");
        jfieldID fieldIdX4 = env->GetFieldID(cubicCurveClass, "x4", "F");
        jfieldID fieldIdY4 = env->GetFieldID(cubicCurveClass, "y4", "F");
        jfieldID fieldIdPaint = env->GetFieldID(cubicCurveClass, "paint",
                                                "Lcom/randsoft/apps/musique/renderdata/Paint;");
        int index = 0;
        for (auto curve : renderData.CubicCurves) {
            jobject newCurve = env->NewObject(cubicCurveClass, cubicCurveConstructor);

            env->SetFloatField(newCurve, fieldIdX1, curve.x1);
            env->SetFloatField(newCurve, fieldIdY1, curve.y1);
            env->SetFloatField(newCurve, fieldIdX2, curve.x2);
            env->SetFloatField(newCurve, fieldIdY2, curve.y2);
            env->SetFloatField(newCurve, fieldIdX3, curve.x3);
            env->SetFloatField(newCurve, fieldIdY3, curve.y3);
            env->SetFloatField(newCurve, fieldIdX4, curve.x4);
            env->SetFloatField(newCurve, fieldIdY4, curve.y4);

            jobject paintObject = env->NewObject(paintClass, paintConstructor, curve.paint.color);
            env->SetFloatField(paintObject, paintFieldIdStrokeWidth, curve.paint.strokeWidth);
            env->SetIntField(paintObject, paintFieldIdStrokeCap, (int)curve.paint.strokeCap);
            env->SetFloatField(paintObject, paintFieldIdTextSize, curve.paint.textSize);

            env->SetObjectField(newCurve, fieldIdPaint, paintObject);

            env->SetObjectArrayElement(cubicCurvesArray, index, newCurve);
            index++;
        }
    }
    return renderDataObject;
}

void UpdateRender(const RenderData& renderData) {
    JNIEnv* env = GetEnv();

    jobject renderDataObject = ConvertRenderDataToObject(env, renderData);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, updateRenderCallback, renderDataObject);
}

void UpdatePrintRenderData(const PrintRenderData& printRenderData)
{
    JNIEnv* env = GetEnv();

    jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
    jmethodID callback = env->GetMethodID(classMainActivity, "onUpdatePrintRenderData",
                                          "(Lcom/randsoft/apps/musique/renderdata/PrintRenderData;)V");

    // creating new PrintRenderData object
    jclass printRenderDataClass = env->FindClass("com/randsoft/apps/musique/renderdata/PrintRenderData");
    jmethodID constructor = env->GetMethodID(printRenderDataClass, "<init>", "()V");
    jobject printRenderDataObject = env->NewObject(printRenderDataClass, constructor);

    // PAGES

    // getting pages array field
    jfieldID fieldIdPages = env->GetFieldID(printRenderDataClass, "pages",
                                            "[Lcom/randsoft/apps/musique/renderdata/RenderData;");

    // creating new RenderData Array
    jclass renderDataClass = env->FindClass("com/randsoft/apps/musique/renderdata/RenderData");
    jmethodID renderDataConstructor = env->GetMethodID(renderDataClass, "<init>", "()V");
    jobject renderDataObject = env->NewObject(renderDataClass, renderDataConstructor);
    jobjectArray renderDataArray = env->NewObjectArray((jsize)printRenderData.pages.size(), renderDataClass, renderDataObject);


    {
        int index = 0;
        for (const auto& page : printRenderData.pages) {
            jobject newPageRenderData = ConvertRenderDataToObject(env, page); // get RenderData as a jobject

            env->SetObjectArrayElement(renderDataArray, index, newPageRenderData);
            index++;
        }
    }

    // set pages array field
    env->SetObjectField(printRenderDataObject, fieldIdPages, renderDataArray);

    // calling callback
    env->CallVoidMethod(mainActivityRefObj, callback, printRenderDataObject);
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