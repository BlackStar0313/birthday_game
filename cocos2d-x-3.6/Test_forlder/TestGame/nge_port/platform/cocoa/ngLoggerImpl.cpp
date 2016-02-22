/*
 *  ngLoggerImpl.h
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include "./ngLoggerCocoa.h"
#include <core/ngLogger.h>

void ngLogger::Log(NGCSTR format, ...)
{
#ifdef DEBUG_LOG
	va_list ap;	
	va_start( ap, format );
	ngLoggerCocoa::Log( format, ap );
	va_end( ap ); 
#endif
}

void ngLogger::EnhancedLog( NGCSTR filename, NGCSTR funcname,int linenum, NGCSTR format, ...)
{
#ifdef DEBUG_LOG
	va_list ap;	
	va_start( ap, format );
	ngLoggerCocoa::EnhancedLog(filename, funcname, linenum, format, ap);
	va_end( ap ); 
#endif
}

