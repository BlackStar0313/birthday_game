/*
 *  ngLoggerImpl.h
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "ngLoggerAndroid.h"
#include <core/ngLogger.h>

void ngLogger::Log(NGCSTR format, ...)
{
    va_list ap;
    va_start( ap, format );
    ngLoggerAndroid::Log( format, ap );
    va_end( ap );
}

void ngLogger::EnhancedLog(NGCSTR filename, NGCSTR funcname, int linenum, NGCSTR format, ...) {
    va_list ap;
    va_start( ap, format );
    ngLoggerAndroid::Log( format, ap );
    va_end( ap );
}
