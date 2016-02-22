/*
 *  ngDeviceAndroid.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *	Modified by lg2000 on 09-04-13.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#include "ngDeviceAndroid.h"
#include <android/ngJNIUtils.h>

ngDevice* ngDevice::CreateInstance() {
    ngDevice::m_pThis = DNEW(ngDeviceAndroid);
    return ngDevice::m_pThis;
}
 
void ngDevice::ReleaseInstance(void *pObj) {
    ngDeviceAndroid* pThis = (ngDeviceAndroid*)pObj;
    DELETE_OBJECT(pThis, ngDeviceAndroid);
}

ngDeviceAndroid::ngDeviceAndroid() {

}

ngDeviceAndroid::~ngDeviceAndroid() {

}

void ngDeviceAndroid::Initialize() {

}

boolean ngDeviceAndroid::IsFileExist(const char* file) {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "isFileExist", "(Ljava/lang/String;)Z");
    
    jstring jstrFileName = stoJstring(file, pEnv);
    
    jboolean exist = pEnv->CallStaticBooleanMethod(cls, mid, jstrFileName);

    pEnv->DeleteLocalRef(jstrFileName);

    return exist;
}

boolean	ngDeviceAndroid::GetFilePath(ngStringV2& out, const char* file) {
    
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getFilePath", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jfile = stoJstring(file, pEnv);
    jstring path = (jstring)pEnv->CallStaticObjectMethod(cls, mid, jfile);

    char* str = jstringTostring(path, pEnv);
    out = str;
    SAFE_FREE(str);

    pEnv->DeleteLocalRef(jfile);
    pEnv->DeleteLocalRef(path);

//    if (ngTaskManager::GetInstance()->InTask()) {
//        DetachCurrentJNIEnv();
//    }

    return TRUE;
}
void ngDeviceAndroid::GetFileWritePath(ngStringV2& out, const char* file) {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getFileWritePath", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring jstrFile = stoJstring(file, pEnv);
    jstring path = (jstring)pEnv->CallStaticObjectMethod(cls, mid, jstrFile);

    pEnv->DeleteLocalRef(jstrFile);

    char* str = jstringTostring(path);
    out = str;
    SAFE_FREE(str);

    pEnv->DeleteLocalRef(path);

}

const char* ngDeviceAndroid::GetDeviceFamily() {
    return "Android";
}

const char* ngDeviceAndroid::GetDeviceType() {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getModel", "()Ljava/lang/String;");
    
    static char model[33] = {'\0'};
    
    if (strlen(model) <= 0) {
        
        jstring jModel = (jstring)pEnv->CallStaticObjectMethod(cls, mid);
        
        char* str = jstringTostring(jModel);
        memcpy(model, str, 32/* * sizeof(char)*/);
        SAFE_FREE(str);
        
        pEnv->DeleteLocalRef(jModel);
    }
    
    return model;
}

const char* ngDeviceAndroid::GetDeviceVersion() {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getDeviceVersion", "()Ljava/lang/String;");

    static char version[33] = {'\0'};
    
    if (strlen(version) <= 0) {
        
        jstring jVersion = (jstring)pEnv->CallStaticObjectMethod(cls, mid);
        
        char* str = jstringTostring(jVersion);
        memcpy(version, str, 32/* * sizeof(char)*/);
        SAFE_FREE(str);
        
        pEnv->DeleteLocalRef(jVersion);
    }
    
    return version;
}

float ngDeviceAndroid::GetDeviceVersionf() {
    /* Android version like "4.2.1", so we ignore last version, like "4.2". */
    NGCSTR strVer = GetDeviceVersion();
    NG_DEBUG_LOG("[device] device version (float): %f\n", atof(strVer));
    return atof(strVer);
}

#if 0
const char* ngDeviceAndroid::GetDevicePlatform() {
    return "Android Device";
}
#endif

NGCSTR ngDeviceAndroid::GetChannelID() const {

    /* ignore channel id change in runtime. */
    if (strlen(m_channelID) > 0) {
        return m_channelID;
    }
    
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass mainActivityCls = mainActivityClass();
    jmethodID mid = pEnv->GetMethodID(mainActivityCls, "getChannelId", "()Ljava/lang/String;");
    
    jobject rootActivity = mainActivity();
    jstring jstrChannelId = (jstring)pEnv->CallObjectMethod(rootActivity, mid);
    
    NGASSERT(jstrChannelId != NULL);
    
    char* strChannelId = jstringTostring(jstrChannelId);
    if (strChannelId != NULL) {
        strncpy((char*)m_channelID, strChannelId, MAX_CHANNEL_ID_LEN);
        DFREE(strChannelId);
    }
    
    pEnv->DeleteLocalRef(jstrChannelId);
    pEnv->DeleteLocalRef(rootActivity);

    return m_channelID;
}

NGCSTR ngDeviceAndroid::CurrentLocaleLanguage() {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getLocaleLanguage", "()Ljava/lang/String;");
    jstring lang = (jstring)pEnv->CallStaticObjectMethod(cls, mid);
    
    if (lang == NULL) {
        NGASSERT(0);
        return "en";
    }
    
    static char* strLang = NULL;
    
    if (strLang) {
        NG_DEBUG_LOG("free old lang str: %s\n", strLang);
        DFREE(strLang);
    }
    
    strLang = jstringTostring(lang);    
    pEnv->DeleteLocalRef(lang);
    
    return strLang;
}

NGCSTR ngDeviceAndroid::CurrentLocaleRegion() {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getLocaleCountry", "()Ljava/lang/String;");
    jstring region = (jstring)pEnv->CallStaticObjectMethod(cls, mid);
    
    if (region == NULL) {
        NGASSERT(0);
        return "en";
    }
    
    static char* strregion = NULL;
    
    if (strregion) {
        NG_DEBUG_LOG("free old region str: %s\n", strregion);
        DFREE(strregion);
    }
    
    strregion = jstringTostring(region);    
    pEnv->DeleteLocalRef(region);
    
    return strregion;
}

static const char* _get_device_id() {
    
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getDeviceId", "()Ljava/lang/String;");
    jstring deviceId = (jstring)pEnv->CallStaticObjectMethod(cls, mid);
    
    if (deviceId == NULL) {
        //FIXME: here is a bug found in moto milestone, can not get device id.
        return "01234567890";
    }
    
    char* strDeviceId = jstringTostring(deviceId);    
    pEnv->DeleteLocalRef(deviceId);
    
    static char sDeviceId[257] = {'\0'};
    sDeviceId[256] = '\0';
    strncpy(sDeviceId, strDeviceId, 256);
    DFREE(strDeviceId);
    
    return sDeviceId;
}

#if 0
static const char* _get_unique_device_id() {
    
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);

    //mac addr
    jmethodID mGetMacAddr = pEnv->GetStaticMethodID(cls, "getMacAddress", "()Ljava/lang/String;");
    jstring deviceMacId = (jstring)pEnv->CallStaticObjectMethod(cls, mGetMacAddr);

    char* strDeviceMacId = jstringTostring(deviceMacId);    
    pEnv->DeleteLocalRef(deviceMacId);
    
    //package name
    jmethodID mGetPackageName = pEnv->GetStaticMethodID(cls, "getPackageName", "()Ljava/lang/String;");
    jstring packageName = (jstring)pEnv->CallStaticObjectMethod(cls, mGetPackageName);
    
    char* strPackageName = jstringTostring(packageName);    
    pEnv->DeleteLocalRef(packageName);
    
    //concat
    char* strSrc = DNEWARR(char, 257);
    snprintf(strSrc, 256, "%s%s", strDeviceMacId, strPackageName);
    DFREE(strDeviceMacId);
    DFREE(strPackageName);
    
    static char sUniqueDeviceId[257] = {'\0'};
    memset(sUniqueDeviceId, '\0', 257);
    toMD5(sUniqueDeviceId, strSrc);
    SAFE_DELETE_ARRAY(strSrc);
    
    return sUniqueDeviceId;
}
#endif

const char* ngDeviceAndroid::GetDeviceID() {
    return _get_device_id();
}

void ngDeviceAndroid::LaunchURL(const char* url) {
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "launchUrl", "(Ljava/lang/String;)V");
    jstring jstrUrl = stoJstring(url, pEnv);
    pEnv->CallStaticVoidMethod(cls, mid, jstrUrl);
    pEnv->DeleteLocalRef(jstrUrl);
}

#if 0
void ngDeviceAndroid::RegisterPushNotification(NGCSTR userId, NGCSTR host, int32 port, NGCSTR apiKey) {
    JNIEnv* pEnv = GetJNIEnv();
    static jclass cls = getJavaClass(jclsNGNotification);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "registerRemoteNotification", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)V");
    jstring jstrUserId = stoJstring(userId, pEnv);
    jstring jstrHost = stoJstring(host, pEnv);
    jstring jstrApiKey = stoJstring(apiKey, pEnv);
    pEnv->CallStaticVoidMethod(cls, mid, jstrUserId, jstrHost, (jint)port, jstrApiKey);
    pEnv->DeleteLocalRef(jstrUserId);
    pEnv->DeleteLocalRef(jstrHost);
    pEnv->DeleteLocalRef(jstrApiKey);
}

void ngDeviceAndroid::PushLocalNotification(int64 fireTimeFromNow, ngStringV2* msg, int32 channel, NGCSTR param) {
    NGASSERT(msg != NULL);
    
    if (param == NULL) {
        param = "";
    }

    JNIEnv* pEnv = GetJNIEnv();
    static jclass cls = getJavaClass(jclsNGNotification);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "pushLocalNotification", "(JLjava/lang/String;ILjava/lang/String;)V");
    jstring jstrMsg = stoJstring(msg->GetCString(), pEnv);
    jstring jstrParam = stoJstring(param, pEnv);
    pEnv->CallStaticVoidMethod(cls, mid, (jlong)fireTimeFromNow, jstrMsg, (jint)channel, jstrParam);
    pEnv->DeleteLocalRef(jstrMsg);
    pEnv->DeleteLocalRef(jstrParam);
}

void ngDeviceAndroid::CancelLocalNotification(int32 channel, NGCSTR param) {
    JNIEnv* pEnv = GetJNIEnv();
    static jclass cls = getJavaClass(jclsNGNotification);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "cancelLocalNotification", "(ILjava/lang/String;)V");
    jstring jstrParam = stoJstring(param, pEnv);
    pEnv->CallStaticVoidMethod(cls, mid, (jint)channel, jstrParam);
    pEnv->DeleteLocalRef(jstrParam);
}

#endif

int64 ngDeviceAndroid::GetFreeMemory() {
    JNIEnv* pEnv = GetJNIEnv();
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "getFreeMemory", "()J");
    jlong fmem = pEnv->CallStaticLongMethod(cls, mid);
    return fmem;
}

