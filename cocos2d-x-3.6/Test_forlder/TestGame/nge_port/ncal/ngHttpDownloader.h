/*
 *  ngHttpDownloader.h
 *  template
 *
 *  Created by Yuhui Wang on 7/30/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */
#ifndef __NGHTTPDOWNLOADER_H__
#define __NGHTTPDOWNLOADER_H__

#include "./ngDownloader.h"

class ngHttpDownloader : public ngDownloader {
public:
    ngHttpDownloader();
    virtual ~ngHttpDownloader();

public:
    void SaveTempFile(boolean save);
    
    // group interface, use to custom session params, like http method, request header, ...
    //[zhen.chen]
    //<<
    void InitHttpSession();
    int32 StartWithExistSession(ngStringV2& url);
    //>>

	virtual int32 Start(ngStringV2& url);
public:
	virtual void OnConnect();
	virtual void OnResponse(ngConnectionResponse* pResponse);
	virtual void OnData(uint8 *pData, uint32 nData);
	virtual void OnErrorStop(NG_CONNECTION_ERROR nErrorCode);
	virtual void OnFinish();	
	
protected:
	NGCSTR GetFileName();
    
private:
    boolean m_saveTempFile;
    FILE* m_pTempFile;  //just use to support res package download.
};

inline void ngHttpDownloader::SaveTempFile(boolean save) {
    m_saveTempFile = save;
}

#endif //__NGHTTPDOWNLOADER_H__
