/*
*  ngHttpSession.h
*  NextGenEngine
*
*  Created by cooler on 10-5-22.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGHTTPSESSION_H__
#define __NGHTTPSESSION_H__

#include "./ngConnectionSession.h"
#include "./ngHttpResponse.h"
#include "../nsl/ngByteBuffer.h"

class ngHttpSession : public ngConnectionSession {
public:
    static ngHttpSession* CreateSession(); /* platform impl */

protected:
	ngHttpSession();
	virtual ~ngHttpSession();

public:
	virtual ngConnectionResponse* GetResponse() { return m_pResponse; }
    
    void SetFileBuffered(boolean fb) { m_fileBuffered = fb; }

protected:
    /*! 继承自ngConnectionSession，创建并返回一个ngHttpRequest对像实例。 */
    virtual ngConnectionRequest* CreateRequest();

public:
	virtual void OnConnect();
	virtual void OnResponse(ngConnectionResponse* pResponse);
	virtual void OnData(uint8 *pData, uint32 nData);
	virtual void OnErrorStop(NG_CONNECTION_ERROR nErrorCode);
	virtual void OnFinish();

protected:
	ngHttpResponse* m_pResponse;
    
    boolean m_fileBuffered;

public:
    static void SetXorKey(NGCSTR key) { m_XorKey = key; }
    static ngString& GetXorKey() { return m_XorKey; }

    static void SetTailKey(NGCSTR key) { m_tailKey = key; }
    static ngString& GetTailKey() { return m_tailKey; }

private:
    static ngString m_XorKey;
    static ngString m_tailKey;
};

#endif //__NGHTTPSESSION_H__