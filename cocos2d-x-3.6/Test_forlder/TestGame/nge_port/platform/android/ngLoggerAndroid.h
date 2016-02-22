/*
 *  ngLoggerAndroid.h
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __NGLOGGERANDROID_H__
#define __NGLOGGERANDROID_H__

#include <stdio.h>
#include <core/NGE_Types.h>

class ngLoggerAndroid 
{
public:
    static void Log(NGCSTR format, va_list args);
};


#endif //__NGLOGGERANDROID_H__
