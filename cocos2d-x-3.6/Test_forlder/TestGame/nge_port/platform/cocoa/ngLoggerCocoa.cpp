/*
 *  ngLoggerCocoa.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#include "./ngLoggerCocoa.h"
#include <core/NGE_Macros.h>

#define BUFFER_SIZE 40960

void ngLoggerCocoa::Log(NGCSTR format, va_list args) {
	char buffer[BUFFER_SIZE] = {0, };
	int32 cnt = vsnprintf(buffer, BUFFER_SIZE, format, args);
    if (cnt > 0 && cnt < BUFFER_SIZE - 1) { //available buffer
        if (buffer[cnt - 1] != '\n') {
            buffer[cnt] = '\n'; //add a \n if not have.
        }
        printf("%s", buffer);
    }
}

/* Updated by Rui Xie Enhanced log.*/
void ngLoggerCocoa::EnhancedLog(NGCSTR filename, NGCSTR funcname, int linenum, NGCSTR format,va_list args)
{
    char buffer[BUFFER_SIZE] = {0, };
    sprintf(buffer, "%s:%d:%s, %s",filename,linenum,funcname,format);
    Log((NGSTR) buffer, args);
}