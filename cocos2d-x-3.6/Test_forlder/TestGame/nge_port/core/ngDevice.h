/*
 *  ngDevice.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-7.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#ifndef __NGDEVICE_H__
#define __NGDEVICE_H__

#include <core/NGE_Types.h>
#include "NGE_Types.h"
#include "../nsl/ngStringV2.h"
#include "ngSingleton.h"

#define SCREEN_WIDTH	(ngDevice::GetInstance()->GetScreenWidth())
#define SCREEN_HEIGHT	(ngDevice::GetInstance()->GetScreenHeight())

#define MAX_DEVICE_ID_LEN  256
#define MAX_CHANNEL_ID_LEN  128

enum ERotationType
{
	SCREEN_ROTATION_NONE,
	SCREEN_ROTATION_90,
	SCREEN_ROTATION_180,
	SCREEN_ROTATION_270
};

class ngSnapshotListener
{
public:
//    virtual void OnSnapshotFinished(void *image) = 0;
    virtual void OnSnapshotFinished() = 0;
};

class ngDevice : public ngSingleton
{
public:
	ngDevice();
	virtual ~ngDevice();
	
	static ngDevice* CreateInstance();
	static ngDevice* GetInstance()	{ return m_pThis; }
	static void ReleaseInstance(void *pObj);
	
	virtual void	Initialize() {};
	virtual void	Destroy() {};

    void SetDeviceDimesion(int32 width, int32 height);
    
    void SetIsRetina(bool isRetina);
    bool GetIsRetina();

	virtual int32	GetScreenWidth()	{ return m_width; }
	virtual int32	GetScreenHeight()	{ return m_height; }

    virtual boolean IsFileExist(const char* file) { return FALSE; }
	virtual boolean	GetFilePath(ngStringV2& outPath, const char* file) { outPath = file; return TRUE; }
	virtual void	GetFileWritePath(ngStringV2& out, const char* file) { out = file; }
	virtual void	GetFileCachePath(ngStringV2& out, const char* file) { out = file; }
	
	virtual NGCSTR	CurrentLocaleLanguage() { return "en"; }
	virtual NGCSTR	CurrentLocaleRegion() { return "en"; }
	
	virtual void	LaunchURL(const char* url) {}
	
	virtual const char* GetDeviceType() { return "Unknown Device"; }
	virtual const char* GetDeviceVersion() { return "0.0"; }
    virtual float       GetDeviceVersionf() { return 0.0f; }
	virtual const char*	GetDeviceID() { return ""; }
    virtual const char* GetDeviceFamily() { return "Unknown"; }

#if 0
    virtual const char* GetDevicePlatform() { return "Unknown Platform"; }
#endif

    /*!
     @function GetChannelID
     @abstract 获取渠道Id
     @discussion 如果没有设置渠道Id，将返回平台名称("ios"/"android"/"metro") 
     
     各平台渠道Id设置:
     
     1. iOS: 默认“ios”。在主plist(xxxinfo.plist)中添加：
     
         <key>ANANSI_CHANNEL</key>
         <string>渠道ID</string>
     
     2. Android: 默认“android”。在manifest文件中的<application>节点下设置:
     
            <meta-data android:value="渠道ID" android:name="ANANSI_CHANNEL"/>
     
     3. Windows Phone: 暂无，默认“metro”
     
     渠道名长度不能超过128字节(包含结束符"\0")，超出部分会被截断。

     */
    virtual NGCSTR GetChannelID() const { return "metro"; /* TODO: clear this code while window phone implement available. */ }
    
    /*!
     @function GetReleaseVersion
     @abstract 获取发布版本号
     @discusssion 如果没有版本号，返回空字符串("")
     发布版本号在 iOS: plist中的bundle version
     */
    virtual NGCSTR GetReleaseVersion() const {return "";};

    /*!
     @function RegisterPushNotification
     @abstract register platform builtin push server, so may be no param here.
     @discussion
     */
    virtual void    RegisterPushNotification() {};

#if 0 //port to cocos2dx, temp disable push.
    /*!
     @function RegisterPushNotification
     @param userId user id
     @param host push server, may be IP addr or domain name
     @param port push server port
     @param apiKey api key, empty str if no need.
     @abstract register custom push server
     @discussion
     */
    virtual void    RegisterPushNotification(NGCSTR userId, NGCSTR host, int32 port, NGCSTR apiKey) {};

	virtual void	PushLocalNotification(int64 fireTimeFromNow, ngStringV2* msg, int32 channel = 0, NGCSTR param = NULL) {}
    virtual void    CancelLocalNotification(int32 channel, NGCSTR param = NULL) {}

#endif
    virtual int64   GetFreeMemory() { return -1; }

protected:
	static ngDevice* m_pThis;
	
	int32	m_width;
	int32	m_height;
    
    boolean m_isRetina;

    char	m_deviceID[MAX_DEVICE_ID_LEN];
    char    m_channelID[MAX_CHANNEL_ID_LEN];
    char    m_releaseVersion[MAX_CHANNEL_ID_LEN];
	
private:
	ngDevice(const ngDevice &source);
	ngDevice& operator=(const ngDevice &source);
};

#endif