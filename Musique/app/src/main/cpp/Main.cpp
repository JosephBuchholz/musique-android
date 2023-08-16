#include <jni.h>
#include <string>
#include "Debuging/AndroidDebug.h"
#include "Callbacks.h"
#include "App.h"
#include "JNIHelpers/JNIHelper.h"
#include "Events/InputEvent.h"

// got some help from: https://github.com/android/ndk-samples/tree/main/native-midi/app/src/main/cpp

App* app;

extern "C" {

    JavaVM* theJvm;
    jmethodID updateRenderCallback;

    jobject mainActivityRefObj;

    /*JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
        JNIEnv* env;
        if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return JNI_ERR;
        }

        // Find your class. JNI_OnLoad is called from the correct class loader context for this to work.
        jclass c = env->FindClass("com/example/app/package/MyClass");
        if (c == nullptr) return JNI_ERR;

        // Register your class' native methods.
        static const JNINativeMethod methods[] = {
                {"nativeFoo", "()V", reinterpret_cast<void*>(nativeFoo)},
                {"nativeBar", "(Ljava/lang/String;I)Z", reinterpret_cast<void*>(nativeBar)},
        };
        int rc = env->RegisterNatives(c, methods, sizeof(methods)/sizeof(JNINativeMethod));
        if (rc != JNI_OK) return rc;

        return JNI_VERSION_1_6;
    }*/

    // on library loaded
    /*JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        LOGI("JIN_OnLoad");
        JNIEnv* env;
        if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            return JNI_ERR;
        }

        return JNI_VERSION_1_6;
    }*/

    JNIEXPORT jint JIN_OnUnload()
    {

    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_startRendering(JNIEnv* env, jobject instance) {
        LOGI("Starting Rendering");
        if (app != nullptr) {
            app->StartRendering();
        }

        /*RenderData renderData = RenderData();

        Line line = Line(15.0f, 15.0f, 30.0f, 14.0f);
        renderData.Lines.push_back(line);

        Line line2 = Line(30.0f, 14.0f, 30.0f, 30.0f);
        renderData.Lines.push_back(line2);

        UpdateRender(renderData);*/
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_initNative(JNIEnv* env, jobject instance) {
        LOGI("Initializing Native C++");

        if (app == nullptr) {
            LOGI("creating app");
            app = new App();
        } else {
            LOGI("not creating app");
        }

        LOGI("created app");

        env = env;
        env->GetJavaVM(&theJvm);

        mainActivityRefObj = env->NewGlobalRef(instance);
        JNIHelper::SetMainCallbackObject(mainActivityRefObj);

        jclass classMainActivity = env->FindClass("com/randsoft/apps/musique/MainActivity");
        updateRenderCallback = env->GetMethodID(classMainActivity, "onUpdateRender",
                                                "(Lcom/randsoft/apps/musique/renderdata/RenderData;)V");
    }

    bool isUpdating = false;

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_destroyNative(JNIEnv* env, jobject instance) {
        LOGI("Destroying Native C++");
        if (isUpdating)
        {
            LOGI("IS UPDATEING");
        }
        while (isUpdating)
        {
            LOGI("is updateing");
        }
        if (!isUpdating) {
            if (app != nullptr) {
                LOGI("Deleting app");
                delete app;
                app = nullptr;
            } else {
                LOGI("Not deleting app");
            }
        }
        else
        {
            LOGE("FAILED TO DELETE");
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onUpdate(JNIEnv* env, jobject instance, jdouble dt) {
        if (app != nullptr) {
            isUpdating = true;
            app->OnUpdate(dt);
            isUpdating = false;
       }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_setViewModelData(JNIEnv* env, jobject instance,
                                                                 jobject viewModelDataObject) {
        if (app != nullptr) {
            ViewModelData viewModelData = ViewModelData();

            // getting ViewModelData class
            jclass viewModelDataClass = env->FindClass("com/randsoft/apps/musique/ViewModelData");

            // -- Getting Fields --

            // setting 'playLineBeatPosition' field
            jfieldID fieldId = env->GetFieldID(viewModelDataClass, "playLineBeatPosition", "F");
            viewModelData.playLineBeatPosition = env->GetFloatField(viewModelDataObject, fieldId);

            // setting 'currentMeasure' field
            fieldId = env->GetFieldID(viewModelDataClass, "currentMeasure", "I");
            viewModelData.currentMeasure = env->GetIntField(viewModelDataObject, fieldId);

            app->SetViewModelData(viewModelData);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onPlayButtonToggledNative(JNIEnv* env, jobject instance,
                                                                          jboolean state) {
        if (app != nullptr) {
            app->OnPlayButtonToggled(state);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onMetronomeButtonToggledNative(JNIEnv* env, jobject instance,
                                                                          jboolean state) {

        LOGE_TAG("Main", "metronomeIsOn: %d, appIsNull: %d", state, app == nullptr);
        if (app != nullptr) {
            app->OnMetronomeToggled(state);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onResetButtonPressedNative(JNIEnv* env, jobject instance) {
        if (app != nullptr) {
            app->OnResetButtonPressed();
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onPlayProgressChangedNative(JNIEnv* env, jobject instance, jfloat progress) {
        if (app != nullptr) {
            app->OnPlayProgressChanged(progress);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onMidiStart(JNIEnv* env, jobject instance) {
        if (app != nullptr) {
            app->OnMidiStart();
        }
    }

    JNIEXPORT jboolean JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onUpdatePrintLayoutNative(JNIEnv* env, jobject instance,
                                                                          jobject attributes) {
        if (app != nullptr) {
            bool layoutChanged = app->OnUpdatePrintLayout();
            return layoutChanged;
        }
        return false;
    }

    JNIEXPORT jint JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onCalculateNumPagesNative(JNIEnv* env, jobject instance) {
        if (app != nullptr) {
            int numPages = app->OnCalculateNumPages();
            return numPages;
        }
        return 0;
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_loadSongFromString(JNIEnv* env, jobject instance,
                                                                   jstring string) {
        if (app != nullptr) {
            jboolean isCopy = true;
            app->LoadSongFromString(env->GetStringUTFChars(string, &isCopy));
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_updateInstrumentInfoNative(JNIEnv* env, jobject instance,
                                                                           jobject info,
                                                                           jint index) {
        if (app != nullptr) {
            InstrumentInfo instrumentInfo = InstrumentInfo();
            jfieldID isVisibleField = env->GetFieldID(env->GetObjectClass(info), "visible", "Z");
            jboolean isVisible = env->GetBooleanField(info, isVisibleField);
            instrumentInfo.visible = isVisible;
            app->UpdateInstrumentInfo(instrumentInfo, int(index));
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onSettingsChangedNative(JNIEnv* env, jobject instance, jobject settings) {
        if (app != nullptr) {
            Settings newSettings = Settings();

            newSettings.chordSymbolStyle = (Settings::ChordSymbolStyle)JNIHelper::GetEnumClassField(env, settings, "chordSymbolStyle", "Lcom/randsoft/apps/musique/ChordSymbolStyleType;");
            newSettings.showChordDiagram = (Settings::ShowChordDiagram)JNIHelper::GetEnumClassField(env, settings, "showChordDiagrams", "Lcom/randsoft/apps/musique/ShowChordDiagramsType;");

            //jfieldID field = env->GetFieldID(env->GetObjectClass(settings), "showChordDiagrams", "Lcom/randsoft/apps/musique/ShowChordDiagramsType;")
            //jfieldID musicLayoutField = env->GetFieldID(env->GetObjectClass(settings), "musicLayout", "Lcom/randsoft/apps/musique/SettingsDialogFragment$Settings$MusicLayout;");
            //jobject musicLayoutObject = env->GetObjectField(settings, musicLayoutField);
            //jint musicLayoutInt = env->CallIntMethod(musicLayoutObject, env->GetMethodID(env->GetObjectClass(musicLayoutObject), "ordinal", "()I"));
            //newSettings.musicLayout = (Settings::MusicLayout)musicLayoutInt;

            app->UpdateSettings(newSettings);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onInputEventNative(JNIEnv* env, jobject instance, jobject inputEvent) {
        if (app != nullptr) {
            /*jfieldID eventTypeField = env->GetFieldID(env->GetObjectClass(inputEvent), "type", "Lcom/randsoft/apps/musique/event/InputEvent$InputEventType;");
            jobject eventTypeObject = env->GetObjectField(inputEvent, eventTypeField);
            jint eventTypeInt = env->CallIntMethod(eventTypeObject, env->GetMethodID(env->GetObjectClass(eventTypeObject), "ordinal", "()I"));*/

            InputEvent newInputEvent = InputEvent();

            newInputEvent.type = (InputEvent::InputEventType)JNIHelper::GetEnumClassField(env, inputEvent, "type", "Lcom/randsoft/apps/musique/event/InputEvent$InputEventType;");

            newInputEvent.position.x = JNIHelper::GetFloatField(env, inputEvent, "x");
            newInputEvent.position.y = JNIHelper::GetFloatField(env, inputEvent, "y");

            app->OnInputEvent(newInputEvent);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_randsoft_apps_musique_MainActivity_onVolumeChangedNative(JNIEnv* env, jobject instance, jfloat volume) {
        if (app != nullptr) {
            app->OnVolumeChanged(volume);
        }
    }
}