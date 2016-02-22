//
//  ngFileManager.cpp
//  islands
//
//  Created by Yuhui Wang on 11/15/11.
//  Copyright (c) 2011 Anansi. All rights reserved.
//

#include <io/ngFileManager.h>
#include <core/ngDevice.h>
#include <stdio.h>
#include <core/NGE_Defs.h>

#ifdef NGE_PLATFORM_ANDROID
#include <android/ngJNIUtils.h>
#endif

#if ! defined (NGE_PLATFORM_METRO)
#   include <sys/stat.h>
#   include <errno.h>
#endif

ngFileManager* ngFileManager::m_pThis = NULL;
ngFileManager* ngFileManager::GetInstance() {
	if (m_pThis == NULL) {
		m_pThis = DNEW ngFileManager();
	}
	return (ngFileManager*)m_pThis;
}

void ngFileManager::ReleaseInstance(void* pObj)
{
	ngFileManager* pThis = (ngFileManager *)pObj;
	DELETE_OBJECT(pThis, ngFileManager);
}

ngFileManager::ngFileManager()
: ngSingleton(ReleaseInstance, this)
{
#if defined (NGE_PLATFORM_METRO)
    m_pImpl = ngFileManagerImpl::GetInstance();
#endif

}

int32 ngFileManager::MoveFile(NGCSTR from, NGCSTR to)
{
#ifdef NGE_PLATFORM_METRO
    return m_pImpl->MoveFile(from, to);
#else
    ngStringV2 fromPath, toPath;
    ngDevice::GetInstance()->GetFileWritePath(fromPath, from);
    ngDevice::GetInstance()->GetFileWritePath(toPath, to);

	return rename(fromPath.GetCString(), toPath.GetCString());
#endif // NGE_PLATFORM_METRO

}

int32 ngFileManager::MoveLocalFile(NGCSTR from, NGCSTR to) {
    NG_DEBUG_LOG("[ngFileManager] move local file: (%s) => (%s)", from, to);
    /* if from and are on different deive, rename will be failed! */
    /* so use android interface to handle this. */
#if defined (NGE_PLATFORM_ANDROID) && 0
    JNIEnv* pEnv = GetJNIEnv();
    
    static jclass cls = getJavaClass(jclsNGDevice);
    jmethodID mid = pEnv->GetStaticMethodID(cls, "moveFile", "(Ljava/lang/String;Ljava/lang/String;)V");
    
    jstring jstrFrom = stoJstring(from);
    jstring jstrTo = stoJstring(to);
    pEnv->CallStaticVoidMethod(cls, mid, jstrFrom, jstrTo);
    pEnv->DeleteLocalRef(jstrFrom);
    pEnv->DeleteLocalRef(jstrTo);

    return NG_OK;

#else
    return rename(from, to);
#endif
}

int32 ngFileManager::RemoveFile(NGCSTR filename) {
#ifdef NGE_PLATFORM_METRO
    return m_pImpl->RemoveFile(filename);
#else
    ngStringV2 path;
    ngDevice::GetInstance()->GetFileWritePath(path, filename);

    return remove(path);
#endif // NGE_PLATFORM_METRO
    
}

#define FILE_DATA_LOAD_SIZE 4096 //byte

void* ngFileManager::GetFileData(NGCSTR filename, long* pSize) const {
    
#ifdef NGE_PLATFORM_METRO
    return m_pImpl->GetFileData(filename, pSize);
#else
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        /* open file failed!~ */
        NGASSERT(0);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    if (len <= 0) {
        /* get file size failed or file len is 0. */
        return NULL;
    }
    
    long readSize = 0;

    void* pData = DMALLOC(len);
    
    if (pData == NULL) {
        /* alloc memory failed!~ */
        fclose(fp);
        return NULL;
    }

    long pos = 0;
    
    do {
        fseek(fp, pos, SEEK_SET);
        readSize = fread(pData, sizeof(uint8), len, fp);
        fclose(fp);
    } while (0);
    
    if (len != readSize) {
        //check len.
        DFREE(pData);
        return NULL;
    }
    
    if (pSize) {
        *pSize = readSize;
    }
    
    return pData;
#endif // NGE_PLATFORM_METRO
}

int32 ngFileManager::MkDir(NGCSTR dir) {
#ifdef NGE_PLATFORM_METRO
    NGASSERT(0 && "[ngFileManager] no impl on METRO!~")
    return NG_ERROR_IO;
//    return m_pImpl->MkDir(dir);
#else

	ngStringV2 path;
    ngDevice::GetInstance()->GetFileWritePath(path, dir);

    NG_DEBUG_LOG("[ngFileManager] mkdir: %s\n", path.GetCString());

	if (0 == mkdir(path, 0777)) {
        return NG_OK;
    } else {
        NG_DEBUG_LOG("[ngFileManager] mkdir failed, errno: %d", errno);
        if (errno == EEXIST) {
            return NG_OK;
        } else {
            return NG_ERROR_IO;
        }
    }

#endif
}

