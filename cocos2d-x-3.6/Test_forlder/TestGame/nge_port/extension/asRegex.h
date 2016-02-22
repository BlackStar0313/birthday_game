//
//  asRegex.h
//  jianghu
//
//  Created by Xie Rui on 10/29/13.
//  Copyright (c) 2013 Anansi. All rights reserved.
//

#ifndef __jianghu__asRegex__
#define __jianghu__asRegex__

#if defined( NGE_PLATFORM_METRO )

#else

#define USE_POSIX_PCRE 1

#include <lib/pcre/pcre.h>
#if USE_POSIX_PCRE
#include <lib/pcre/pcreposix.h>
#endif

#include <nsl/ngLinkedList.h>
/*
 asRegex use pcre as regular expression in our engine.
 There are two ways to use pcre
 1. use native API
 2. use posix API
 */

namespace asRegex {
#if USE_POSIX_PCRE
    ngLinkedList* grepText(NGCSTR pattern, NGCSTR text);
#endif
    
    int test();
}

#endif // NGE_PLATFORM_METRO

#endif /* defined(__jianghu__asRegex__) */
