//
//  HMACSHA1.h
//  pet_native_project_lib
//
//  Created by chen on 12-10-30.
//  Copyright (c) 2012年 Anaisi. All rights reserved.
//

#ifndef __HMACSHA1_H__
#define __HMACSHA1_H__

/* this is a test file. */

#if defined (NGE_PLATFORM_ANDROID)
#include <sha1.h>
#elif defined (NGE_PLATFORM_COCOA)
#include <CommonCrypto/CommonCrypto.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 @function HMAC_SHA1
 @abstract .
 @discussion 
 */
void HMAC_SHA1(const u_char* d, int ld, const u_char* k, int lk, char* ret);
    
#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif  //__HMACSHA1_H__
