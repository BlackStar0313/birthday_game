/*
 *  asWXHelperCocoa.mm
 *  questions
 *
 *  Created by Chen Zhen on 13-9-13.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#include "asWXHelper.h"

#if WX_SHARE_SUPPORTED

#import "WXApi.h"
#import "asWXHelperDelegate.h"

#include <core/NGE_Macros.h>

#pragma mark - share callback
@implementation asWXHelperDelegate

static asWXHelperDelegate* sDelegate = nil;

+(asWXHelperDelegate*) sharedInstance {
    if (sDelegate == nil) {
        sDelegate  = [[asWXHelperDelegate alloc] init];
    }

    return sDelegate;
}

asWXHelperEventListener* m_pEventListener = NULL;

-(void) setEventListener: (asWXHelperEventListener*) l {
    m_pEventListener = l;
}

-(void) onResp:(BaseResp *)resp {
    NG_DEBUG_LOG("[asWXHelper] on wx resp: %s\n", [resp.errStr UTF8String]);

    if (m_pEventListener == NULL) {
        return;
    }

    switch (resp.errCode) {
        case WXSuccess:
            m_pEventListener->OnWXShareDone();
            break;
        case WXErrCodeUserCancel:
            m_pEventListener->OnWXShareCanceled();
            break;
        case WXErrCodeCommon:
        case WXErrCodeSentFail:
        case WXErrCodeAuthDeny:
        case WXErrCodeUnsupport:
            m_pEventListener->OnWXShareFailed(resp.errCode, [resp.errStr UTF8String]);
            break;
            
        default:
            m_pEventListener->OnWXShareFailed(resp.errCode, [resp.errStr UTF8String]);
            break;
    }
}

/*! @brief 收到一个来自微信的请求，第三方应用程序处理完后调用sendResp向微信发送结果
 *
 * 收到一个来自微信的请求，异步处理完成后必须调用sendResp发送处理结果给微信。
 * 可能收到的请求有GetMessageFromWXReq、ShowMessageFromWXReq等。
 * @param req 具体请求内容，是自动释放的
 */

-(void) onReq:(BaseReq*)req
{
    if([req isKindOfClass:[GetMessageFromWXReq class]])
    {
        // 微信请求App提供内容， 需要app提供内容后使用sendRsp返回
        NSString *strTitle = [NSString stringWithFormat:@"微信请求App提供内容"];
        NSString *strMsg = @"微信请求App提供内容，App要调用sendResp:GetMessageFromWXResp返回给微信";

        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        alert.tag = 1000;
        [alert show];
        [alert release];
    }
    else if([req isKindOfClass:[ShowMessageFromWXReq class]])
    {
        ShowMessageFromWXReq* temp = (ShowMessageFromWXReq*)req;
        WXMediaMessage *msg = temp.message;

        //显示微信传过来的内容
        WXAppExtendObject *obj = msg.mediaObject;

        NSString *strTitle = [NSString stringWithFormat:@"微信请求App显示内容"];
        NSString *strMsg = [NSString stringWithFormat:@"标题：%@ \n内容：%@ \n附带信息：%@ \n缩略图:%u bytes\n\n", msg.title, msg.description, obj.extInfo, msg.thumbData.length];

        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
        [alert release];
    }
    else if([req isKindOfClass:[LaunchFromWXReq class]])
    {
        //从微信启动App
        NSString *strTitle = [NSString stringWithFormat:@"从微信启动"];
        NSString *strMsg = @"这是从微信启动的消息";

        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:strTitle message:strMsg delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
        [alert release];
    }
}

@end

void asWXHelper::SetEventListener(asWXHelperEventListener* l) {

    [[asWXHelperDelegate sharedInstance] setEventListener:l];

}

static int GetWXScene(asWXHelper::asWXShareType type) {
    switch (type) {
        case asWXHelper::shareTypeFriend:
            return WXSceneSession;
        case asWXHelper::shareTypeTimeLine:
            return WXSceneTimeline;
        default:
            return WXSceneSession;
            break;
    }
}

boolean asWXHelper::IsWXAppInstalled() {
    return (boolean)[WXApi isWXAppInstalled];
}

boolean asWXHelper::IsWXAppSupportAPI() {
    return (boolean)[WXApi isWXAppSupportApi];
}

void asWXHelper::RegisterApp(NGCSTR appID) {
    [WXApi registerApp: [NSString stringWithUTF8String: appID]];
}

void asWXHelper::ShareText(NGCSTR text, asWXShareType type) {
    
    if (text == NULL || strlen(text) <= 0) {
        NG_DEBUG_LOG("[asWXHelper] text len should be > 0\n");
        return;
    }
    
    SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init] autorelease];
    req.bText = YES;
    req.text = [NSString stringWithUTF8String: text];
    req.scene = GetWXScene(type);
    
    [WXApi sendReq:req];
}

void asWXHelper::ShareImage(void* pRGBData, int32 width, int32 height, NGCSTR title, NGCSTR desc, asWXShareType type) {

#if 1
    /* 通过rgb数据生成图像 */
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, pRGBData, width * height * 4, NULL);
    CGImageRef cgImage = CGImageCreate(width,
                                       height,
                                       8,
                                       32,
                                       width * 4,
                                       colorSpace,
                                       kCGBitmapByteOrderDefault,//kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big,
                                       provider,
                                       NULL,
                                       false,
                                       kCGRenderingIntentDefault);
    
    NSData* pImageJPGData = UIImageJPEGRepresentation([UIImage imageWithCGImage: cgImage], 1.0f);   //pImageJPGData autorelease if not nil.
    
    if (pImageJPGData == nil) {
        NG_DEBUG_LOG("[asWXHelper] generate image data failed!~\n");
        CGColorSpaceRelease(colorSpace);
        CGDataProviderRelease(provider);
        CGImageRelease(cgImage);
        
        return;
    }
    
    WXMediaMessage *message = [WXMediaMessage message];
//    NSString* iconFilePath = [[NSBundle mainBundle] pathForResource: @"Icon" ofType: @"png"];
//    [message setThumbImage:[UIImage imageWithContentsOfFile: iconFilePath]];
    [message setThumbImage: [UIImage imageWithCGImage: cgImage scale: 1.0f orientation: UIImageOrientationUp]];
    
    WXImageObject *ext = [WXImageObject object];
    ext.imageData = pImageJPGData;
    
    message.mediaObject = ext;
    
    /* 可选的title & description. */
    if (title) {
        message.title = [NSString stringWithUTF8String: title];
    }

    if (desc) {
        message.description = [NSString stringWithUTF8String: desc];
    }
    
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
    CGImageRelease(cgImage);

#else

    WXMediaMessage *message = [WXMediaMessage message];
    [message setThumbImage:[UIImage imageNamed:@"icon@2x.png"]];
    
    WXImageObject *ext = [WXImageObject object];
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"Icon" ofType:@"png"];
    ext.imageData = [NSData dataWithContentsOfFile:filePath] ;
    
    message.mediaObject = ext;
    
#endif

    SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init] autorelease];
    req.bText = NO;
    req.message = message;
    req.scene = GetWXScene(type);
    
    [WXApi sendReq:req];

}

void asWXHelper::ShareUrl(NGCSTR title, NGCSTR desc, NGCSTR url, void* pThumbData, int32 nThumbData, asWXShareType type) {

    if (url == NULL || strlen(url) <= 0) {
        NG_DEBUG_LOG("[asWXHelper] url len should be > 0\n");
        return;
    }

    WXWebpageObject *message = [[WXWebpageObject alloc] init];
    message.webpageUrl = [NSString stringWithUTF8String:url];

    WXMediaMessage* mediaMsg = [WXMediaMessage message];
    mediaMsg.mediaObject = message;
    [message release];

    mediaMsg.title = [NSString stringWithUTF8String:title];
    mediaMsg.description = [NSString stringWithUTF8String:desc];

    if (pThumbData != NULL && nThumbData > 0) {
        [mediaMsg setThumbImage: [UIImage imageWithData: [NSData dataWithBytes:pThumbData length:nThumbData]]];
    }

    SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init] autorelease];
    req.bText = NO;
    req.message = mediaMsg;
    req.scene = GetWXScene(type);

    [WXApi sendReq:req];
}

void asWXHelper::ShareImage(void* pData, int32 size, NGCSTR title, NGCSTR desc, asWXShareType type) {

    NSData* pImgData = [NSData dataWithBytes:pData length:size];

    static const float thumb_max_w = 64.0f;
    static const float thumb_max_h = 64.0f;

    UIImage* img = [UIImage imageWithData:pImgData];

    WXImageObject *obj = [WXImageObject object];
    obj.imageData = UIImagePNGRepresentation(img); //pImgData

    float scale_h = thumb_max_w / img.size.width;
    float scale_v = thumb_max_h / img.size.height;
    float scale_factor = scale_h > scale_v ? scale_h : scale_v;

    WXMediaMessage *message = [WXMediaMessage message];
    message.mediaObject = obj;

    [message setThumbImage: [UIImage imageWithCGImage: img.CGImage scale: scale_factor orientation: UIImageOrientationUp]];

    /* 可选的title & description. */
    if (title) {
        message.title = [NSString stringWithUTF8String: title];
    }

    if (desc) {
        message.description = [NSString stringWithUTF8String: desc];
    }

    SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init] autorelease];
    req.bText = NO;
    req.message = message;
    req.scene = GetWXScene(type);

    [WXApi sendReq:req];

}

void asWXHelper::ShareImage(NGCSTR imgUrl, void* pThumbData, int32 nThumbData, NGCSTR title, NGCSTR desc, asWXShareType type) {
    WXImageObject *obj = [WXImageObject object];
    obj.imageUrl = [NSString stringWithUTF8String:imgUrl];

    WXMediaMessage *message = [WXMediaMessage message];
    message.mediaObject = obj;

    if (pThumbData != NULL && nThumbData > 0) {
        //message.thumbData = [NSData dataWithBytes:pThumbData length:nThumbData];
        [message setThumbImage:[UIImage imageWithData: [NSData dataWithBytes:pThumbData length:nThumbData]]];
    }

    /* 可选的title & description. */
    if (title) {
        message.title = [NSString stringWithUTF8String: title];
    }

    if (desc) {
        message.description = [NSString stringWithUTF8String: desc];
    }

    SendMessageToWXReq* req = [[[SendMessageToWXReq alloc] init] autorelease];
    req.bText = NO;
    req.message = message;
    req.scene = GetWXScene(type);

    [WXApi sendReq:req];
}


#endif