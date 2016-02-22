/*
 *  asWXHelperDelegate.h
 *  questions
 *
 *  Created by Chen Zhen on 13-12-12.
 *  Copyright (c) 2013年 Anansi. All rights reserved.
 */

#ifndef __asWXHelperDelegate_h__
#define __asWXHelperDelegate_h__

#import "WXApi.h"

#pragma mark - share callback
@interface asWXHelperDelegate : NSObject<WXApiDelegate>

-(void) onReq:(BaseReq*)req;
-(void) onResp:(BaseResp*)resp;

+(asWXHelperDelegate*) sharedInstance;

@end

#endif	//__asWXHelperDelegate_h__
