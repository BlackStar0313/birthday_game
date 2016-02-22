//
//  ngFacebookShareCocoa.m
//  city
//
//  Created by liangliang wang on 13-7-3.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include <core/ngLogger.h>
#include <system/ngMemory.h>
#include <core/ngDevice.h>
#include "ngFacebookShareCocoa.h"

//#import "FacebookSDK.h"

#define CHEAT_VERSION_LIMIT 6.0

static ngFacebookShareCocoa* m_sFBShareCocoa = NULL;

@implementation ngFacebookCall

/**
 * A function for parsing URL parameters.
 */
- (NSDictionary*)parseURLParams:(NSString *)query {
    NSArray *pairs = [query componentsSeparatedByString:@"&"];
    NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
    for (NSString *pair in pairs) {
        NSArray *kv = [pair componentsSeparatedByString:@"="];
        NSString *val =
        [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        params[kv[0]] = val;
    }
    return params;
}

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation {
    
    BOOL urlWasHandled = [FBAppCall handleOpenURL:url
                                sourceApplication:sourceApplication
                                  fallbackHandler:^(FBAppCall *call) {
                                      NSLog(@"Unhandled deep link: %@", url);
                                  }];
    
    return urlWasHandled;
}

@end

ngFacebookShareCocoa::ngFacebookShareCocoa()
{
    m_pFacebookCall = [ngFacebookCall alloc];
    
    m_requestType = FB_REQUEST_SELFINFO;
    m_sessionType = FB_SESSION_READ;
}

ngFacebookShareCocoa::~ngFacebookShareCocoa()
{
    [ngFacebookCall release];
}

ngFacebookShare* ngFacebookShare::GetInstance()
{
    if (m_sFBShareCocoa == NULL) {
        m_sFBShareCocoa = DNEW ngFacebookShareCocoa;
    }
    return m_sFBShareCocoa;
}

void ngFacebookShareCocoa::ShareStory(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName)
{
#ifdef DEBUG_LOG
    [FBSettings setLoggingBehavior:[NSSet setWithObjects:FBLoggingBehaviorFBRequests,
                                    FBLoggingBehaviorFBURLConnections,
                                    FBLoggingBehaviorAccessTokens,
                                    FBLoggingBehaviorSessionStateTransitions,
                                    nil]];
#endif
    
    BestPractices(name,shareText, imageName);
    return;
    
    if ([[FBSession activeSession] isOpen]) {
        PublishStory();
    } else {
        [FBSession openActiveSessionWithPublishPermissions:[NSArray arrayWithObject:@"publish_actions"]
                                           defaultAudience:FBSessionDefaultAudienceEveryone
                                              allowLoginUI:YES
                                         completionHandler:^(FBSession *session, FBSessionState status, NSError *error) {
                                             if (!error && status == FBSessionStateOpen) {
                                                 PublishStory();
                                             }else{
                                                 NSLog(@"Facebook Share Error: %@", error.description);
                                             }
                                         }];
    }
    
}


void ngFacebookShareCocoa::BestPractices(ngStringV2& name, ngStringV2& shareText, ngStringV2& imageName)
{
    ngStringV2 imageTmpName;
    ngStringV2 objectType;
    ngStringV2 objectString;
    ngStringV2 actionType;
//    ngStringV2 url;
//    ngStringV2 appUrl;
    
    imageTmpName.Format("%s%s",m_FBShareData.imageHead.GetCString(),imageName.GetCString());
//    ngStringV2 imageWithSuffix = ngGameResManager::GetInstance()->GetResName(imageTmpName);
    objectType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.object.GetCString());
    actionType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.actionType.GetCString());
    objectString.AssignString(m_FBShareData.object.GetCString());
//    url.AssignString(m_FBShareData.url.GetCString());
//    appUrl.AssignString(m_FBShareData.appUrl.GetCString());
    
    NSString *pImage = [[[NSString alloc] initWithUTF8String:imageTmpName.GetCString()] autorelease];
    //    NSArray* images = @[ @{@"url": [UIImage imageNamed:pImage], @"user_generated" : @"true" } ];
//    NSString *pStr = [[[NSString alloc] initWithUTF8String:shareText.GetCString()] autorelease];
//    NSString *pObjectType = [[[NSString alloc] initWithUTF8String:objectType.GetCString()] autorelease];
    NSString *pObject = [[[NSString alloc] initWithUTF8String:objectString.GetCString()] autorelease];
    NSString *pActionType = [[[NSString alloc] initWithUTF8String:actionType.GetCString()] autorelease];
//    NSString *pUrl = [[[NSString alloc] initWithUTF8String:url.GetCString()] autorelease];
//    NSString *pAppUrl = [[[NSString alloc] initWithUTF8String:appUrl.GetCString()] autorelease];
//    NSString *pTitle = [[[NSString alloc] initWithUTF8String:name.GetCString()] autorelease];
    
    
    id<FBOpenGraphAction> action = (id<FBOpenGraphAction>)[FBGraphObject graphObject];
    [action setObject:pImage forKey:@"image"];
    
    FBOpenGraphActionParams* params = [[FBOpenGraphActionParams alloc]init];
    params.actionType = pActionType;
    params.action = action;
    params.previewPropertyName = pObject;
    
    // Show the Share dialog if available
    if([FBDialogs canPresentShareDialogWithOpenGraphActionParams:params] == TRUE) {
        NSLog(@"Facebook call presentShareDialogWithOpenGraphAction: ");
        PresentShareDialogWithOpenGraph(name, shareText, imageName);        
    }
    // If the Facebook app isn't available, show the Feed dialog as a fallback
    else {
        NSLog(@"Facebook call presentFeedDialogModallyWithSession: ");
        PresentFeedDialogModallyWithSession(name, shareText, imageName);
    }
}

void ngFacebookShareCocoa::PresentShareDialogWithOpenGraph(ngStringV2 &name, ngStringV2 &shareText, ngStringV2 &imageName)
{
    ngStringV2 imageTmpName;
    ngStringV2 objectType;
    ngStringV2 objectString;
    ngStringV2 actionType;
//    ngStringV2 url;
    ngStringV2 appUrl;
    
    imageTmpName.Format("%s%s",m_FBShareData.imageHead.GetCString(),imageName.GetCString());
//    ngStringV2 imageWithSuffix = ngGameResManager::GetInstance()->GetResName(imageTmpName);
    objectType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.object.GetCString());
    actionType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.actionType.GetCString());
    objectString.AssignString(m_FBShareData.object.GetCString());
//    url.AssignString(m_FBShareData.url.GetCString());
    appUrl.AssignString(m_FBShareData.appUrl.GetCString());
    
    NSString *pImage = [[[NSString alloc] initWithUTF8String:imageTmpName.GetCString()] autorelease];
    //    NSArray* images = @[ @{@"url": [UIImage imageNamed:pImage], @"user_generated" : @"true" } ];
    NSString *pStr = [[[NSString alloc] initWithUTF8String:shareText.GetCString()] autorelease];
    NSString *pTitle = [[[NSString alloc] initWithUTF8String:name.GetCString()] autorelease];
    NSString *pObjectType = [[[NSString alloc] initWithUTF8String:objectType.GetCString()] autorelease];
    NSString *pObject = [[[NSString alloc] initWithUTF8String:objectString.GetCString()] autorelease];
    NSString *pActionType = [[[NSString alloc] initWithUTF8String:actionType.GetCString()] autorelease];
//    NSString *pUrl = [[[NSString alloc] initWithUTF8String:url.GetCString()] autorelease];
    NSString *pAppUrl = [[[NSString alloc] initWithUTF8String:appUrl.GetCString()] autorelease];
    
    id<FBGraphObject> Object =
    [FBGraphObject openGraphObjectForPostWithType:pObjectType
                                            title:pTitle
                                            image:pImage
                                              url:pAppUrl // @"http://samples.ogp.me/479936052101366"
                                      description:pStr];
    
    id<FBOpenGraphAction> action = (id<FBOpenGraphAction>)[FBGraphObject graphObject];
    [action setObject:Object forKey:pObject];
    
    [FBDialogs presentShareDialogWithOpenGraphAction:action
                                          actionType:pActionType
                                 previewPropertyName:pObject
                                             handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                                 if (error) {
                                                     NSLog(@"Facebook Share Error: %@", error.description);
                                                     
                                                     if (m_pFBShareListener) {
                                                         m_pFBShareListener->OnShareFaild();
                                                     }
                                                 } else {
                                                     NSLog(@"Results Dictionary: %@",results);
                                                     NSLog(@"FBCall Dictionary: %@",call);
                                                     if (results[@"completionGesture"] &&
                                                         [results[@"completionGesture"] isEqualToString:@"cancel"]) {
                                                         NSLog(@"User canceled story publishing.");
                                                         
                                                         if (m_pFBShareListener) {
                                                             m_pFBShareListener->OnShareCanceled();
                                                         }
                                                         
//                                                         // If the post went through show a success message
//                                                         NSLog(@"Facebook Share Success!");
//                                                         
//                                                         if (m_pFBShareListener) {
//                                                             m_pFBShareListener->OnShareSuccess();
//                                                         }
                                                     } else {
                                                         // If the post went through show a success message
                                                         NSLog(@"Facebook Share Success!");
                                                         
                                                         if (m_pFBShareListener) {
                                                             m_pFBShareListener->OnShareSuccess();
                                                         }
                                                        
//                                                         ngStringV2 versionStr = ngDevice::GetInstance()->GetDeviceVersion();
//                                                         float version = versionStr.GetFloat();
//                                                         /*
//                                                          [FIXME]
//                                                          a cheat temp code ,because facebooksdk can't return "completionGesture" when post or cancel that deviceVersion low 6.0
//                                                          so just think it's success ,this may facebook sdk's bug.
//                                                          */
//                                                         if (version >= CHEAT_VERSION_LIMIT) {
//                                                             NSLog(@"User canceled story publishing.");
//                                                             
//                                                             if (m_pFBShareListener) {
//                                                                 m_pFBShareListener->OnShareCanceled();
//                                                             }
//                                                         } else {
//                                                             NSLog(@"Facebook Share Success!");
//                                                             
//                                                             if (m_pFBShareListener) {
//                                                                 m_pFBShareListener->OnShareSuccess();
//                                                             }
//                                                         }
                                                     }
                                                     
                                                 }
                                                 
                                             }];
}

void ngFacebookShareCocoa::PresentFeedDialogModallyWithSession(ngStringV2 &name, ngStringV2 &shareText, ngStringV2 &imageName)
{
    ngStringV2 imageTmpName;
    ngStringV2 objectType;
    ngStringV2 objectString;
    ngStringV2 actionType;
//    ngStringV2 url;
    ngStringV2 appUrl;
    
    imageTmpName.Format("%s%s",m_FBShareData.imageHead.GetCString(),imageName.GetCString());
//    ngStringV2 imageWithSuffix = ngGameResManager::GetInstance()->GetResName(imageTmpName);
    objectType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.object.GetCString());
    actionType.Format("%s:%s",m_FBShareData.nameSpace.GetCString(),m_FBShareData.actionType.GetCString());
    objectString.AssignString(m_FBShareData.customObject.GetCString());
//    url.AssignString(m_FBShareData.url.GetCString());
    appUrl.AssignString(m_FBShareData.appUrl.GetCString());
    
    NSString *pImage = [[[NSString alloc] initWithUTF8String:imageTmpName.GetCString()] autorelease];
    //    NSArray* images = @[ @{@"url": [UIImage imageNamed:pImage], @"user_generated" : @"true" } ];
    NSString *pStr = [[[NSString alloc] initWithUTF8String:shareText.GetCString()] autorelease];
    NSString *pTitle = [[[NSString alloc] initWithUTF8String:name.GetCString()] autorelease];
    //    NSString *pObjectType = [[[NSString alloc] initWithUTF8String:objectType.GetCString()] autorelease];
    NSString *pObject = [[[NSString alloc] initWithUTF8String:objectString.GetCString()] autorelease];
//    NSString *pUrl = [[[NSString alloc] initWithUTF8String:url.GetCString()] autorelease];
    NSString *pAppUrl = [[[NSString alloc] initWithUTF8String:appUrl.GetCString()] autorelease];
    
    NSDictionary* params = @{@"name": pTitle,
                             @"caption":pStr,
                             @"description": pObject,
                             @"link": pAppUrl,
                             @"picture": pImage};
    
    [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                           parameters:params
                                              handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                                                  // handle response or error
                                                  if (error) {
                                                      // Error launching the dialog or publishing a story.
                                                      NSLog(@"Facebook Share Error: %@", error.description);
                                                      if (m_pFBShareListener) {
                                                          m_pFBShareListener->OnShareFaild();
                                                      }
                                                  } else {
                                                      if (result == FBWebDialogResultDialogNotCompleted) {
                                                          // User clicked the "x" icon
                                                          NSLog(@"User canceled story publishing.");
                                                          if (m_pFBShareListener) {
                                                              m_pFBShareListener->OnShareCanceled();
                                                          }
                                                      } else {
                                                          // Handle the publish feed callback
                                                          NSDictionary *urlParams = [m_pFacebookCall parseURLParams:[resultURL query]];
                                                          if (![urlParams valueForKey:@"post_id"]) {
                                                              // User clicked the Cancel button
                                                              NSLog(@"User canceled story publishing.");
                                                              if (m_pFBShareListener) {
                                                                  m_pFBShareListener->OnShareCanceled();
                                                              }
                                                          } else {
                                                              // User clicked the Share button
                                                              NSString *msg = [NSString stringWithFormat:
                                                                               @"Posted story, id: %@",
                                                                               [urlParams valueForKey:@"post_id"]];
                                                              NSLog(@"%@", msg);
                                                              // Show the result in an alert
                                                              [[[UIAlertView alloc] initWithTitle:@"Facebook Share"
                                                                                          message:@"Shared successfully!!"
                                                                                         delegate:nil
                                                                                cancelButtonTitle:@"OK!"
                                                                                otherButtonTitles:nil]
                                                               show];
                                                              if (m_pFBShareListener) {
                                                                  m_pFBShareListener->OnShareSuccess();
                                                              }
                                                          }
                                                      }
                                                  }
                                              }];
}

void ngFacebookShareCocoa::PresentShareDialogWithLink(ngStringV2 shareText)
{
    NSString *pUrl = [[[NSString alloc] initWithUTF8String:shareText.GetCString()] autorelease];
    NSURL *urlToShare = [NSURL URLWithString:pUrl];
    
    
    // Check if the Facebook app is installed and we can present the share dialog
    FBLinkShareParams *params = [[FBLinkShareParams alloc] init];
    params.link = urlToShare;
    
    // If the Facebook app is installed and we can present the share dialog
    if ([FBDialogs canPresentShareDialogWithParams:params]) {
        
        // Present share dialog
        [FBDialogs presentShareDialogWithLink:params.link
                                      handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                          if(error) {
                                              // An error occurred, we need to handle the error
                                              // See: https://developers.facebook.com/docs/ios/errors
                                              NSLog(@"Error publishing story: %@", error.description);
                                          } else {
                                              // Success
                                              NSLog(@"result %@", results);
                                          }
                                      }];
        
        // If the Facebook app is NOT installed and we can't present the share dialog
    } else {
        // FALLBACK: publish just a link using the Feed dialog
        
        // Put together the dialog parameters
        NSMutableDictionary *params = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                       @"Shooter", @"name",
                                       @"Crazy Cowboys are insane now, shot them and protect your women & kids", @"caption",
                                       @".", @"description",
                                       @"https://blog.xrdavies.com/", @"link",
                                       @"http://city.wiyun.com/static/web/img/gamedesc.jpg", @"picture",
                                       nil];
        
        // Show the feed dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                                                      if (error) {
                                                          // An error occurred, we need to handle the error
                                                          // See: https://developers.facebook.com/docs/ios/errors
                                                          NSLog(@"Error publishing story: %@", error.description);
                                                      } else {
                                                          if (result == FBWebDialogResultDialogNotCompleted) {
                                                              // User canceled.
                                                              NSLog(@"User cancelled.");
                                                          } else {
                                                              // Handle the publish feed callback
                                                              NSDictionary *urlParams = [[ngFacebookCall alloc] parseURLParams:[resultURL query]];
                                                              
                                                              if (![urlParams valueForKey:@"post_id"]) {
                                                                  // User canceled.
                                                                  NSLog(@"User cancelled.");
                                                                  
                                                              } else {
                                                                  // User clicked the Share button
                                                                  NSString *result = [NSString stringWithFormat: @"Posted story, id: %@", [urlParams valueForKey:@"post_id"]];
                                                                  NSLog(@"result %@", result);
                                                              }
                                                          }
                                                      }
                                                  }];
    }

//    [FBDialogs presentShareDialogWithLink:urlToShare
//                                     name:@"medal obtain"
//                                  handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
//                                      if(error) {
//                                          // An error occurred, we need to handle the error
//                                          // See: https://developers.facebook.com/docs/ios/errors
//                                          NSLog(@"Error publishing story: %@", error.description);
//                                      } else {
//                                          // Success
//                                          NSLog(@"result %@", results);
//                                      }
//                                  }];
    
}


void ngFacebookShareCocoa::PresentShareWithLink(NGCSTR name, NGCSTR caption, NGCSTR desc, NGCSTR url, NGCSTR imgUrl)
{
    // Put together the dialog parameters
    NSMutableDictionary *params = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                   [[[NSString alloc] initWithUTF8String:name] autorelease] , @"name",
                                   [[[NSString alloc] initWithUTF8String:caption] autorelease], @"caption",
                                   [[[NSString alloc] initWithUTF8String:desc] autorelease], @"description",
                                   [[[NSString alloc] initWithUTF8String:url] autorelease], @"link",
                                   [[[NSString alloc] initWithUTF8String:imgUrl] autorelease], @"picture",
                                   nil];
    
    // Show the feed dialog
    [FBWebDialogs presentFeedDialogModallyWithSession:nil
                   parameters:params
                      handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                          if (error) {
                              // An error occurred, we need to handle the error
                              // See: https://developers.facebook.com/docs/ios/errors
                              NSLog(@"Error publishing story: %@", error.description);
                              
                              if (m_pFBShareListener) {
                                  m_pFBShareListener->OnShareFaild();
                              }
                              
                          } else {
                              if (result == FBWebDialogResultDialogNotCompleted) {
                                  // User canceled.
                                  NSLog(@"User cancelled.");
                                  
                                  if (m_pFBShareListener) {
                                      m_pFBShareListener->OnShareCanceled();
                                  }
                                  
                              } else {
                                  // Handle the publish feed callback
                                  NSDictionary *urlParams = [[ngFacebookCall alloc] parseURLParams:[resultURL query]];
                                  
                                  if (![urlParams valueForKey:@"post_id"]) {
                                      // User canceled.
                                      NSLog(@"User cancelled.");
                                      
                                      if (m_pFBShareListener) {
                                          m_pFBShareListener->OnShareCanceled();
                                      }
                                      
                                  } else {
                                      // User clicked the Share button
                                      NSString *result = [NSString stringWithFormat: @"Posted story, id: %@", [urlParams valueForKey:@"post_id"]];
                                      NSLog(@"result %@", result);
                                      
                                      if (m_pFBShareListener) {
                                          m_pFBShareListener->OnShareSuccess();
                                      }
                                  }
                              }
                          }
                      }];
}

void ngFacebookShareCocoa::PublishStory()
{
    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
        RequestPermissionAndPost();
    } else {
        PostOpenGraphAction();
    }
}

void ngFacebookShareCocoa::RequestPermissionAndPost()
{
    [FBSession.activeSession requestNewPublishPermissions:[NSArray arrayWithObject:@"publish_actions"]
                                          defaultAudience:FBSessionDefaultAudienceEveryone
                                        completionHandler:^(FBSession *session, NSError *error) {
                                            if (!error) {
                                                // Now have the permission
                                                PostOpenGraphAction();
                                            } else {
                                                // Facebook SDK * error handling *
                                                // if the operation is not user cancelled
                                                if (error.fberrorCategory != FBErrorCategoryUserCancelled) {
                                                    if (error.fberrorShouldNotifyUser) {
                                                        // The SDK has a message for the user, surface it.
                                                        [[[UIAlertView alloc] initWithTitle:@"Something Went Wrong"
                                                                                    message:error.fberrorUserMessage
                                                                                   delegate:nil
                                                                          cancelButtonTitle:@"OK"
                                                                          otherButtonTitles:nil] show];
                                                    } else {
                                                        NSLog(@"unexpected error:%@", error);
                                                    }
                                                }
                                            }
                                        }];
}

void ngFacebookShareCocoa::PostOpenGraphAction()
{
    FBRequestConnection *requestConnection = [[FBRequestConnection alloc] init];
    
//    ngStringV2 imageWithSuffix = ngGameResManager::GetInstance()->GetResName("dengji_1.png");
//    NSString *pImage = [[[NSString alloc] initWithUTF8String:imageWithSuffix.GetCString()] autorelease];
//    NSArray* images = @[ @{@"url": [UIImage imageNamed:pImage], @"user_generated" : @"true" } ];

    
    // First create the Open Graph meal object for the meal we ate.
//    id<SCOGMeal> mealObject = [self mealObjectForMeal:self.selectedMeal];
    
    // Now create an Open Graph eat action with the meal, our location, and the people we were with.
//    id<SCOGEatMealAction> action = (id<SCOGEatMealAction>)[FBGraphObject graphObject];
//    action.meal = mealObject;
    
    NSMutableDictionary<FBGraphObject> *object =
    [FBGraphObject openGraphObjectForPostWithType:@"wildcity:medal"
                                            title:@"Sample Medal"
                                            image:@"https://fbstatic-a.akamaihd.net/images/devsite/attachment_blank.png"
                                              url:@"http://samples.ogp.me/522233337832051"
                                      description:@""];;
    
    id<FBOpenGraphAction> action = (id<FBOpenGraphAction>)[FBGraphObject graphObject];
//    action[@"medal"] = @"http://samples.ogp.me/522233337832051";
    [action setObject:object forKey:@"medal"];
//    [action setObject:images forKey:@"image"];
    
    // Create the request and post the action to the "me/fb_sample_scrumps:eat" path.
    FBRequest *actionRequest = [FBRequest requestForPostWithGraphPath:@"me/wildcity:obtain"
                                                          graphObject:action];
    [requestConnection addRequest:actionRequest
                completionHandler:^(FBRequestConnection *connection,
                                    id result,
                                    NSError *error) {
                    
                    if (!error) {
                        [[[UIAlertView alloc] initWithTitle:@"Result"
                                                    message:[NSString stringWithFormat:@"Posted Open Graph action, id: %@",
                                                             [result objectForKey:@"id"]]
                                                   delegate:nil
                                          cancelButtonTitle:@"Thanks!"
                                          otherButtonTitles:nil]
                         show];
                    } else {
                        if (error.fberrorCategory == FBErrorCategoryPermissions) {
                            NSLog(@"Re-requesting permissions");
                            RequestPermissionAndPost();
                            return;
                        } else if (error.fberrorShouldNotifyUser) {
                            // The SDK has a message for the user, surface it.
                            [[[UIAlertView alloc] initWithTitle:@"Something Went Wrong"
                                                        message:error.fberrorUserMessage
                                                       delegate:nil
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil] show];
                        } else {
                            NSLog(@"unexpected error:%@", error);
                        }
                    }
                }];
    
    [requestConnection start];
}

bool ngFacebookShareCocoa::IsAvailable()
{
    return [[FBSession activeSession] isOpen];
}

void ngFacebookShareCocoa::Login()
{
    m_requestType = FB_REQUEST_SELFINFO;
    m_sessionType = FB_SESSION_READ;
    
    if ([[FBSession activeSession] isOpen]) {
        
    } else {
        OpenSession();
    }
}

void ngFacebookShareCocoa::LogOut()
{
    [FBSession.activeSession closeAndClearTokenInformation];
}

void ngFacebookShareCocoa::UploadPhoto(ngStringV2 imageName, ngStringV2 desc)
{
    if (desc.GetLength()<=0) {
        desc.AssignString("");
    }
    
    RequestUploadPhoto(imageName.GetCString(), desc.GetCString());
}

void ngFacebookShareCocoa::DisableAutoRotation()
{
    
}

void ngFacebookShareCocoa::OnLoginCanceled()
{
    
}

void ngFacebookShareCocoa::OnLoginFailed()
{
    if (m_pFBLoginListener) {
        m_pFBLoginListener->OnFBLoginFailed();
    }
}

void ngFacebookShareCocoa::OnLoginSuccess(NGCSTR uin, NGCSTR name)
{
    if (m_pFBLoginListener) {
        m_pFBLoginListener->OnFBLoginSuccess(uin, name);
    }
}

void ngFacebookShareCocoa::OnUploadPhotoSuccess()
{
    if (m_pFBLoginListener) {
        m_pFBLoginListener->OnFBUploadPhotoSuccess();
    }
}

void ngFacebookShareCocoa::OnUploadPhotoFailed()
{
    if (m_pFBLoginListener) {
        m_pFBLoginListener->OnFBUploadPhotoFailed();
    }
}

bool ngFacebookShareCocoa::HandleURL(NGCSTR url)
{
    return [FBSession.activeSession handleOpenURL:[NSURL URLWithString:[[NSString alloc] initWithUTF8String:url]]];
}


// for FBSession
void ngFacebookShareCocoa::OnFBSessionStateHandler(FBSession *session, FBSessionState status, NSError *error, NGCSTR param1, NGCSTR param2, NGCSTR param3)
{
    NSLog(@"session state change error: %@", error);
    switch (status) {
        case FBSessionStateOpen:
        {
            if (m_requestType == FB_REQUEST_SELFINFO && m_sessionType == FB_SESSION_READ) {
                RequestSelfInfo();
            } else if (m_requestType == FB_REQUEST_UPLOAD_PHOTO && m_sessionType == FB_SESSION_PUBLISH){
                // param1 is image name, param2 is desc
                RequestUploadPhoto(param1, param2);
            } else if (m_requestType == FB_REQUEST_POST_MSG && m_sessionType == FB_SESSION_PUBLISH) {
                
            }
            
            break;
        }
        case FBSessionStateClosed:
        case FBSessionStateClosedLoginFailed:
            OnLoginFailed();
            break;
        default:
            break;
    }
}

void ngFacebookShareCocoa::OnFBSessionRequestPermissionResultHandler(FBSession *session, NSError *error)
{
    
}

// for FBRequest
void ngFacebookShareCocoa::OnFBRequestHandler(FBRequestConnection *connection, id result, NSError *error)
{
    NSLog(@"didLoad result: %@", result);
    NSLog(@"didLoad error: %@", error);
    if (!error) {
        switch (m_requestType) {
            case FB_REQUEST_SELFINFO:
            {
                if ([result isKindOfClass:[NSArray class]] && [result count] > 0) {
                    result = [result objectAtIndex:0];
                }
                NSString* name = [result objectForKey:@"name"];
                int64 uid = [[result objectForKey:@"id"] longLongValue];
                NSString* facebookId = [NSString stringWithFormat:@"%lld", uid];
                OnLoginSuccess([facebookId UTF8String], [name UTF8String]);
                break;
            }
                
            case FB_REQUEST_UPLOAD_PHOTO:
            {
                OnUploadPhotoSuccess();
                break;
            }
                
            default:
            {
                [[[UIAlertView alloc] initWithTitle:@"Result"
                                            message:[NSString stringWithFormat:@"Posted Open Graph action, id: %@",
                                                     [result objectForKey:@"id"]]
                                           delegate:nil
                                  cancelButtonTitle:@"Thanks!"
                                  otherButtonTitles:nil]
                 show];
                break;
            }
        }
    } else {
        
        switch (m_requestType) {
            case FB_REQUEST_SELFINFO:
            {
                OnLoginFailed();
                break;
            }
                
            case FB_REQUEST_UPLOAD_PHOTO:
            {
                OnUploadPhotoFailed();
                break;
            }
            default:
                break;
        }
        if (error.fberrorCategory == FBErrorCategoryPermissions) {
            NSLog(@"Re-requesting permissions");
            RequestPermissionAndPost();
            return;
        } else if (error.fberrorShouldNotifyUser) {
            // The SDK has a message for the user, surface it.
            [[[UIAlertView alloc] initWithTitle:@"Something Went Wrong"
                                        message:error.fberrorUserMessage
                                       delegate:nil
                              cancelButtonTitle:@"OK"
                              otherButtonTitles:nil] show];
        } else {
            NSLog(@"unexpected error:%@", error);
        }
    }
    
    m_requestType = FB_REQUEST_NONE;
    m_sessionType = FB_SESSION_NONE;
}


// establish session
void ngFacebookShareCocoa::OpenSession()
{
    //    NSArray* _permissions =  [[NSArray arrayWithObjects: @"read_stream", @"offline_access", @"user_checkins", @"publish_stream", nil] retain];
    NSArray* _permissions =  [[NSArray arrayWithObjects: @"read_stream",@"user_photos", nil] retain];
    
    [FBSession openActiveSessionWithReadPermissions:_permissions
                                       allowLoginUI:YES
                                  completionHandler:
     ^(FBSession *session,
       FBSessionState state, NSError *error) {
         OnFBSessionStateHandler(session,state,error);
     }];
}

// login facebook
void ngFacebookShareCocoa::RequestSelfInfo()
{
    if ([[FBSession activeSession] isOpen]) {
        
        m_requestType = FB_REQUEST_SELFINFO;
        
        FBRequestConnection* connection = [[FBRequestConnection alloc] init];
        
        [connection addRequest:[FBRequest requestForMe] completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
            OnFBRequestHandler(connection, result, error);
        }];
        
        [connection start];
    }
}

// upload photo
void ngFacebookShareCocoa::RequestUploadPhoto(NGCSTR imageName, NGCSTR desc)
{
    //    if (![[FBSession activeSession] isOpen]) {
    //        return;
    //    }
    
    m_requestType = FB_REQUEST_UPLOAD_PHOTO;
    
    NSLog(@"permissions : %@", [[FBSession activeSession ] permissions]);
    
    //    if ([[[FBSession activeSession ] permissions] indexOfObject:@"publish_stream"] == NSNotFound) {
    if (m_sessionType != FB_SESSION_PUBLISH) {
        m_sessionType = FB_SESSION_PUBLISH;
        NSArray* _permissions =  [[NSArray arrayWithObjects: @"publish_actions", @"publish_stream", nil] retain];
        [FBSession openActiveSessionWithPublishPermissions:_permissions
                                           defaultAudience:FBSessionDefaultAudienceEveryone
                                              allowLoginUI:YES
                                         completionHandler: ^(FBSession *session, FBSessionState status, NSError *error) {
                                             OnFBSessionStateHandler(session, status, error, imageName, desc);
                                         }];
    } else {
        FBRequestConnection* connection = [[FBRequestConnection alloc] init];
        
        ngStringV2 path;
        ngDevice::GetInstance()->GetFileWritePath(path, imageName);
        NSString *nsPath = [[NSString alloc] initWithUTF8String:path.GetCString()];
        UIImage *pImage = [UIImage imageWithContentsOfFile: nsPath];
        [nsPath release];
        
        // first request to upload photo
        [connection addRequest:[FBRequest requestForUploadPhoto:pImage] completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
            //            OnFBRequestHandler(connection, result, error);
            NSLog(@"result : %@", result);
        }
                batchEntryName:@"photopost"
         ];
        
        // Second request retrieves photo information for just-created
        // photo so we can grab its source.
        //    [connection addRequest:[FBRequest
        //                            requestForGraphPath:@"{result=photopost:$.id}"]
        //         completionHandler:
        //     ^(FBRequestConnection *connection, id result, NSError *error) {
        //         if (!error &&
        //                 result) {
        //
        //                 NSString *source = [result objectForKey:@"source"];
        //    //             [self postOpenGraphActionWithPhotoURL:source];
        //             NSLog(@"result %@", result);
        //             NSLog(@"source url %@", source);
        //
        //             NSString *format =
        //             @"http://www.anansimobile.com/repeater.php?"
        //             @"fb:app_id=368751113219861&og:type=%@&"
        //             @"og:title=%@&og:description=%%22%@%%22&"
        //             @"og:image=https://s-static.ak.fbcdn.net/images/devsite/attachment_blank.png&"
        //             @"body=%@";
        //
        //             // We create an FBGraphObject object, but we can treat it as
        //             // an SCOGMeal with typed properties, etc. See <FacebookSDK/FBGraphObject.h>
        //             // for more details.
        //             NSMutableDictionary* result = [FBGraphObject graphObject];
        //
        //             // Give it a URL that will echo back the name of the meal as its title,
        //             // description, and body.
        //
        //
        //            //                     [connection addRequest:[FBRequest requestForPostOpenGraphObjectWithType:@"me/photos" title:@"test" image:source url:@"http://www.anansimobile.com" description:@"serawefasdf" objectProperties:nil] completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
        ////                         OnFBRequestHandler(connection, result, error);
        ////                     }];
        //                 }
        //
        ////                 [FBRequestConnection startForPostWithGraphPath:@"me/photos"
        ////                                                    graphObject:Object
        ////                                              completionHandler:
        ////                  ^(FBRequestConnection *connection, id result, NSError *error) {
        //////                      NSString *alertText;
        //////                      if (!error) {
        //////                          alertText = [NSString stringWithFormat:
        //////                                       @"Posted Open Graph action, id: %@",
        //////                                       [result objectForKey:@"id"]];
        //////                      } else {
        //////                          alertText = [NSString stringWithFormat:
        //////                                       @"error: domain = %@, code = %d",
        //////                                       error.domain, error.code];
        //////                      }
        //////                      [[[UIAlertView alloc] initWithTitle:@"Result"
        //////                                                  message:alertText
        //////                                                 delegate:nil
        //////                                        cancelButtonTitle:@"Thanks!"
        //////                                        otherButtonTitles:nil]
        //////                       show];
        ////                      OnFBRequestHandler(connection, result, error);
        ////                  }
        ////                  ];
        //
        //         }
        ////     }
        //     ];
        
        //        id<FBGraphObject> Object =
        //        [FBGraphObject openGraphObjectForPostWithType:@"me/photos"
        //                                                title:@"test"
        //                                                image:pImage
        //                                                  url:@"http://www.anansimobile.com"
        //                                          description:@"test"];
        //
        ////        [connection addRequest:[FBRequest requestForPostWithGraphPath:@"me/photos" graphObject:Object] completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
        ////            OnFBRequestHandler(connection, result, error);
        ////        }];
        //        
        //        Object[@"create_object"] = @"1";
        //        Object[@"fbsdk:create_object"] = @"1";
        //        
        //        [connection addRequest:[[FBRequest alloc] initForPostWithSession:[FBSession activeSession] graphPath:@"me/photos" graphObject:Object] completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
        //            OnFBRequestHandler(connection, result, error);
        //        }];
        //        
        [connection start];
        
    }
}