/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <pthread.h>
#include <semaphore.h>
/* Header for class com_demo_lbs_myndk_jni_PThreadJni */

#ifndef _Included_com_demo_lbs_myndk_jni_PThreadJni
#define _Included_com_demo_lbs_myndk_jni_PThreadJni
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_demo_lbs_myndk_jni_PThreadJni
 * Method:    nativeInit
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeInit
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_demo_lbs_myndk_jni_PThreadJni
 * Method:    nativeFree
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeFree
  (JNIEnv *, jobject);

/*
 * Class:     com_demo_lbs_myndk_jni_PThreadJni
 * Method:    nativeWorker
 * Signature: (Ljava/lang/Object;II)V
 */
JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_nativeWorker
  (JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     com_demo_lbs_myndk_jni_PThreadJni
 * Method:    posixThreads
 * Signature: (Ljava/lang/Object;II)V
 */
JNIEXPORT void JNICALL Java_com_demo_lbs_myndk_jni_PThreadJni_posixThreads
  (JNIEnv *, jobject, jobject, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
