//
//  ngStringHelper.cpp
//  
//
//  Created by chen on 12-10-16.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifdef NGE_PLATFORM_ANDROID

#include "ngStringHelper.h"
#include <core/NGE_NSL.h>

#include <android/ngJNIUtils.h>

ngStringV2* ngStringHelper::encryptXOR(NGCSTR pStr, NGCSTR pKey) {
    
    ngStringV2 data(pStr);
    
    uint8* dataPtr          = data.GetBuffer();

    const uint8* keyData    = (const uint8*) pKey;

    const uint8* keyPtr = keyData;
    int32 keyIndex      = 0;
    
    int32 dataLen   = data.GetLength();
    int32 keyLen    = strlen(pKey);
    
    for (int32 x = 0; x < dataLen; x++) {

		*dataPtr = *dataPtr ^ *keyPtr;
        dataPtr ++;
        keyPtr ++;

        if (++keyIndex == keyLen) {
			keyIndex = 0, keyPtr = keyData;
		}
    }
    
    size_t len = 0;
    char* newStr = NewBase64Encode(data.GetBuffer(), data.GetLength(), FALSE, &len);
    ngStringV2* pRetStr = DNEW(ngStringV2)(newStr);
    SAFE_FREE(newStr);
    
    return pRetStr;
}

ngStringV2* ngStringHelper::decryptXOR(NGCSTR pStr, NGCSTR pKey) {
    
	size_t len = 0;
	uint8* data = (uint8*)NewBase64Decode(pStr, strlen(pStr), &len);
    
    uint8* dataPtr          = data;

    const uint8* keyData    = (const uint8*) pKey;

    const uint8* keyPtr = keyData;
    int32 keyIndex      = 0;
    
    int32 keyLen = strlen(pKey);
    
    for (int32 x = 0; x < len; x++) {

		*dataPtr = *dataPtr ^ *keyPtr;
        dataPtr ++;
        keyPtr ++;

        if (++keyIndex == keyLen) {
			keyIndex = 0, keyPtr = keyData;
		}
    }
    
    ngStringV2* pRetStr = DNEW(ngStringV2)((const char*)data, len);
    SAFE_FREE(data);
    
    return pRetStr;
}

static const char* word_start = ",./\\[]{}|<>'\";: !@#$%^&*()-+=";
static const char* word_end = ",./\\[]{}|<>'\";: !@#$%^&*()-+=\n";

static NGCSTR string_word_start_flag() {
    return word_start;
}

static NGCSTR string_word_end_flag() {
    return word_end;
}

void ngStringHelper::BreakText(NGCSTR text,
                               ngSystemFont* pFont,
                               int32 maxWidth,
                               void* pHandler,
                               btCallbackAddLine cbAddLine,
                               btCallbackUpdateLineHeight cbUpdateLineHeight,
                               btCallbackWordStartFlag cbWordStartFlag,
                               btCallbackWordEndFlag cbWordEndFlag) {
#if 0 //port to cocos2dx, temp disable.
    NGASSERT(pFont != NULL);

    BreakTextContext btContext;
    btContext.m_cbAddLine           = cbAddLine;
    btContext.m_cbUpdateLineHeight  = cbUpdateLineHeight;
    btContext.m_cbWordStartFlag     = (cbWordStartFlag != NULL) ? cbWordStartFlag : string_word_start_flag;
    btContext.m_cbWordEndFlag       = (cbWordEndFlag != NULL) ? cbWordEndFlag : string_word_end_flag;
    btContext.m_pHandler = pHandler;
    
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass clsHelper     = getJavaClass(jclsNGStringHelper);
    jmethodID mid               = pEnv->GetStaticMethodID(clsHelper,
                                                          "breakText",
                                                          "(IILjava/lang/String;Ljava/lang/String;I)V");
    
    jstring jstrFontName = stoJstring(pFont->GetName());
    jstring jstrText = stoJstring(text);
    pEnv->CallStaticVoidMethod(clsHelper, mid,
                               &btContext,
                               maxWidth,
                               jstrText,
                               jstrFontName,
                               pFont->GetSize());

    pEnv->DeleteLocalRef(jstrText);
    pEnv->DeleteLocalRef(jstrFontName);
#endif
}

#endif //NGE_PLATFORM_ANDROID
