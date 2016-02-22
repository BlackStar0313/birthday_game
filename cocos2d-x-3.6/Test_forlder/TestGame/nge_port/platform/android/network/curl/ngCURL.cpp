//
//  ngCURL.cpp
//  
//
//  Created by chen on 12-10-31.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include "ngCURL.h"

#if defined (NGE_PLATFORM_ANDROID)

#include <android/ngJNIUtils.h>

#include <curl/curl.h>

#include <android/network/curl/ngCURLManager.h>
#include <nsl/ngString.h>
#include <nsl/ngArrayList.h>

#include <ncal/ngHttpResponse.h>

#pragma mark - curl callback
static size_t curl_header_func(void*, size_t, size_t, void*);
static size_t curl_data_func(void*, size_t, size_t, void*);
static size_t curl_read_func(void*, size_t, size_t, void*);

#ifdef DEBUG_LOG
static int curl_debug_func(CURL* curl, curl_infotype type, char* curl_data, size_t size, void* debug_data);
#endif

/* thread init func. */
void doCURLRequestAsync(ngCURL*);

typedef struct __T_CURL_REQUEST_DATA__ {
    uint8* m_pData;
    int32 m_size;
    uint8* m_ptr;
    
    __T_CURL_REQUEST_DATA__() {
        m_pData = NULL;
        m_size  = 0;
        m_ptr = 0;
    }
} CURLRequestData;


#define CURL_INSTANCE_CHECK()           \
    if (m_pCURL == NULL) {              \
        NGASSERT(m_pCURL != NULL);      \
        return;                         \
    }

#define CURL_SAFE_FREE(curl)            \
    if (curl) {                         \
        curl_easy_cleanup(curl);        \
        curl = NULL;                    \
    }

#define CURL_SAFE_CLEAR_HEADER_CHUNK(chunk) \
    if (chunk) {                            \
        curl_slist_free_all(chunk);         \
        chunk = NULL;                       \
    }

ngCURL::ngCURL()
: m_pListener(NULL)
//, m_async(FALSE)
{
    Initialize();
}

ngCURL::ngCURL(ngCURLListener* l)
: m_pListener(l)
//, m_async(FALSE)
{
    pthread_mutex_init(&mutex, NULL);
    SetIdle();
    
    Initialize();
}

ngCURL::~ngCURL() {
    Clear();

    pthread_mutex_destroy(&mutex);
}

#pragma mark - network

void ngCURL::SetUrl(NGCSTR url) {
    m_pUrl = url;
}

void ngCURL::Get(NGCSTR url) {
    CURL_INSTANCE_CHECK();

    if (url) {
        m_pUrl = url;
    }
    
    Prepare();

#if 0
    //default curl method is "GET".
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
#endif

    DoRequest();
}

void ngCURL::Put(NGCSTR url) {
    CURL_INSTANCE_CHECK();
    
    if (url) {
        m_pUrl = url;
    }
    
    Prepare();

#if 0
    /*
     * deprecated and use CURLOPT_UPLOAD in 7.12.1 or later.
     * see: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html
     * [zhen.chen]
     */
    curl_easy_setopt(m_pCURL, CURLOPT_PUT, 1L);
#else
    curl_easy_setopt(m_pCURL, CURLOPT_UPLOAD, 1L);
#endif

    DoRequest();

}

void ngCURL::POST(NGCSTR url) {
    CURL_INSTANCE_CHECK();
    
    if (url) {
        m_pUrl = url;
    }
    
    Prepare();
    
    curl_easy_setopt(m_pCURL, CURLOPT_POST, 1L);
    
    DoRequest();
}

void ngCURL::AsyncGet(NGCSTR url) {
    CURL_INSTANCE_CHECK();
    
    if (url) {
        m_pUrl = url;
    }
    
    NG_DEBUG_LOG("[curl] GET(async) %s.", m_pUrl.GetCString());
    
    Prepare();

#if 0
    //default curl method is "GET".
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
#endif
    
    DoAsyncRequest();
}

void ngCURL::AsyncPut(NGCSTR url) {
    CURL_INSTANCE_CHECK();
    
    if (url) {
        m_pUrl = url;
    }
    
    NG_DEBUG_LOG("[curl] PUT(async) %s.", m_pUrl.GetCString());
    
    Prepare();
    
#if 0
    /*
     * deprecated and use CURLOPT_UPLOAD in 7.12.1 or later.
     * see: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html
     * [zhen.chen]
     */
    curl_easy_setopt(m_pCURL, CURLOPT_PUT, 1L);
#else
    curl_easy_setopt(m_pCURL, CURLOPT_UPLOAD, 1L);
#endif
    
    DoAsyncRequest();
}

void ngCURL::AsyncPost(NGCSTR url) {
    CURL_INSTANCE_CHECK();
    
    if (url) {
        m_pUrl = url;
    }
    
    NG_DEBUG_LOG("[curl] POST(async) %s.", m_pUrl.GetCString());
    
    Prepare();
    
    curl_easy_setopt(m_pCURL, CURLOPT_POST, 1L);
    
    DoAsyncRequest();
}

#pragma mark -

void ngCURL::Clear() {
    CURL_SAFE_CLEAR_HEADER_CHUNK(m_pHeaderChunk);
    CURL_SAFE_FREE(m_pCURL);
    
    m_threadStatus = casIdle;
    m_responseStatusCode = 200;
    
    CURLRequestData* pReqData = (CURLRequestData*)m_pRequestData;
    SAFE_DELETE(pReqData);
    m_pRequestData = NULL;
}
    
void ngCURL::Initialize() {
    m_pCURL = NULL;
    m_pHeaderChunk = NULL;
    m_responseStatusCode = 200;
    
    m_pRequestData = NULL;

    m_threadStatus = casIdle;
    
    m_pMainQueueInfo = NULL;
    
    NG_DEBUG_LOG("curl version: %s\n", curl_version());
    m_pCURL = curl_easy_init();
    
    curl_easy_setopt(m_pCURL, CURLOPT_NOSIGNAL, 1L);

//    CURL_INSTANCE_CHECK();
}

void ngCURL::Prepare() {
#ifdef DEBUG_LOG
    curl_easy_setopt(m_pCURL, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(m_pCURL, CURLOPT_DEBUGFUNCTION, curl_debug_func);
    curl_easy_setopt(m_pCURL, CURLOPT_DEBUGDATA, this);
#endif
    
    curl_easy_setopt(m_pCURL, CURLOPT_WRITEHEADER, this);
    curl_easy_setopt(m_pCURL, CURLOPT_HEADERFUNCTION, curl_header_func);
    
    curl_easy_setopt(m_pCURL, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_pCURL, CURLOPT_WRITEFUNCTION, curl_data_func);

    //??? this data will be free in curl callback. is it good? [zhen.chen]
    CURLRequestData* pRequestData = DNEW(CURLRequestData);
    if (m_pListener) {
        pRequestData->m_pData = m_pListener->GetHeaderData();
        pRequestData->m_size  = m_pListener->GetHeaderDataSize();
        pRequestData->m_ptr  = pRequestData->m_pData;
    }
    curl_easy_setopt(m_pCURL, CURLOPT_READDATA, pRequestData);
    curl_easy_setopt(m_pCURL, CURLOPT_READFUNCTION, curl_read_func);
    curl_easy_setopt(m_pCURL, CURLOPT_INFILESIZE, pRequestData->m_size);
    
    m_pRequestData = pRequestData;  //use m_pRequestData to handle data ptr.

    //TODO: prepare chunk here.
    //<<
    ngStringList chunkData;
    if (m_pListener) {
        m_pListener->MakeHeaderChunk(chunkData);
    }
    for (int32 i = 0; i < chunkData.Size(); i++) {
        ngStringV2* pStr = (ngStringV2*)chunkData[i];
        m_pHeaderChunk = curl_slist_append(m_pHeaderChunk, pStr->GetCString());
        SAFE_DELETE(pStr);
    }
    chunkData.Clear();
    
    curl_easy_setopt(m_pCURL, CURLOPT_HTTPHEADER, m_pHeaderChunk);
    
    //>>
}

void ngCURL::DoRequest() {
    
    NGASSERT(!m_pUrl.IsEmpty());
    
    curl_easy_setopt(m_pCURL, CURLOPT_URL, m_pUrl.GetCString());
	CURLcode res = curl_easy_perform(m_pCURL);
    
    Clear();

    //TODO: check res.
    
}

void ngCURL::DoAsyncRequest() {
    NGASSERT(!m_pUrl.IsEmpty());

    curl_easy_setopt(m_pCURL, CURLOPT_URL, m_pUrl.GetCString());
    
    SetBusy();
    
    ngCURLManager::GetInstance()->OfferCURL(this);

    doCURLRequestAsync(this);
}

void ngCURL::SetBusy() {
    pthread_mutex_lock(&mutex);
    m_threadStatus = casBusy;
    pthread_mutex_unlock(&mutex);
}

#if 0
void ngCURL::SynchronousLock() {
    pthread_mutex_lock(&mutex);
//    m_threadStatus = casSynchronous;
//    pthread_mutex_unlock(&mutex);
}

void ngCURL::SynchronousUnlock() {
//    pthread_mutex_lock(&mutex);
//    m_threadStatus = casBusy;
    pthread_mutex_unlock(&mutex);
}

void ngCURL::WaitSynchronous() {
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
}
#endif

void ngCURL::SyncMainQueue(MainQueueFunc func, void* pData, int32 nData, void* pContext) {
    pthread_mutex_lock(&mutex);
    SAFE_DELETE(m_pMainQueueInfo);
    CURLMainQueueInfo* pInfo = DNEW(CURLMainQueueInfo);
    pInfo->m_func = func;
    pInfo->m_pData = pData;
    pInfo->m_nData = nData;
    pInfo->m_pContext = pContext;
    m_pMainQueueInfo = pInfo;    
    sem_init(&sem, 0, 0);
    pthread_mutex_unlock(&mutex);

    //wait CommitMainQueue() call in MainThread.
    sem_wait(&sem);
    sem_destroy(&sem);

}

void ngCURL::CommitMainQueue() {
    boolean commit = FALSE;
    pthread_mutex_lock(&mutex);
    commit = m_pMainQueueInfo != NULL;
    pthread_mutex_unlock(&mutex);
    
    if (commit) {
        (m_pMainQueueInfo->m_func)(m_pMainQueueInfo->m_pData, m_pMainQueueInfo->m_nData, m_pMainQueueInfo->m_pContext);
        SAFE_DELETE(m_pMainQueueInfo);
        sem_post(&sem);
    }
}

void ngCURL::SetDone(int32 statusCode) {
    
    m_responseStatusCode = statusCode;
    
    pthread_mutex_lock(&mutex);
    m_threadStatus = casDone;
    pthread_mutex_unlock(&mutex);
}

void ngCURL::SetErrorDone(int32 errorCode) {
    m_responseStatusCode = errorCode;
    
    pthread_mutex_lock(&mutex);
    m_threadStatus = casErrorDone;
    pthread_mutex_unlock(&mutex);
}

void ngCURL::SetIdle() {
    pthread_mutex_lock(&mutex);
    m_threadStatus = casIdle;
    pthread_mutex_unlock(&mutex);
}

void ngCURL::NotifyDone() {
    
    NG_DEBUG_LOG("[curl] %s\n", __FUNCTION__);
    
    if (m_pListener == NULL) return;

    pthread_mutex_lock(&mutex);
    CURLThreadStatus st = m_threadStatus;
    pthread_mutex_unlock(&mutex);

    NG_DEBUG_LOG("[curl] status: %d\n", st);

    switch (st) {
        case casDone:
            if (m_responseStatusCode != 200) {
                m_pListener->OnCURLError(m_responseStatusCode, "[curl] response status non-20)!~");
            } else {
                m_pListener->OnCURLFinish();
            }
            break;
        case casErrorDone:
            NG_DEBUG_LOG("[curl] network error!~\n");
            m_pListener->OnCURLError(m_responseStatusCode, "[curl] network error!~");
            break;
        default:
            break;
    }
    
}

boolean ngCURL::IsDone() {
    pthread_mutex_lock(&mutex);
    CURLThreadStatus st = m_threadStatus;
    pthread_mutex_unlock(&mutex);
    
    return (st == casDone || st == casErrorDone);
}

void ngCURL::ResetRequestData() {
    if (m_pRequestData == NULL) {
        return;
    }
    
    CURLRequestData* pReqData = (CURLRequestData*)m_pRequestData;
    pReqData->m_ptr = pReqData->m_pData;
}

//main queue func
static void curl_queue_data(void* buffer, int32 len, void* pContext) {
    ngCURL* pCURL = (ngCURL*)pContext;
    if (pCURL->GetLisntener() && !pCURL->GetLisntener()->IsResponseDone()) {
        size_t statusCode;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_RESPONSE_CODE, &statusCode);
        ngHttpResponse* pResponse = DNEW(ngHttpResponse);
        pResponse->SetStatusCode(statusCode);
        
        NG_DEBUG_LOG("[curl] status code: %d", statusCode);
        
        // set headers.
        ngHttpHeader* pHeader = pResponse->GetHeader();
        double content_length = 0;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
        ngStringV2 strContentLength;
        strContentLength.Format("%d", (size_t)content_length);
        pHeader->Put(HTTP_CONTENT_LENGTH, strContentLength.GetCString());
        NG_DEBUG_LOG("[curl] content-length: %s\n", strContentLength.GetCString());
        
        char* headerVal;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_CONTENT_TYPE, &headerVal);
        NG_DEBUG_LOG("[curl] headerval: %s", headerVal);
        pHeader->Put(HTTP_CONTENT_TYPE, headerVal);
        
        pCURL->GetLisntener()->OnCURLResponse(pResponse);
    }
    
    NG_DEBUG_LOG("[curl] data callback, data len: %d, curl(0x%X)", len, pCURL);
    if (pCURL->GetLisntener()) {
        pCURL->GetLisntener()->OnCURLData((uint8*)buffer, len);
    }
}

#pragma mark - curl callback func

static size_t
curl_header_func(void* buffer, size_t size, size_t nmemb, void* userData)
{
	int len = size * nmemb;
    ngCURL* pCURL = (ngCURL*)userData;
    
    NG_DEBUG_LOG("[curl] header callback, header len: %d, curl(0x%X)", len, pCURL);

#if 0
    //move to curl_data_func()
    if (pCURL->GetLisntener()) {
        size_t statusCode;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_RESPONSE_CODE, &statusCode);
        ngHttpResponse* pResponse = DNEW(ngHttpResponse);
        pResponse->SetStatusCode(statusCode);
        
        NG_DEBUG_LOG("[curl] status code: %d", statusCode);

        // set headers.
        ngHttpHeader* pHeader = pResponse->GetHeader();
        double content_length = 0;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
        ngStringV2 strContentLength;
        strContentLength.Format("%d", (size_t)content_length);
        pHeader->Put(HTTP_CONTENT_LENGTH, strContentLength.GetCString());
        NG_DEBUG_LOG("[curl] content-length: %d\n", (size_t)content_length);

        char* headerVal;
        curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_CONTENT_TYPE, &headerVal);
        NG_DEBUG_LOG("headerval: %s", headerVal);
        pHeader->Put(HTTP_CONTENT_TYPE, headerVal);

        pCURL->GetLisntener()->OnCURLResponse(pResponse);
    }
#endif

	return len;	
}

static size_t
curl_data_func(void* buffer, size_t size, size_t nmemb, void* userData) 
{
    
    ngCURL* pCURL = (ngCURL*)userData;
	int len = size * nmemb;

    pCURL->SyncMainQueue(curl_queue_data, buffer, len, pCURL);

	return len;
}

static size_t 
curl_read_func(void* ptr, size_t size, size_t nmemb, void* userData)
{
    CURLRequestData* pRequestData = (CURLRequestData*)userData;
	size_t curl_size = nmemb * size;

    NG_DEBUG_LOG("[curl] read callback, data len: %ld, data(0x%X)", curl_size, userData);
	
    int leftDataSize = pRequestData->m_size - (pRequestData->m_ptr - pRequestData->m_pData);
    if (leftDataSize <= 0) {
        /* reach end. */
        return 0;
    }
    
	size_t to_copy = (leftDataSize < curl_size) ? leftDataSize : curl_size;
	memcpy(ptr, pRequestData->m_ptr, to_copy);
    pRequestData->m_ptr += to_copy;
    
	return to_copy;
}

#ifdef DEBUG_LOG
static int
curl_debug_func(CURL* curl, curl_infotype type, char* curl_data, size_t size, void* userData)
{
    ngCURL* pCURL = (ngCURL*)userData;

    char copy_data[size + 1];
    copy_data[size] = '\0';
    switch (type) {
        case CURLINFO_TEXT:
            memcpy(copy_data, curl_data, size);
            NG_DEBUG_LOG("[curl debug] text : %s\n", copy_data);
            break;
        case CURLINFO_HEADER_IN:
            memcpy(copy_data, curl_data, size);
            NG_DEBUG_LOG("[curl debug] header in : %s\n", copy_data);
            break;
        case CURLINFO_HEADER_OUT:
            memcpy(copy_data, curl_data, size);
            NG_DEBUG_LOG("[curl debug] header out : %s\n", copy_data);
            break;
        case CURLINFO_DATA_IN:
//            memcpy(copy_data, curl_data, size);
//            NG_DEBUG_LOG("[curl debug] data in : %s\n", copy_data);
            break;
        case CURLINFO_DATA_OUT:
            memcpy(copy_data, curl_data, size);
            NG_DEBUG_LOG("[curl debug] data out : %s\n", copy_data);
            break;
            
        default:
            break;
    }
    
    return 0;
}
#endif

#pragma mark - thread func here
/* use POSIX Threads to handle curl request */
#include "pthread.h"

#define CURL_RETRY_COUNT 5
void* startCURLRequestSession(void* pContext) {
    
    AttachCurrentJNIEnv();
    
    ngCURL* pCURL = (ngCURL*)pContext;
    
    CURLcode lastCode = CURLE_OK;
    
    for (int i = 0; i < CURL_RETRY_COUNT; i++) {

#ifdef DEBUG_LOG
        if (i > 0) {
            NG_DEBUG_LOG("[curl] retry count: %d\n", i);
        }
#endif

#if 1
        lastCode = curl_easy_perform(pCURL->GetPTRCURL());
#else
        /* error debug case. */
        lastCode = CURLE_OPERATION_TIMEDOUT;
#endif
        NG_DEBUG_LOG("[curl] (async) curl result code: %d\n", lastCode);
        
        /* set done flag. */
        if (lastCode == CURLE_OK) {
            size_t statusCode;
            curl_easy_getinfo(pCURL->GetPTRCURL(), CURLINFO_RESPONSE_CODE, &statusCode);
            NG_DEBUG_LOG("[curl] status code is %ld\n", statusCode);
            
            if (statusCode >= 500) {
                //retry
                pCURL->ResetRequestData();
            } else {
                /* cleanup */
                pCURL->Clear();
                pCURL->SetDone(statusCode);
                break;
            }
        } else {
#if 0
            /* cleanup */
            pCURL->Clear();
            
            //TODO: notify error.
#else
            //retry
            pCURL->ResetRequestData();
#endif
        }
    }
    
    if (lastCode != CURLE_OK) {
        NG_DEBUG_LOG("[curl] error code: %d\n", lastCode);
        pCURL->Clear();
        pCURL->SetErrorDone(lastCode);
    }

    DetachCurrentJNIEnv();
    
    return NULL;
}

void doCURLRequestAsync(ngCURL* pCURL) {
    
#define ASSERT_POSIX_THREAD_VAL(val)    if (!(val == 0)) { NGASSERT(0); success = FALSE; break; }
    
    // Create the thread using POSIX Threads, it can use to Android platform.
    boolean success = TRUE;
    do {
        pthread_attr_t  attr;
        pthread_t       posixThreadID;
        int             returnVal;
        
        /* 初始化线程属性 */
        returnVal = pthread_attr_init(&attr);        
        ASSERT_POSIX_THREAD_VAL(returnVal);
        
        /* 这里使用PTHREAD_CREATE_DETACHED，线程执行完成后立即退出，释放线程资源，有需要时可以设置线程启动后等待一会儿。[zhen.chen] */
        returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        ASSERT_POSIX_THREAD_VAL(returnVal);
        
        /* make context */
        //use ngCURL instance as context.
        
        int threadError = pthread_create(&posixThreadID, &attr, &startCURLRequestSession, pCURL);
        returnVal = pthread_attr_destroy(&attr);
        ASSERT_POSIX_THREAD_VAL(returnVal);

        if (threadError != 0) {
//            SAFE_DELETE(pCURL);
            NG_DEBUG_LOG("[curl] create POSIX Thread failed!~ code : %d\n", threadError);
        }
    } while (0);
    
    NGASSERT(success);
}



#endif