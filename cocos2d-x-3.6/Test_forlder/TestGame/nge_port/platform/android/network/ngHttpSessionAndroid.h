/*
 *  ngHttpSessionAndroid.h
 *  template
 *
 *  Created by Yuhui Wang on 10/8/10.
 *  Modified by chen on 2012-11-1.
 *  Copyright 2010 Anansi. All rights reserved.
 *
 */

#ifndef __NGHTTPSESSIONANDROID_H__
#define __NGHTTPSESSIONANDROID_H__

#include <ncal/ngHttpSession.h>
#include "curl/ngCURL.h"

//this XORKEY is the copy form NSString+XOR.h
#define XORKEY "One ring to rule them all, one ring to find them, one ring to bring them all and in the darkness bind them."

class ngHttpSessionAndroid : public ngHttpSession, public ngCURLListener
{
public:
	ngHttpSessionAndroid();
	~ngHttpSessionAndroid();

protected:
	virtual void RequestInternal(ngStringV2& url);
	virtual void CancelInternal();
    
#pragma mark - ngCURLListener
    /* implement this interface if need extra header chunk. */
    virtual void MakeHeaderChunk(ngStringList& chunk);
    /* this to interface is not necessary if have no data to send in request. */
    virtual uint8*  GetHeaderData();
    virtual int32   GetHeaderDataSize();

    virtual boolean IsResponseDone();
    virtual void OnCURLResponse(ngHttpResponse* response);
    virtual void OnCURLData(uint8* pData, uint32 nData);
    virtual void OnCURLFinish();
    virtual void OnCURLError(int32 errCode, NGCSTR errMsg);
#pragma mark -
    
protected:    
    ngByteBuffer* m_pEncryptData;
};
    

#endif //__NGHTTPSESSIONANDROID_H__