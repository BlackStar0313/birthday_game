//
//  ngStringHelper.h
//  
//
//  Created by chen on 12-10-16.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifdef NGE_PLATFORM_ANDROID

#ifndef __NGSTRINGHELPER_H__
#define __NGSTRINGHELPER_H__

#include <nsl/ngString.h>

//here XORKEY is copy from NSString+XOR.h
#define XORKEY "One ring to rule them all, one ring to find them, one ring to bring them all and in the darkness bind them."

class ngSystemFont;

typedef void (*btCallbackAddLine) (void*, NGCSTR);
typedef void (*btCallbackUpdateLineHeight) (void*, int32);
typedef NGCSTR (*btCallbackWordStartFlag) ();
typedef NGCSTR (*btCallbackWordEndFlag) ();

class ngStringHelper {
public:
    static ngStringV2* encryptXOR(NGCSTR pStr, NGCSTR pKey);
    static ngStringV2* decryptXOR(NGCSTR pStr, NGCSTR pKey);
    
    static void BreakText(NGCSTR text,
                          ngSystemFont* pFont,
                          int32 maxWidth,
                          void* pHandler,
                          btCallbackAddLine cbAddLine,
                          btCallbackUpdateLineHeight cbUpdateLineHeight,
                          btCallbackWordStartFlag cbWordStartFlag = NULL,
                          btCallbackWordEndFlag cbWordEndFlag = NULL);
};

typedef struct __T_BREAK_TEXT_CONTEXT__ {
    void*                       m_pHandler;
    btCallbackAddLine           m_cbAddLine;
    btCallbackUpdateLineHeight  m_cbUpdateLineHeight;
    btCallbackWordStartFlag     m_cbWordStartFlag;
    btCallbackWordEndFlag       m_cbWordEndFlag;
} BreakTextContext;

#endif  //__NGSTRINGHELPER_H__


#endif //NGE_PLATFORM_ANDROID

