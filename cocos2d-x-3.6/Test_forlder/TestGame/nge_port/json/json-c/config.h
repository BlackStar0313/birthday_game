/*
 * $Id: config.h.win32,v 1.2 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#ifndef __JSONCONFIG_H__
#define __JSONCONFIG_H__

#if (defined NGE_PLATFORM_WIN32 | defined NGE_PLATFORM_METRO)
#include "configWin32.h"
#elif (defined NGE_PLATFORM_COCOA)
#include "configCocoa.h"
#elif (defined NGE_PLATFORM_ANDROID)
#include "configAndroid.h"
#else
#error "config for this platform"
#endif


#endif //__JSONCONFIG_H__
