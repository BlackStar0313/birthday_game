/*
 *  ngHttpClient.h
 *  template
 *
 *  Created by Yuhui Wang on 5/06/11.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGHTTPCLIENT_H__
#define __NGHTTPCLIENT_H__

#include "../json/ngJsonRoot.h"
#include "../json/ngJsonElement.h"

#ifdef NGE_PLATFORM_METRO
#undef ERROR_SUCCESS
#endif // NGE_PLATFORM_METRO


#define ERROR_MAINTENANCE_PERIOD	3002
#define ERROR_SUCCESS				0
#define ERROR_UNKNOWN				1001
#define ERROR_NO_AIRLINE			2009

#define ERROR_WRONGINFO				"1006"
#define ERROR_PLAYERNOTEXIST		"1005"

class ngHttpSession;
class ngHttpClientListener;

class ngHttpClient {
public:
	ngHttpClient();
	virtual ~ngHttpClient();
	
public:
	enum CLIENT_STATUS {
		CLIENT_IDLE = 0,
		CLIENT_ACTIVE,
		CLIENT_FINISH
	};
    
    enum SYSTEM_REQ_TYPE {
        REQ_NOTHING = 0,
        REQ_RATER
    };
	
public:
	void	SetListener(ngHttpClientListener* pListener);
	ngHttpClientListener* GetListener() { return m_pListener; }
	static void	SetGameId(NGCSTR gameId);
    static void SetGameVersion(int32 version);
    static void SetGamePlatform(int32 platform);
    static void SetServerIdx(int32 serverIdx);
    void    SetRequestType(int32 t) { m_reqType = t; }
	virtual int32 ParseResponse(int64 timestamp, void *pData);
    virtual int32 ParseError( NGCSTR errorCode, NGCSTR errorMessage, void *pData);
	
public:
	virtual boolean HandleUpdate(int32 deltaMS);

	CLIENT_STATUS GetStatus() { return m_status; }

    int32 PutURL(ngStringV2& url, ngJsonHash* pReqHash, boolean useLocalServer = FALSE);

protected:
	virtual ngJsonHash* GetHeader();
	ngJsonRoot* CreateReqBody(ngJsonHash* bodyHash);
	void ShowErrorDialog();
	
protected:
	ngHttpSession* m_pSession;
	ngHttpClientListener* m_pListener;
	ngJsonRoot m_jsonRoot;
	CLIENT_STATUS m_status;
	static NGCSTR m_gameId;
    static int32  m_gameVersion;
    static int32  m_platform;
    static int32  m_serverIdx;
	
protected:
	int32 m_reqType;
	
};

#endif //__NGHTTPCLIENT_H__