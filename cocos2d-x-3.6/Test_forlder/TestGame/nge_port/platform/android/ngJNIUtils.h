/*
 *  ngJNIUtils.h
 *  lib_nge_project
 *
 *  Modified by Chen Zhen on 12-10-10.
 *  Copyright 2011 Anansi Mobile. All rights reserved.
 *
 */


#include <jni.h>
#include <core/NGE_Macros.h>
#include <nsl/ngArrayList.h>

#ifndef __NGJNIUTILS_H__
#define __NGJNIUTILS_H__

/* if add new class in java which will be called in jni, enum it here and get ref in init. */
typedef enum __E_JAVA_CLASS__ {
    jclsInvalid = -1,
    jclsNextGenEngine = 0,
    jclsMainActivity,
    jclsNGDevice,
    jclsNGStringHelper,
    jclsNGImagePicker,
    jclsNGMediaPlayer,
    jclsNGStoreKit,
    jclsNGNotification,
    jclsNGLabel,
    jclsNGLocation,
    jclsNGReachability,
    jclsNGAlertDialog,
    jclsNGTextInput,
    jclsNGAppRater,
    /* java class here */
    //<<
    jclsString,
    jclsMediaPlayer,
    //>>
    jclsNum,
} JavaClass;

jclass getJavaClass(JavaClass jclsId);
void setJavaClass(JavaClass jclsId, jclass jcls);
void InitJavaClass();

#if 1 //port to cocos2dx
void jni_init_nge_port(JNIEnv* env);
#endif

//#ifdef __cplusplus
//extern "C" {
//#endif

JNIEnv* GetJNIEnv();

    extern JavaVM* g_jvm;
    extern ngArrayList g_jcls;

    jstring stoJstring(const char * str, JNIEnv* jenv = NULL);
    char* jstringTostring(jstring jstr, JNIEnv* jenv = NULL);
    
    /*!
     @function mainActivity
     @abstract get the main activity in java environment.
     @discussion must call GetJNIEnv()->DeleteLocalRef(jobject) with return value if it is no longer in use.
     */
    jobject mainActivity(/* JNIEnv* env */);
    
    /*!
     @function mainActivityClass
     @abstract get the class of main activity in java environment.
     @discussion 
     */
    jclass mainActivityClass();

//#ifdef __cplusplus
//}
//#endif

JNIEnv* AttachCurrentJNIEnv();
void DetachCurrentJNIEnv();

#endif  //__NGJNIUTILS_H__
