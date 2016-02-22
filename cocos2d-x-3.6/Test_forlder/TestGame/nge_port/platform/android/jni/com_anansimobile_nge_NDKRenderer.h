/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_anansimobile_nge_NDKRenderer */

#ifndef _Included_com_anansimobile_nge_NDKRenderer
#define _Included_com_anansimobile_nge_NDKRenderer
#ifdef __cplusplus
extern "C" {
#endif
#undef com_anansimobile_nge_NDKRenderer_framesPerSecond
#define com_anansimobile_nge_NDKRenderer_framesPerSecond 30LL
#undef com_anansimobile_nge_NDKRenderer_frameInterval
#define com_anansimobile_nge_NDKRenderer_frameInterval 33LL
/*
 * Class:     com_anansimobile_nge_NDKRenderer
 * Method:    nativeInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeInit
  (JNIEnv *, jobject);

/*
 * Class:     com_anansimobile_nge_NDKRenderer
 * Method:    nativeResize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeResize
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_anansimobile_nge_NDKRenderer
 * Method:    nativeRender
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeRender
  (JNIEnv *, jobject);

/*
 * Class:     com_anansimobile_nge_NDKRenderer
 * Method:    nativeDone
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_anansimobile_nge_NDKRenderer_nativeDone
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif