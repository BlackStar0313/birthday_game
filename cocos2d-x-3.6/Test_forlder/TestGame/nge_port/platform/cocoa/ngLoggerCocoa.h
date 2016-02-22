/*
 *  ngLoggerCocoa.h
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGLOGGERCOCOA_H__
#define __NGLOGGERCOCOA_H__

#include <stdio.h>
#include <core/NGE_Types.h>

class ngLoggerCocoa 
{
public:
	static void Log(NGCSTR format, va_list args);
    static void EnhancedLog(NGCSTR filename, NGCSTR funcname, int linenum, NGCSTR format, va_list args);
};


#endif //__NGLOGGERCOCOA_H__