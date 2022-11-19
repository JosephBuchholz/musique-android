#include <jni.h>
#include <string>
#include "AndroidDebug.h"
#include "Callbacks.h"
#include "App.h"

// got some help from: https://github.com/android/ndk-samples/tree/main/native-midi/app/src/main/cpp

App* app;

extern "C" {

    JavaVM* theJvm;
    jmethodID updateRenderCallback;

    jobject mainActivityRefObj;

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

            // setting 'playing' field
            jfieldID fieldId = env->GetFieldID(viewModelDataClass, "playing","Z");
            viewModelData.playing = env->GetBooleanField(viewModelDataObject, fieldId);

            // setting 'playLineBeatPosition' field
            fieldId = env->GetFieldID(viewModelDataClass, "playLineBeatPosition", "F");
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
}