//
//  ngCURL.h
//  
//
//  Created by chen on 12-10-31.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifndef __NGCURL_H__
#define __NGCURL_H__

#include <core/NGE_Types.h>
#include <core/NGE_NSL.h>

#if defined (NGE_PLATFORM_ANDROID)

/* only available in android. */

class ngHttpResponse;

#pragma mark - ngCURLListener

/*!
 @class ngCURLListener
 @abstract provider interface to handle curl event.
 @discussion class implement this interface must be thread safe.
 */

class ngCURLListener {
public:
    /* implement this interface if need extra header chunk. */
    virtual void MakeHeaderChunk(ngStringList& chunk) {};
    
    /* this to interface is not necessary if have no data to send in request. */
    virtual uint8*  GetHeaderData() { return NULL; }
    virtual int32   GetHeaderDataSize() { return 0; }

    virtual boolean IsResponseDone() = 0;
    virtual void OnCURLResponse(ngHttpResponse* response) = 0;
    virtual void OnCURLData(uint8* pData, uint32 nData) = 0;
    virtual void OnCURLFinish() = 0;
    virtual void OnCURLError(int32 errCode, NGCSTR errMsg) = 0;
};

#pragma mark -

//type CURL is define as a void in curl.h: "typedef void CURL;"
//so I do this:
typedef void* PTRCURL;

//main queue
//this is a prototype to call func in main thread, just fit to use curl yet, fill it later. [zhen.chen]
typedef void (*MainQueueFunc)(void* pData, int32 nData, void* pContext);
typedef struct __CURL_MAIN_QUEUE_INFO__ {
    MainQueueFunc m_func;
    void* m_pData;
    int32 m_nData;
    void* m_pContext;
} CURLMainQueueInfo;

#ifdef __cplusplus
extern "C" {
#endif
    struct curl_slist;
#ifdef __cplusplus
}
#endif

#pragma mark - ngCURL

#include "pthread.h"
#include "semaphore.h"

/*!
 @class ngCURL
 @abstract a package class for curl lib.
 @discussion 
 */
class ngCURL {
public:
//    static ngCURL* CreateInstance();
public:
    ngCURL();
    ngCURL(ngCURLListener* l);
    ~ngCURL();

//private:
//    tyepdef enum HTTP_METHOD {
//		HTTP_GET = 0,
//		HTTP_POST,
//		HTTP_PUT,
//		HTTP_DELETE
//	} curlHttpMethod;

public:
    void SetListener(ngCURLListener* l);
    ngCURLListener* const GetLisntener() const;
    
    void SetUrl(NGCSTR url);

public:
    /* sync method. */
    void Get(NGCSTR url = NULL);
    void Put(NGCSTR url = NULL);
    void POST(NGCSTR url = NULL);
//    void Delete(NGCSTR url = NULL);
    
    /* async method. */
    void AsyncGet(NGCSTR url = NULL);
    void AsyncPut(NGCSTR url = NULL);
    void AsyncPost(NGCSTR url = NULL);
//    void AsyncDelete(NGCSTR url = NULL);
    
    void Clear();
    
    const PTRCURL GetPTRCURL() const;

private:
    void Initialize();
    void Prepare();
    void DoRequest();
    void DoAsyncRequest();
    
private:
    ngStringV2 m_pUrl;
    PTRCURL m_pCURL;  //a pointer to a CURL struct.
    struct curl_slist *m_pHeaderChunk;
    
//    boolean m_async;
    
    ngCURLListener* m_pListener;
    
#pragma mark - thread
protected:
    typedef enum __E_CURL_THREAD_STATUS__ {
        casIdle = 0,
        casBusy,
//        casSynchronous,
        casDone,
        casErrorDone,
    } CURLThreadStatus;

public:    
    boolean IsDone();

#if 0
    void SynchronousLock();
    void SynchronousUnlock();
    void WaitSynchronous();
#endif
    void SyncMainQueue(MainQueueFunc func, void* pData, int32 nData, void* pContext);
    void CommitMainQueue();

private:
 
    void SetBusy();

    /* call from thread func startCURLRequestSession(). */
    void SetDone(int32 statusCode);
    
    /* call from thread func startCURLRequestSession(). */
    void SetErrorDone(int32 errorCode);
    
    /* call from friend class ngCURLManager. */
    void SetIdle();
    void NotifyDone();

    void ResetRequestData();

    pthread_mutex_t* GetStatusMutex() { return &mutex; }
    
protected:
    pthread_mutex_t mutex;
    CURLThreadStatus m_threadStatus;
    
    void* m_pRequestData;   //type: CURLRequestData*

    sem_t sem;
    CURLMainQueueInfo* m_pMainQueueInfo;

    int32 m_responseStatusCode;
    
    friend void* startCURLRequestSession(void*);
    friend class ngCURLManager;
};

#pragma mark - inline implement
inline void ngCURL::SetListener(ngCURLListener* l) {
    m_pListener = l;
}

inline ngCURLListener* const ngCURL::GetLisntener() const {
    return m_pListener;
}

inline const PTRCURL ngCURL::GetPTRCURL() const {
    return m_pCURL;
}

#endif


#endif  //__NGCURL_H__
