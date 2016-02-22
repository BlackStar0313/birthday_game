/*
 *  CDeviceAndroid.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *  Modified by lg2000 on 09-04-13.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGDEVICEANDROID_H__
#define __NGDEVICEANDROID_H__

#include <core/NGE_Types.h>
#include <core/ngDevice.h>

class ngDeviceAndroid : public ngDevice {
public:
    ngDeviceAndroid();
    virtual ~ngDeviceAndroid();

    virtual void Initialize();
    virtual boolean IsFileExist(const char* file);
    virtual boolean	GetFilePath(ngStringV2& out, const char* file);
	virtual void GetFileWritePath(ngStringV2& out, const char* file);

    virtual const char* GetDeviceType();
    virtual const char* GetDeviceVersion();
    virtual float       GetDeviceVersionf();
    virtual const char* GetDeviceID();
    virtual const char* GetDeviceFamily();
    
//    virtual const char* GetDevicePlatform();

    /* get channel id in android */
    virtual NGCSTR GetChannelID() const;

    virtual NGCSTR	CurrentLocaleLanguage();
    virtual NGCSTR	CurrentLocaleRegion();
    
    virtual void LaunchURL(const char* url);

//    virtual boolean IsIPad() { return FALSE; };
#if 0
    virtual void    RegisterPushNotification(NGCSTR userId, NGCSTR host, int32 port, NGCSTR apiKey);
    virtual void	PushLocalNotification(int64 fireTimeFromNow, ngStringV2* msg, int32 channel = 0, NGCSTR param = NULL);
    virtual void    CancelLocalNotification(int32 channel, NGCSTR param = NULL);
#endif

    virtual int64   GetFreeMemory();

};

#endif
