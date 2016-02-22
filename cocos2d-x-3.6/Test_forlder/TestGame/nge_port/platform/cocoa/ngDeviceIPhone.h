/*
 *  ngDeviceIPhone.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGDEVICEIPHONE_H__
#define __NGDEVICEIPHONE_H__

#include <core/NGE_Defs.h>
#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>
#include <nsl/ngStringV2.h>
#include <core/ngDevice.h>

class ngDeviceIPhone : public ngDevice {
public:
	ngDeviceIPhone();
	virtual ~ngDeviceIPhone();
	
	virtual void	Initialize();
	
    virtual boolean IsFileExist(const char* file);
	virtual boolean	GetFilePath(ngStringV2& outPath, const char* file);
	virtual void	GetFileWritePath(ngStringV2& out, const char* file);
	virtual void	GetFileCachePath(ngStringV2& out, const char* file);

	virtual NGCSTR	CurrentLocaleLanguage();
	virtual NGCSTR CurrentLocaleRegion();
	
	virtual const char* GetDeviceType();
	virtual const char* GetDeviceVersion();
    virtual float       GetDeviceVersionf();
	virtual const char*	GetDeviceID();
    virtual const char* GetDeviceFamily();
//    virtual const char* GetDevicePlatform();

    /* get channel id in iOS */
    virtual NGCSTR GetChannelID() const;
    
    virtual NGCSTR GetReleaseVersion() const;

	virtual void	LaunchURL(const char* url);

    virtual void    RegisterPushNotification();
#if 0
	virtual void	PushLocalNotification(int64 fireTimeFromNow, ngStringV2* msg, int32 channel = 0, NGCSTR param = NULL);
    virtual void    CancelLocalNotification(int32 channel, NGCSTR param = NULL);
#endif

    virtual int64   GetFreeMemory();
	
protected:
	boolean FindFilePath(ngStringV2& out, const ngStringV2& in);
    
private:

//    ngStringV2 m_platformStr;
};

#endif