//
//  base64.h
//  islands
//
//  Created by Yuhui Wang on 1/15/12.
//  Copyright (c) 2012 Anansi. All rights reserved.
//

#ifndef __BASE64_H__
#define __BASE64_H__

#include "../core/NGE_Types.h"

#ifdef __cplusplus
extern "C" 
{
#endif

void *NewBase64Decode(
                      const char *inputBuffer,
                      size_t length,
                      size_t *outputLength);

char *NewBase64Encode(
                      const void *inputBuffer,
                      size_t length,
                      boolean separateLines,
                      size_t *outputLength);


#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif //__BASE64_H__
