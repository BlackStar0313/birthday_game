/*
 *  ngLogger.h
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGLOGGER_H__
#define __NGLOGGER_H__

class ngLogger {
public:
	static void Log(const char* format, ...);
    static void EnhancedLog(const char* filename, const char* funcname, int linenum, const char* format, ...);
};

#endif //__NGLOGGER_H__land