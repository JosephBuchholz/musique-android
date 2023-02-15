#include "JNIHelper.h"

static jobject MainActivityRefObj;

jobjectArray JNIHelper::CreateNewObjectArray(JNIEnv* env, size_t size, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);
    jobjectArray objectArray = env->NewObjectArray((jsize)size, clazz, object);

    return objectArray;
}

jobjectArray JNIHelper::CreateNewObjectArray(JNIEnv* env, size_t size, const char* className)
{
    return CreateNewObjectArray(env, size, env->FindClass(className));
}

jobject JNIHelper::CreateNewObject(JNIEnv* env, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);

    return object;
}

jobject JNIHelper::CreateNewObject(JNIEnv* env, const char* className)
{
    return CreateNewObject(env, env->FindClass(className));
}

void JNIHelper::SetFloatField(JNIEnv* env, jobject& object, const char* fieldName, float value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "F");
    env->SetFloatField(object, fieldId, value);
}

void JNIHelper::SetIntField(JNIEnv* env, jobject& object, const char* fieldName, int value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "I");
    env->SetIntField(object, fieldId, value);
}

void JNIHelper::SetBoolField(JNIEnv* env, jobject& object, const char* fieldName, bool value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "Z");
    env->SetBooleanField(object, fieldId, value);
}

void JNIHelper::SetStringField(JNIEnv* env, jobject& object, const char* fieldName, std::string value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "Ljava/lang/String;");
    env->SetObjectField(object, fieldId, env->NewStringUTF(value.c_str()));
}

void JNIHelper::SetStringUTF16Field(JNIEnv* env, jobject& object, const char* fieldName, const uint16_t* value, unsigned int size)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "Ljava/lang/String;");
    env->SetObjectField(object, fieldId, env->NewString(value, size));
}

void JNIHelper::SetObjectField(JNIEnv* env, jobject& object, const char* fieldName, jobject value, const char* sig)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, sig);
    env->SetObjectField(object, fieldId, value);
}

void JNIHelper::SetElementInObjectArray(JNIEnv* env, jobjectArray& objectArray, int index, jobject element)
{
    env->SetObjectArrayElement(objectArray, index, element);
}

jobject JNIHelper::GetCallbackObject()
{
    return MainActivityRefObj;
}

void JNIHelper::SetMainCallbackObject(jobject callbackObj)
{
    MainActivityRefObj = callbackObj;
}
