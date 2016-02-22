/*
 *  asWXHelper.h
 *  questions
 *
 *  Created by Chen Zhen on 13-9-13.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#ifndef __asWXHelper_h__
#define __asWXHelper_h__

#include <core/NGE_Types.h>

//#pragma mark - platform support

/* 
 
 目前支持的平台：

    iOS
    Android
    WP
 
 */

#define WX_SHARE_SUPPORTED 0

#if defined (NGE_PLATFORM_COCOA) || defined (NGE_PLATFORM_ANDROID) || defined (NGE_PLATFORM_METRO)
#   undef WX_SHARE_SUPPORTED
#   define WX_SHARE_SUPPORTED 1
#endif

//#pragma mark -

#if WX_SHARE_SUPPORTED

/*! 微信分享事件回调 */
class asWXHelperEventListener {
public:
    virtual void OnWXShareDone() = 0;
    virtual void OnWXShareCanceled() = 0;
    virtual void OnWXShareFailed(int32 errCode, NGCSTR errStr) = 0;
};

//#pragma mark - asWXHelper
/*!
 @class asWXHelper
 @abstract 微信openAPI接口类，统一封装各平台接口
 @discussion 
 */
class asWXHelper {

public:
    typedef enum __E_WX_SHARE_TYPE__ {
        shareTypeFriend = 0,
        shareTypeTimeLine,
    } asWXShareType;

public:
    
    static boolean IsWXAppInstalled();
    static boolean IsWXAppSupportAPI();
    
    /*!
     @function RegisterApp
     @param appID 微信后台获取到的appID
     @abstract 注册微信应用，必须在所有其他调用前。
     @discussion
     */
    static void RegisterApp(NGCSTR appID);
    
    /*!
     @function ShareText
     @param text 分享的文本
     @param type 分享类型，默认分享给好友
     @abstract 分享文本到微信
     @discussion 
     */
    static void ShareText(NGCSTR text, asWXShareType type = shareTypeFriend);

    static void ShareImage(void* pRGBData, int32 width, int32 height, NGCSTR title = NULL, NGCSTR desc = NULL, asWXShareType type = shareTypeFriend); //not support in wp8
    static void ShareImage(void* pData, int32 size, NGCSTR title = NULL, NGCSTR desc = NULL, asWXShareType type = shareTypeFriend);
    static void ShareImage(NGCSTR imgUrl, void* pThumbData = NULL, int32 nThumbData = 0, NGCSTR title = NULL, NGCSTR desc = NULL, asWXShareType type = shareTypeFriend);

    /*!
     @function ShareUrl
     @param title 标题
     @param desc 描述
     @param url 分享的url地址
     @param pThumbData 缩略图数据
     @param nThumbData 缩略图数据大小
     @param type 分享类型，默认分享给好友
     @abstract 分享Url到微信
     @discussion
     */
    static void ShareUrl(NGCSTR title, NGCSTR desc, NGCSTR url, void* pThumbData = NULL, int32 nThumbData = 0, asWXShareType type = shareTypeFriend);

public:

    /*! 设置回调接口，如果需要分享后接收回调，需要在分享前调用。 */
    static void SetEventListener(asWXHelperEventListener* l);
};

#endif

#endif	//__asWXHelper_h__
