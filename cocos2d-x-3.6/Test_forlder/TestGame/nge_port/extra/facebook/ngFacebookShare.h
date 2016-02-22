//
//  ngFacebookShare.h
//  city
//
//  Created by liangliang wang on 13-7-3.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef _NGFACEBOOKSHARE_H_
#define _NGFACEBOOKSHARE_H_

#include <nsl/ngStringV2.h>

/*
 * FBShareData_t facebook share story need review action and object for generate the story,so facebook develop center will provide a url to point the object
 * title and message we can custom it，imageURL need a valid url that facebook can access link
 *
 * example:
 *    imageHead:https://s3-ap-northeast-1.amazonaws.com/city-achievement/
 *    url:http://samples.ogp.me/479936052101366
 *    actionType:obtain
 *    object:medal
 *    nameSpace:wildcityplace
 *
 * [liangliang.wang]
 */
typedef struct facebookShare_t{
    ngStringV2 imageHead;
    ngStringV2 url;
    ngStringV2 actionType;
    ngStringV2 object;
    ngStringV2 nameSpace;
    ngStringV2 appUrl;
    ngStringV2 customObject;
}FBShareData_t;

class ngFacebookShareListener {
    
public:
    virtual void OnShareSuccess() = 0;
    virtual void OnShareFaild() = 0;
    virtual void OnShareCanceled() = 0;
};

class ngFacebookShareLoginListener {
    
public:
    virtual void OnFBLoginSuccess(NGCSTR uin,NGCSTR name) = 0;
    virtual void OnFBLoginFailed() = 0;
    virtual void OnFBUploadPhotoSuccess() = 0;
    virtual void OnFBUploadPhotoFailed() = 0;
};

class ngFacebookShare
{
public:
    virtual ~ngFacebookShare();
    
    static ngFacebookShare* GetInstance();
    
public:
    virtual bool IsAvailable() = 0;
    virtual void Login() = 0;
    virtual void LogOut() = 0;
    virtual void ShareStory(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName) = 0;
    virtual void UploadPhoto(ngStringV2 imageName, ngStringV2 desc) = 0;
    virtual void PresentShareDialogWithLink(ngStringV2 shareText) = 0;
    
    virtual void PresentShareWithLink(NGCSTR name, NGCSTR caption, NGCSTR desc, NGCSTR url, NGCSTR imgUrl) = 0;
    
    virtual bool HandleURL(NGCSTR url) = 0;
    
    virtual void DisableAutoRotation() = 0;
    
    virtual void OnLoginSuccess(NGCSTR uin, NGCSTR name) = 0;
    virtual void OnLoginFailed() = 0;
    virtual void OnLoginCanceled() = 0;
    
    void         SetFBShareData(FBShareData_t& data);
    void         SetFBShareListener(ngFacebookShareListener* pListener) { m_pFBShareListener = pListener; }
    void         SetFBShareLoginListener(ngFacebookShareLoginListener* pListener) { m_pFBLoginListener = pListener; }
    ngFacebookShareListener* GetFBShareListener() { return m_pFBShareListener; }
    ngFacebookShareLoginListener* GEtFBShareLoginListener() { return m_pFBLoginListener; }
    
protected:
    ngFacebookShare();
    
protected:
    FBShareData_t m_FBShareData;
    ngFacebookShareListener* m_pFBShareListener;
    ngFacebookShareLoginListener* m_pFBLoginListener;
};

#endif
