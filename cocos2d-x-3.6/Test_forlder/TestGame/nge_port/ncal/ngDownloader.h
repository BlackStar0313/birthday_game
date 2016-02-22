/*
 *  ngDownloader.h
 *  template
 *
 *  Created by Yuhui Wang on 7/30/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#ifndef __NGDOWNLOADER_H__
#define __NGDOWNLOADER_H__

#include "./ngConnectionListener.h"
#include "./ngConnectionSession.h"

class ngDownloadListener 
{
public:
	virtual void OnDownloadComplete(ngByteBuffer* pBuffer, NGCSTR pSuggestName, NGCSTR pUrl) = 0;
	virtual void OnDownloadFailed() = 0;
	virtual void OnDownloadUpdate(int32 nPercent) = 0;
};

class ngDownloader : public ngConnectionListener
{
public:
	ngDownloader();
	virtual ~ngDownloader();
    
    virtual void OnConnect() = 0;
	virtual void OnResponse(ngConnectionResponse* pResponse) = 0;
	virtual void OnData(uint8 *pData, uint32 nData) = 0;
	virtual void OnErrorStop(NG_CONNECTION_ERROR nErrorCode) = 0;
	virtual void OnFinish() = 0;
	
public:
	virtual void	SetListener(ngDownloadListener* pListener) { m_pListener = pListener; }
	virtual int32	Start(ngStringV2& url);
    virtual ngConnectionSession *GetSession() { return m_pSession; }
		
protected:
	ngStringV2 m_url;
	ngDownloadListener	*m_pListener;
	ngConnectionSession *m_pSession;
	int32 m_nCurrentLength;
	int32 m_nMaxLength;
};


#endif //__NGDOWNLOADER_H__