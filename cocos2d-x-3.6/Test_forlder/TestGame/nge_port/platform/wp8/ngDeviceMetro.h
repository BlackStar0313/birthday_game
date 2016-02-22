/*
 *  CDeviceMetro.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *  Modified by lg2000 on 09-04-13.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGDEVICEANDROID_H__
#define __NGDEVICEANDROID_H__

#include "NGE_Core.h"

namespace NextGenEngineMetro {
    ref class DeviceInfo;
}

class ngDeviceMetro : public ngDevice
{
public:
    ngDeviceMetro();
    virtual ~ngDeviceMetro();

    virtual void Initialize();

	virtual void GetFileWritePath( ngStringV2& out, const char* file );


    virtual const char* GetDeviceType();
    virtual const char* GetDeviceVersion();
    virtual const char* GetDeviceID();
    virtual const char* GetDeviceFamily();

    virtual boolean IsIPad() { return FALSE; };

	virtual NGCSTR CurrentLocaleLanguage();

	virtual void LaunchURL( const char* url );

	virtual int64 GetFreeMemory();

	virtual boolean IsFileExist( const char* file );

	virtual boolean GetFilePath( ngStringV2& out, const char* file );

	virtual void GetFileCachePath( ngStringV2& out, const char* file );

	virtual double GetCurrentSystemTime();

	virtual NGCSTR CurrentLocaleRegion();

	virtual float GetDeviceVersionf();

	virtual const char* GetDevicePlatform();
	
	virtual NGCSTR GetChannelID() const;

	virtual NGCSTR GetReleaseVersion() const;
	
	virtual const char* GetDeviceVendorID();

	virtual int32 GetRotationAngle();

	virtual int32 GetKeyboardHeight();

	virtual void SetKeyboardHeight( int32 height );

private:
	/*! 构造函数中(游戏初始化时)调用，初始化设备信息。 */
	void SetDeviceInfo();

protected:
	//boolean FindFilePath(ngStringV2& out,const ngStringV2& in);

	virtual const char* GetDeviceMacID();

	virtual void RegisterPushNotification();

	virtual void PushLocalNotification( int64 fireTimeFromNow, ngStringV2* msg, int32 channel = 0, NGCSTR param = NULL );

	virtual void CancelLocalNotification( int32 channel, NGCSTR param = NULL );


	ngStringV2 DoGetChannelID();

    /* 使用完整路径检查文件是否存在 */
    boolean IsFileExistWithFullPath(const char* full_path);

private:
	ngStringV2 m_deviceId;
	ngStringV2 m_deviceMacId;
	ngStringV2 m_deviceFamily;
	ngStringV2 m_deviceType;
	ngStringV2 m_deviceVersion;

	ngStringV2 m_strChannelID;
};

#endif
