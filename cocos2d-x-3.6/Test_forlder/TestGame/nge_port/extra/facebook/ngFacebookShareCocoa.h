//
//  ngFacebookShareCocoa.h
//  city
//
//  Created by liangliang wang on 13-7-3.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef _NGFACEBOOKSHARECOCOA_H_
#define _NGFACEBOOKSHARECOCOA_H_

#include "ngFacebookShare.h"

#import <FacebookSDK/FacebookSDK.h>

enum FBRequestType {
    FB_REQUEST_NONE = 0,
    FB_REQUEST_SELFINFO = 1,
    FB_REQUEST_UPLOAD_PHOTO = 2,
    FB_REQUEST_POST_MSG = 3,
};

enum FBSessionType {
    FB_SESSION_NONE = 0,
    FB_SESSION_READ = 1,
    FB_SESSION_PUBLISH = 2,
};

@interface ngFacebookCall :FBDialogs {
    
}

@end

class ngFacebookShareCocoa : public ngFacebookShare
{
    
public:
    ngFacebookShareCocoa();
    virtual ~ngFacebookShareCocoa();
    
public:
    virtual bool IsAvailable();
    virtual void Login();
    virtual void LogOut();
    virtual void UploadPhoto(ngStringV2 imageName, ngStringV2 desc);
    virtual void ShareStory(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName);
    
    virtual void DisableAutoRotation();
    
    virtual void OnLoginSuccess(NGCSTR uin, NGCSTR name);
    virtual void OnLoginFailed();
    virtual void OnLoginCanceled();
    
    virtual void OnUploadPhotoSuccess();
    virtual void OnUploadPhotoFailed();
    
    virtual bool HandleURL(NGCSTR url);
    
    // for FBSession
    void OnFBSessionStateHandler(FBSession *session, FBSessionState status, NSError *error, NGCSTR param1 = NULL, NGCSTR param2 = NULL, NGCSTR param3 = NULL);
    void OnFBSessionRequestPermissionResultHandler(FBSession *session,
                                                   NSError *error);
    
    // for FBRequest
    void OnFBRequestHandler(FBRequestConnection *connection,
                            id result,
                            NSError *error);
    

    virtual void PresentShareWithLink(NGCSTR name, NGCSTR caption, NGCSTR desc, NGCSTR url, NGCSTR imgUrl);
    virtual void PresentShareDialogWithLink(ngStringV2 shareText);
    
private:
    
    void PublishStory();
    void RequestPermissionAndPost();
    void PostOpenGraphAction();
    
    void BestPractices(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName);
    void PresentShareDialogWithOpenGraph(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName);
    void PresentFeedDialogModallyWithSession(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName);
    
    void OpenSession();
    void RequestSelfInfo();
    void RequestUploadPhoto(NGCSTR imageName, NGCSTR desc);
    
private:
    static ngFacebookShareCocoa* m_sFBShareCocoa;
    ngFacebookCall* m_pFacebookCall;
    
    FBRequestType m_requestType;
    FBSessionType m_sessionType;
    
};

#endif
