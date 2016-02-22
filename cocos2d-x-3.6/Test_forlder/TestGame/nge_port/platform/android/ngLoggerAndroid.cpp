/*
 *  ngLoggerAndroid.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/10/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ngLoggerAndroid.h"
#include <android/log.h>

#define BUFFER_SIZE 40960

void ngLoggerAndroid::Log(NGCSTR format, va_list args) {

	char buffer[BUFFER_SIZE] = {'\0'};
	int32 cnt = vsnprintf(buffer, BUFFER_SIZE, format, args);
    if (cnt > 0 && cnt < BUFFER_SIZE - 1) { //available buffer
        if (buffer[cnt - 1] != '\n') {
            buffer[cnt] = '\n'; //add a \n if not have.
        }
        __android_log_print(ANDROID_LOG_INFO, "native_project_lib", "%s", buffer);
    }
}
