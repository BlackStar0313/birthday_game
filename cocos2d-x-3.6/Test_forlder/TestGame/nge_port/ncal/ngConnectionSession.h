/*
*  ngConnectionSession.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGCONNECTIONSESSION_H__
#define __NGCONNECTIONSESSION_H__

#include "./ngConnectionCommon.h"
#include "./ngConnectionListener.h"
#include <core/NGE_NSL.h>

class ngConnectionResponse;
class ngConnectionRequest;

/*!
 @class ngConnectionSession
 @abstract 连接会话，管理连接状态
 @discussion ngConnectionSession本身是ngConnectionListener，子类实现相应方法响应连接事件通知，处理数据并调用上层m_pListener的接口继续处理。
 */
class ngConnectionSession
: public ngConnectionListener {

protected:
	virtual ~ngConnectionSession(); /* 只能由ngConnectionManager销毁。 */
    friend class ngConnectionManager;

public:
	virtual void Connect(ngStringV2& url);
	virtual void Cancel() { CancelInternal(); }

	void SetConnectionListener(ngConnectionListener* pListener);

    /*! 获取Request，如果m_pRequest为NULL，创建m_pRequest。 */
	ngConnectionRequest* GetRequest();

    virtual ngConnectionResponse* GetResponse() { NGASSERT(0); return NULL; }
	virtual ngByteBuffer* GetResponseData() { NGASSERT(0); return NULL; }

	virtual NG_SESSION_STATUS GetStatus() { return m_status; }
	void SetUseless() { m_status = SESSION_USELESS; }
	boolean IsUseless() { return (m_status == SESSION_USELESS); }

	virtual void HandleUpdate(int32 deltaMS) {};
	
	int32 GetErrorCode() { return m_nErrorCode; }
    
    void SetDESEncrypt(boolean encrypt) { m_DESEncrypt = encrypt; }
    boolean IsDESEncrypt() { return m_DESEncrypt; }

    void SetMD5Check(boolean md5_chk) { m_MD5ChkSum = md5_chk; }
    boolean IsMD5Check() { return m_MD5ChkSum; }

protected:
    /*! 子类继承CreateRequest()用于创建特定协议的ngConnectionRequest对象实例，只在GetRequest()中调用。 */
    virtual ngConnectionRequest* CreateRequest() = 0;

public:
#pragma mark - ngConnectionListener
	virtual void OnConnect() = 0;
	virtual void OnResponse(ngConnectionResponse* pResponse) = 0;
	virtual void OnData(uint8 *pData, uint32 nData) = 0;
	virtual void OnErrorStop(NG_CONNECTION_ERROR nErrorCode) = 0;
	virtual void OnFinish() = 0;
protected:
	ngConnectionSession();
	virtual void RequestInternal(ngStringV2& url) {}
	virtual void CancelInternal() {}

protected:
	NG_SESSION_STATUS m_status;
	int32 m_nErrorCode;

    ngConnectionListener* m_pListener;
	ngConnectionRequest* m_pRequest;

    /* encrypt flag */
    boolean m_DESEncrypt;   //TODO: if need more encrypt type, here should be enum or int32 flag
    boolean m_MD5ChkSum;
};

#pragma mark - inline implement
inline ngConnectionRequest* ngConnectionSession::GetRequest() {
    if (m_pRequest == NULL) {
        m_pRequest = CreateRequest();
    }
    return m_pRequest;
}


inline void ngConnectionSession::SetConnectionListener(ngConnectionListener* pListener) {
	m_pListener = pListener;
}

#endif //__NGHTTPSESSION_H__