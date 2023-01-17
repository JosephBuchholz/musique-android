#pragma once

#include <jni.h>

/**
 * Creates a new `jobjectArray` with a `size` and with an element type of `clazz.`
 *
 * @param env the JIN environment
 * @param size the size of the array
 * @param clazz the class of the elements within the array
 * @return a jobjectArray object
 */
jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);
    jobjectArray objectArray = env->NewObjectArray((jsize)size, clazz, object);

    return objectArray;
}

/**
 * Creates a new `jobjectArray` with a `size` and with an element type of the class referenced with `className.`
 *
 * @param env the JIN environment
 * @param size the size of the array
 * @param className the name of the class of the elements within the array
 * @return a jobjectArray object
 */
jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, const char* className)
{
    return CreateNewObjectArray(env, size, env->FindClass(className));
}

/**
 * Creates a new jobject with a specified class.
 *
 * @param env the JIN environment
 * @param clazz the class of the object to be created
 * @return a new jobject
 */
jobject CreateNewObject(JNIEnv* env, jclass clazz)
{
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jobject object = env->NewObject(clazz, constructor);

    return object;
}

/**
 * Creates a new jobject with a specified class.
 *
 * @param env the JIN environment
 * @param className the name of the class of the object to be created
 * @return a new jobject
 */
jobject CreateNewObject(JNIEnv* env, const char* className)
{
    return CreateNewObject(env, env->FindClass(className));
}

/**
 * Sets a field on a jobject that has a type of float.
 *
 * @param env[in,out] the JIN environment
 * @param object[out] the object to set the field on
 * @param fieldName[in] the name of the field to set
 * @param value[in] the value to set
 */
void SetFloatField(JNIEnv* env, jobject& object, const char* fieldName, float value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "F");
    env->SetFloatField(object, fieldId, value);
}

/**
 * Sets a field on a jobject that has a type of int.
 *
 * @param env[in,out] the JIN environment
 * @param object[out] the object to set the field on
 * @param fieldName[in] the name of the field to set
 * @param value[in] the value to set
 */
void SetIntField(JNIEnv* env, jobject& object, const char* fieldName, int value)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, "I");
    env->SetIntField(object, fieldId, value);
}

/**
 * Sets a field on a jobject that does not have a primitive type (i.e. it is an object).
 *
 * @param env[in,out] the JIN environment
 * @param object[out] the object to set the field on
 * @param fieldName[in] the name of the field to set
 * @param value[in] the value to set
 */
void SetObjectField(JNIEnv* env, jobject& object, const char* fieldName, jobject value, const char* sig)
{
    jfieldID fieldId = env->GetFieldID(env->GetObjectClass(object), fieldName, sig);
    env->SetObjectField(object, fieldId, value);
}

/**
 * Sets an element in an object array.
 *
 * @param env[in,out] the JIN environment
 * @param objectArray[out] the object array to set the element on
 * @param index[in] the index of the element to set
 * @param element[in] the element to set
 */
void SetElementInObjectArray(JNIEnv* env, jobjectArray& objectArray, int index, jobject element)
{
    env->SetObjectArrayElement(objectArray, index, element);
}