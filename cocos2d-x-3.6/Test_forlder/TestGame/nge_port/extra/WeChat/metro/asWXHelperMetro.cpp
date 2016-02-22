/*
 *  asWXHelperMetro.cpp
 *  jianghu
 *
 *  Created by Chen Zhen on 13-12-09.
 *  Copyright (c) 2013年 Anansi Mobile. All rights reserved.
 */

#include "../asWXHelper.h"
#include "./asWXHelperMetroImpl.h"
#include "UtilsMetro.h"
#include <cocos2d.h>
#include "ngFileReader.h"

USING_NS_CC;

//#pragma mark - asWXHelper

using namespace PhoneDirect3DXamlAppComponent::Extra;

void asWXHelper::SetEventListener(asWXHelperEventListener* l) {
    asWXHelperMetroImplRef::GetInstance()->SetEventListener(l);
}

boolean asWXHelper::IsWXAppInstalled() {
    return TRUE;
}

boolean asWXHelper::IsWXAppSupportAPI() {
    return TRUE;
}

void asWXHelper::RegisterApp(NGCSTR appID) {
    Platform::String^ strAppID = ref new Platform::String(CCUtf8ToUnicode(appID).data());
    asWXHelperMetroImplRef::GetInstance()->RegisterApp(strAppID);
}

void asWXHelper::ShareText(NGCSTR text, asWXShareType type) {
    
    if (text == NULL || strlen(text) <= 0) {
        NG_DEBUG_LOG("[asWXHelper] text len should be > 0\n");
        return;
    }
    
    Platform::String^ strTitle = ref new Platform::String(CCUtf8ToUnicode("").data());
    Platform::String^ strContent = ref new Platform::String(CCUtf8ToUnicode(text).data());
    asWXHelperMetroImplRef::GetInstance()->ShareText(strTitle, strContent, type);
}

void asWXHelper::ShareUrl(NGCSTR title, NGCSTR desc, NGCSTR url, void* pThumbData, int32 nThumbData, asWXShareType type) {

    if (url == NULL || strlen(url) <= 0) {
        NG_DEBUG_LOG("[asWXHelper] url len should be > 0\n");
        return;
    }

    Platform::String^ strTitle = ref new Platform::String(CCUtf8ToUnicode(title).data());
    Platform::String^ strDesc = ref new Platform::String(CCUtf8ToUnicode(desc).data());
    Platform::String^ strUrl = ref new Platform::String(CCUtf8ToUnicode(url).data());

    Platform::Array<unsigned char>^ thumbData = nullptr;
    if (pThumbData != NULL && nThumbData > 0) {
        thumbData = ref new Platform::Array<unsigned char>(nThumbData);
        memcpy(thumbData->Data, pThumbData, nThumbData);
    }

    asWXHelperMetroImplRef::GetInstance()->ShareUrl(strTitle, strDesc, strUrl, thumbData, type);

}

void asWXHelper::ShareImage(void* pRGBData, int32 width, int32 height, NGCSTR title, NGCSTR desc, asWXShareType type) {

    static const char* image_file_name = "__wx_share_temp_file_from_rgb.png";

    CCImage image;
   // image.initWithImageData(pRGBData, 0, CCImage::kFmtRawData, width, height);
	image.initWithImageData((const unsigned char*)pRGBData, 0);
    image.saveToFile(image_file_name);
    ngFileReader fr;
    if (fr.Load(image_file_name)) {
        ShareImage(fr.GetData(), fr.GetLength(), title, desc, type);
    } else {
        //ignore if load faied.
        NG_DEBUG_LOG("[asWXHelper] share image failed!~\n");
    }
}

void asWXHelper::ShareImage(void* pData, int32 size, NGCSTR title, NGCSTR desc, asWXShareType type) {

    static const float thumb_max_w = 64.0f;
    static const float thumb_max_h = 64.0f;
    static const char* thumb_file_name = "__wx_share_temp_file.png";

    Platform::Array<unsigned char>^ data = ref new Platform::Array<unsigned char>(size);
    memcpy(data->Data, pData, size);

    Platform::Array<unsigned char>^ thumbData = nullptr;

    CCImage image;
	image.initWithImageData((const unsigned char *)pData, size);
    float scale_h = thumb_max_w / image.getWidth();
    float scale_v = thumb_max_h / image.getHeight();
    float scale_factor = scale_h > scale_v ? scale_h : scale_v;

    if (scale_factor < 1.0f) {
        /* 生成缩略图 */
	    //image.ScaleBicubic(scale_factor , scale_factor);
        image.saveToFile(thumb_file_name);
        ngFileReader fr;
        if (fr.Load(thumb_file_name)) {
            thumbData = ref new Platform::Array<unsigned char>(fr.GetLength());
            memcpy(thumbData->Data, fr.GetData(), fr.GetLength());
        }
#if 0
        /* FIXME: 这里需要文件数据而不是rgb数据，稍后处理。 */
        thumbData = data;
#endif
    } else {
        /* 小图(128 x 128以下的，使用原图)。 */
        thumbData = data;
    }

    Platform::String^ strTitle = ref new Platform::String(CCUtf8ToUnicode(title).data());
    Platform::String^ strDesc = ref new Platform::String(CCUtf8ToUnicode(desc).data());


    asWXHelperMetroImplRef::GetInstance()->ShareImage(data, thumbData, strTitle, strDesc, type);
}

void asWXHelper::ShareImage(NGCSTR imgUrl, void* pThumbData, int32 nThumbData, NGCSTR title, NGCSTR desc, asWXShareType type) {

    Platform::Array<unsigned char>^ thumbData = nullptr;
    if (pThumbData != NULL && nThumbData > 0) {
        thumbData = ref new Platform::Array<unsigned char>(nThumbData);
        memcpy(thumbData->Data, pThumbData, nThumbData);
    }

    Platform::String^ strUrl = ref new Platform::String(CCUtf8ToUnicode(imgUrl).data());
    Platform::String^ strTitle = ref new Platform::String(CCUtf8ToUnicode(title).data());
    Platform::String^ strDesc = ref new Platform::String(CCUtf8ToUnicode(desc).data());

    asWXHelperMetroImplRef::GetInstance()->ShareImage(strUrl, thumbData, strTitle, strDesc, type);

}

