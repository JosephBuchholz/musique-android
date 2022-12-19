#pragma once

#include <jni.h>

jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);
    jobjectArray objectArray = env->NewObjectArray((jsize)size, clazz, object);

    return objectArray;
}

jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, const char* className)
{
    return CreateNewObjectArray(env, size, env->FindClass(className));
}

jobject CreateNewObject(JNIEnv* env, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);

    return object;
}

jobject CreateNewObject(JNIEnv* env, const char* className)
{
    return CreateNewObject(env, env->FindClass(className));
}

void SetFloatField(JNIEnv* env, jobject& object, const char* fieldName, float value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "F");
    env->SetFloatField(object, fieldId, value);
}

void SetIntField(JNIEnv* env, jobject& object, const char* fieldName, int value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "I");
    env->SetIntField(object, fieldId, value);
}

void SetObjectField(JNIEnv* env, jobject& object, const char* fieldName, jobject value, const char* sig)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, sig);
    env->SetObjectField(object, fieldId, value);
}

void SetElementInObjectArray(JNIEnv* env, jobjectArray& objectArray, int index, jobject element)
{
    env->SetObjectArrayElement(objectArray, index, element);
}