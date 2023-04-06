#ifndef MUSIQUE_JNIHELPER_H
#define MUSIQUE_JNIHELPER_H

#include <jni.h>
#include <string>

class JNIHelper
{
public:
    /**
     * Creates a new `jobjectArray` with a `size` and with an element type of `clazz.`
     *
     * @param env the JIN environment
     * @param size the size of the array
     * @param clazz the class of the elements within the array
     * @return a jobjectArray object
     */
    static jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, jclass clazz);

    /**
     * Creates a new `jobjectArray` with a `size` and with an element type of the class referenced with `className.`
     *
     * @param env the JIN environment
     * @param size the size of the array
     * @param className the name of the class of the elements within the array
     * @return a jobjectArray object
     */
    static jobjectArray CreateNewObjectArray(JNIEnv* env, size_t size, const char* className);

    /**
     * Creates a new jobject with a specified class.
     *
     * @param env the JIN environment
     * @param clazz the class of the object to be created
     * @return a new jobject
     */
    static jobject CreateNewObject(JNIEnv* env, jclass clazz);

    /**
     * Creates a new jobject with a specified class.
     *
     * @param env the JIN environment
     * @param className the name of the class of the object to be created
     * @return a new jobject
     */
    static jobject CreateNewObject(JNIEnv* env, const char* className);

    /**
     * Sets a field on a jobject that has a type of float.
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetFloatField(JNIEnv* env, jobject& object, const char* fieldName, float value);

    /**
     * Sets a field on a jobject that has a type of int.
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetIntField(JNIEnv* env, jobject& object, const char* fieldName, int value);

    /**
     * Sets a field on a jobject that has a type of bool.
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetBoolField(JNIEnv* env, jobject& object, const char* fieldName, bool value);

    /**
     * Sets a field on a jobject that has a type of string.
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetStringField(JNIEnv* env, jobject& object, const char* fieldName, std::string value);

    /**
     * Sets a field on a jobject that has a type of UTF 16 string.
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetStringUTF16Field(JNIEnv* env, jobject& object, const char* fieldName, const uint16_t* value, unsigned int size);

    /**
     * Sets a field on a jobject that does not have a primitive type (i.e. it is an object).
     *
     * @param env[in,out] the JIN environment
     * @param object[out] the object to set the field on
     * @param fieldName[in] the name of the field to set
     * @param value[in] the value to set
     */
    static void SetObjectField(JNIEnv* env, jobject& object, const char* fieldName, jobject value, const char* sig);

    /**
     * Sets an element in an object array.
     *
     * @param env[in,out] the JIN environment
     * @param objectArray[out] the object array to set the element on
     * @param index[in] the index of the element to set
     * @param element[in] the element to set
     */
    static void SetElementInObjectArray(JNIEnv* env, jobjectArray& objectArray, int index, jobject element);

    static jobject GetCallbackObject();

    static void SetMainCallbackObject(jobject callbackObj);

    /**
     * Gets a field on a jobject that has a type of int.
     *
     * @param env[in,out] the JIN environment
     * @param object[in] the object to get the field value from
     * @param fieldName[in] the name of the field to get
     */
    static int GetIntField(JNIEnv* env, const jobject& object, const char* fieldName);

    /**
     * Gets a field on a jobject that has a type of float.
     *
     * @param env[in,out] the JIN environment
     * @param object[in] the object to get the field value from
     * @param fieldName[in] the name of the field to get
     */
    static float GetFloatField(JNIEnv* env, const jobject& object, const char* fieldName);

    /**
     * Gets a field on a jobject that has an enum type.
     *
     * @param env[in,out] The JIN environment.
     * @param object[in] The object to get the field value from.
     * @param fieldName[in] The name of the field to get.
     * @param className[in] The name of the enum class.
     * @return The integer value for the enum value.
     */
    static int GetEnumClassField(JNIEnv* env, const jobject& object, const char* fieldName, const char* className);
};

#endif // MUSIQUE_JNIHELPER_H