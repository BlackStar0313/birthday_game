/*
 *  asWXHelperAndroid.cpp
 *  game_native_project_lib
 *
 *  Created by Chen Zhen on 13-9-13.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#include <extra/wechat/asWXHelper.h>

#include <platform/android/ngJNIUtils.h>
#include <nsl/ngString.h>

static jclass getJavaInterface(JNIEnv* pEnv) {
    
    static jclass jClsInterface = NULL;
    
    if (jClsInterface == NULL) {
        NG_DEBUG_LOG("[asWXHelper] get WXHelper!~\n");
        // jclass cls = pEnv->FindClass("com.anansimobile.extra.social.weixin.WXHelper");
        jclass cls = pEnv->FindClass("com.trival.flyingbird.WXHelper");
        NGASSERT(cls);
        jClsInterface = (jclass)pEnv->NewGlobalRef(cls);
        pEnv->DeleteLocalRef(cls);
    }
    return jClsInterface;
}

static asWXHelperEventListener* sEventListener = NULL;

void asWXHelper::SetEventListener(asWXHelperEventListener* l) {
    sEventListener = l;
}

boolean asWXHelper::IsWXAppInstalled() {
    JNIEnv* pEnv = GetJNIEnv();
    
    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "isWXAppInstalled", "()Z");
    return (boolean)pEnv->CallStaticBooleanMethod(cls, mid);
}

boolean asWXHelper::IsWXAppSupportAPI() {
    JNIEnv* pEnv = GetJNIEnv();
    
    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "isWXAppSupported", "()Z");
    return (boolean)pEnv->CallStaticBooleanMethod(cls, mid);
}

void asWXHelper::RegisterApp(NGCSTR appID) {
    JNIEnv* pEnv = GetJNIEnv();
    
    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "registerWX", "(Ljava/lang/String;)V");
    
    jstring jstrAppID = stoJstring(appID);

    pEnv->CallStaticVoidMethod(cls, mid, jstrAppID);
    
    pEnv->DeleteLocalRef(jstrAppID);
}

void asWXHelper::ShareText(NGCSTR text, asWXShareType type) {
    JNIEnv* pEnv = GetJNIEnv();
    
    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "shareText", "(Ljava/lang/String;I)V");
    
    jstring jstrText = stoJstring(text);
    
    pEnv->CallStaticVoidMethod(cls, mid, jstrText, jint(type));
    
    pEnv->DeleteLocalRef(jstrText);
}

void asWXHelper::ShareImage(void* pRGBData, int32 width, int32 height, NGCSTR title, NGCSTR desc, asWXShareType type) {
    JNIEnv* pEnv = GetJNIEnv();

    //switch rgba order for java
    uint32* originRGB = (uint32*)pRGBData;
    for (int32 i = 0; i < width * height; i++) {
        uint32& pixel = originRGB[i];
        pixel = (pixel & 0xFF00FF00) | ((pixel & 0x00FF0000) >> 16) | ((pixel & 0x000000FF) << 16);
    }
    
    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "shareImage", "([IIILjava/lang/String;Ljava/lang/String;I)V");
    
    jstring jstrTitle = stoJstring(title);
    jstring jstrDesc = stoJstring(desc);
    
    jint* rgb = (jint*) pRGBData;
    jintArray jarray = pEnv->NewIntArray(width * height);
    pEnv->SetIntArrayRegion(jarray, 0, width * height, rgb);
    
    pEnv->CallStaticVoidMethod(cls, mid, jarray, jint(width), jint(height), jstrTitle, jstrDesc, jint(type));
    
    pEnv->DeleteLocalRef(jarray);
    pEnv->DeleteLocalRef(jstrTitle);
    pEnv->DeleteLocalRef(jstrDesc);
}

void asWXHelper::ShareImage(void* pData, int32 size, NGCSTR title, NGCSTR desc, asWXShareType type) {

    JNIEnv* pEnv = GetJNIEnv();

    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "shareImage", "(Ljava/lang/String;Ljava/lang/String;[BI)V");

    jstring jstrTitle = stoJstring(title);
    jstring jstrDesc = stoJstring(desc);

    jbyteArray jarray = NULL;
    if (pData != NULL && size > 0) {
        NG_DEBUG_LOG("[asWXHelper] thumb data size: %d", size);
        jbyte* data = (jbyte*) pData;
        jarray = pEnv->NewByteArray(size);
        pEnv->SetByteArrayRegion(jarray, 0, size, data);
    }

    pEnv->CallStaticVoidMethod(cls, mid, jarray, jstrTitle, jstrDesc, jint(type));

    if (jarray != NULL) {
        pEnv->DeleteLocalRef(jarray);
    }

    pEnv->DeleteLocalRef(jstrTitle);
    pEnv->DeleteLocalRef(jstrDesc);
    
}

void asWXHelper::ShareImage(NGCSTR imgUrl, void* pThumbData, int32 nThumbData, NGCSTR title, NGCSTR desc, asWXShareType type) {

    JNIEnv* pEnv = GetJNIEnv();

    jclass cls = getJavaInterface(pEnv);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "shareImage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[BI)V");

    jstring jstrTitle = stoJstring(title);
    jstring jstrDesc = stoJstring(desc);
    jstring jstrImgUrl = stoJstring(imgUrl);

    jbyteArray jarray = NULL;
    if (pThumbData != NULL && nThumbData > 0) {
        NG_DEBUG_LOG("[asWXHelper] thumb data size: %d", nThumbData);
        jbyte* data = (jbyte*) pThumbData;
        jarray = pEnv->NewByteArray(nThumbData);
        pEnv->SetByteArrayRegion(jarray, 0, nThumbData, data);
    }

    pEnv->CallStaticVoidMethod(cls, mid, jstrImgUrl, jstrTitle, jstrDesc, jarray, jint(type));

    if (jarray != NULL) {
        pEnv->DeleteLocalRef(jarray);
    }

    pEnv->DeleteLocalRef(jstrTitle);
    pEnv->DeleteLocalRef(jstrDesc);
    pEnv->DeleteLocalRef(jstrImgUrl);

}

void asWXHelper::ShareUrl(NGCSTR title, NGCSTR desc, NGCSTR url, void* pThumbData, int32 nThumbData, asWXShareType type) {

    if (url == NULL || strlen(url) <= 0) {
        NG_DEBUG_LOG("[asWXHelper] url len should be > 0\n");
        return;
    }

    JNIEnv* pEnv = GetJNIEnv();

    jclass cls = getJavaInterface(pEnv);
    
    jmethodID mid = pEnv->GetStaticMethodID(cls, "shareUrl", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[BI)V");
    

    jstring jstrTitle = stoJstring(title);
    jstring jstrDesc = stoJstring(desc);
    jstring jstrUrl = stoJstring(url);

    jbyteArray jarray = NULL;
    if (pThumbData != NULL && nThumbData > 0) {
        NG_DEBUG_LOG("[asWXHelper] thumb data size: %d", nThumbData);
        jbyte* data = (jbyte*) pThumbData;
        jarray = pEnv->NewByteArray(nThumbData);
        pEnv->SetByteArrayRegion(jarray, 0, nThumbData, data);
    }

    pEnv->CallStaticVoidMethod(cls, mid, jstrTitle, jstrDesc, jstrUrl, jarray, jint(type));
    if (jarray != NULL) {
        pEnv->DeleteLocalRef(jarray);
    }

    pEnv->DeleteLocalRef(jstrTitle);
    pEnv->DeleteLocalRef(jstrDesc);
    pEnv->DeleteLocalRef(jstrUrl);
}

#pragma mark - jni interface

#include "ngThreadMessageHandler.h"

class asWXHelperRespDoneMessage : public ngThreadMessage {
public:
    virtual void HandleMessageData() {
        if (sEventListener) {
            sEventListener->OnWXShareDone();
        }
    }
};

class asWXHelperRespCancelMessage : public ngThreadMessage {
public:
    virtual void HandleMessageData() {
        if (sEventListener) {
            sEventListener->OnWXShareCanceled();
        }
    }
};

class asWXHelperRespFailedMessage : public ngThreadMessage {
public:
    asWXHelperRespFailedMessage(int32 errCode, NGCSTR errStr) {
        m_errCode = errCode;
        m_errStr = errStr;
    }
public:
    virtual void HandleMessageData() {
        if (sEventListener) {
            sEventListener->OnWXShareFailed(m_errCode, m_errStr.GetCString());
        }
    }

private:
    int32 m_errCode;
    ngStringV2 m_errStr;
};

#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT void JNICALL Java_com_anansimobile_extra_social_weixin_WXHelper_OnSendMsgToWXDone
    (JNIEnv *, jclass) {
        ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(asWXHelperRespDoneMessage));
    }

    JNIEXPORT void JNICALL Java_com_anansimobile_extra_social_weixin_WXHelper_OnSendMsgToWXCanceled
    (JNIEnv *, jclass) {
        ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(asWXHelperRespCancelMessage));
    }

    JNIEXPORT void JNICALL Java_com_anansimobile_extra_social_weixin_WXHelper_OnSendMsgToWXFailed
    (JNIEnv * env, jclass, jint jerrCode, jstring jerrStr) {
        char* errStr = jstringTostring(jerrStr, env);
        ngThreadMessageHandler::GetInstance()->QueueMessage(DNEW(asWXHelperRespFailedMessage)(jerrCode, errStr));
        SAFE_FREE(errStr);
    }
    
#ifdef __cplusplus
}
#endif

