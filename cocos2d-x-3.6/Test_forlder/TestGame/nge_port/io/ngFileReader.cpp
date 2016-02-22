/*
 *  ngFileReader.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-20.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#include <stdio.h>
#include <io/ngFileReader.h>
#include <core/ngDevice.h>

#ifdef NGE_PLATFORM_ANDROID
#include <android/ngJNIUtils.h>
#include <string.h>
#endif

//#include <io/ngResourcePackageReader.h>

ngFileReader::ngFileReader()
{
	m_pData = NULL;
	m_pointer = NULL;
	m_length = 0;

#if defined (NGE_PLATFORM_METRO)
    m_pImpl = ngFileReaderImpl::GetInstance();
#endif
}

ngFileReader::~ngFileReader()
{
	SAFE_DELETE(m_pData);
}

boolean ngFileReader::Load(const char* file)
{
    
    NG_DEBUG_LOG("[ngFileReader] try load file: %s\n", file);

#ifdef ENABLE_RES_PACKAGE
    if (ngResourcePackageReader::GetInstance()->IsFileExist(file)) {
        return LoadFromPackage(file);
    }
#endif
    
	NG_DEBUG_LOG("[ngFileReader] open normal file: %s\n", file);

#if 0

	JNIEnv* pEnv = GetJNIEnv();

	static jclass cls = getJavaClass(jclsNextGenEngine);
	jmethodID mid = pEnv->GetStaticMethodID(cls, "openFile", "(Ljava/lang/String;)[B");
    jstring jstrFileName = stoJstring(file);
	jbyteArray arr = (jbyteArray)pEnv->CallStaticObjectMethod(cls, mid, jstrFileName);

    pEnv->DeleteLocalRef(jstrFileName);

	if (arr == NULL) {
		return FALSE;
	}

	m_length = pEnv->GetArrayLength(arr);

	if (m_length <= 0) {
		return FALSE;
	}

	jbyte* pBytesArray = pEnv->GetByteArrayElements(arr, 0);
	m_pData = new int8[m_length];
	memcpy(m_pData, pBytesArray, m_length);
	pEnv->ReleaseByteArrayElements(arr, pBytesArray, 0);
	pEnv->DeleteLocalRef(arr);
	m_pointer = m_pData;

	return TRUE;
#endif
    
#if (defined NGE_PLATFORM_METRO) || (defined NGE_PLAYFORM_ANDROID)
    boolean ret = m_pImpl->Load(file, m_length, m_pData);
    m_pointer = m_pData;
    return ret;
#else

    //iOS impl here.

	ngStringV2 path;
	ngDevice::GetInstance()->GetFilePath(path, file);

	// open the file
	FILE * pFile = fopen( path, "rb" );

	if (!pFile)
		return FALSE;

	// get the size of the file
	fseek( pFile, 0, SEEK_END );
	m_length = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	// read in the whole file
	m_pData = new int8[m_length];
	fread( m_pData, m_length, 1 , pFile );

	// close the file
	fclose( pFile );

	m_pointer = m_pData;

	return TRUE;
#endif
}

#if 0 // port to cocos2dx

boolean ngFileReader::LoadFromPackage(const char* file) {
    
    int32 nCode = ngResourcePackageReader::GetInstance()->ReadFile(m_pData, m_length, file);

	m_pointer = m_pData;

    return (nCode == NG_OK);
}

#endif

boolean ngFileReader::Available()
{
	return (m_pointer < (m_pData + m_length));
}

boolean ngFileReader::ReadBoolean()
{
	int8 data = *((int8*)m_pointer);
	m_pointer += 1;
	return (data != 0);
}

uint8 ngFileReader::ReadUInt8()
{
	uint8 data = *((uint8*)m_pointer);
	m_pointer += 1;
	return data;
}

int8 ngFileReader::ReadInt8()
{
	int8 data = *((int8*)m_pointer);
	m_pointer += 1;
	return data;
}

//#if ! defined (DEBUG_LOG)
//#ifdef NGE_PLATFORM_ANDROID
//#include <android/log.h>
//#define NG_DEBUG_LOG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "native_project_lib", fmt, ##args)
//#endif
//#endif
int32 ngFileReader::ReadInt32()
{
#ifdef NGE_PLATFORM_ANDROID
    //int32* pData = (int32*)m_pointer;
    //int32 data = *pData;
    //this is a hotfix, use memcpy() can avoid memory bug below and it looks better than use NG_DEBUG_LOG(""). [zhen.chen]
    int32 data = 0x0;
    memcpy(&data, m_pointer, sizeof(int32));
#else
	int32 data = *((int32*)m_pointer);
#endif
//    NG_DEBUG_LOG("");   //FIXME: this is a cheat to solve a bug(may be memory bug), fix it later. [zhen.chen]
	m_pointer += 4;
	return data;
}

int32 ngFileReader::ReadInt32BE()
{
	int32 a = (ReadInt8() & 0xFF) << 24;
	int32 b = (ReadInt8() & 0xFF) << 16;
	int32 c = (ReadInt8() & 0xFF) << 8;
	int32 d = (ReadInt8() & 0xFF);
	
	int32 data = (a | b | c | d);
	return data;
}

int64 ngFileReader::ReadInt64()
{
#ifdef NGE_PLATFORM_ANDROID
    int64* pData = (int64*)m_pointer;
    int64 data = *pData;
#else
	int64 data = *((int64*)m_pointer);
#endif
	m_pointer += 8;
	return data;
}

int64 ngFileReader::ReadInt64BE()
{
	int64 a = (int64)(ReadInt8() & 0xFF) << 56;
    int64 h = (int64)(ReadInt8() & 0xFF) << 48;
	int64 b = (int64)(ReadInt8() & 0xFF) << 40;
	int64 c = (int64)(ReadInt8() & 0xFF) << 32;
	int64 d = (int64)(ReadInt8() & 0xFF) << 24;
	int64 e = (int64)(ReadInt8() & 0xFF) << 16;
	int64 f = (int64)(ReadInt8() & 0xFF) << 8;
	int64 g = (int64)(ReadInt8() & 0xFF);
	
	int64 data = (a | h | b | c | d | e | f | g);
	return data;
}

int16 ngFileReader::ReadInt16()
{
#ifdef NGE_PLATFORM_ANDROID
    int16* pData = (int16*)m_pointer;
	int16 data = *pData;
#else
	int16 data = *((int16*)m_pointer);
#endif
	m_pointer += 2;
	return data;
}

uint16 ngFileReader::ReadUInt16()
{
#ifdef NGE_PLATFORM_ANDROID
    uint16* pData = (uint16*)m_pointer;
	uint16 data = *pData;
#else
	uint16 data = *((int16*)m_pointer);
#endif
	m_pointer += 2;
	return data;
}

int16 ngFileReader::ReadInt16BE()
{
#ifdef NGE_PLATFORM_ANDROID
    uint16* pData = (uint16*)m_pointer;
	uint16 data = *pData;
#else
	uint16 data = *((int16*)m_pointer);
#endif
	m_pointer += 2;
	return (int16)((data << 8) | (data >> 8));
}

uint16 ngFileReader::ReadUInt16BE()
{
#ifdef NGE_PLATFORM_ANDROID
    uint16* pData = (uint16*)m_pointer;
	uint16 data = *pData;
#else
	uint16 data = *((int16*)m_pointer);
#endif
	m_pointer += 2;
	return (uint16)((data << 8) | (data >> 8));
}

int32 ngFileReader::ReadUTF8(ngStringV2& str)
{
	uint16 len = ReadInt16BE();
	str.AssignString((NGCSTR)m_pointer, len);
//	char* buffer = (char *)DMALLOC(sizeof(char) * (len + 1));
//	memcpy(buffer, m_pointer, len);
	m_pointer += len;
//	buffer[len] = '\0';
//	str = buffer;
//	DFREE(buffer);
	return len;
}

int32 ngFileReader::ReadLargeUTF8(ngStringV2& str)
{
    int32 len = ReadInt32BE();
	str.AssignString((NGCSTR)m_pointer, len);
	m_pointer += len;
	return len;
}

int32 ngFileReader::ReadBytes(ngStringV2& str, int32 len)
{
	str.AssignString((NGCSTR)m_pointer, len);
	m_pointer += len;

	return len;
}

boolean ngFileReader::AtEnd() {
    return (m_pData + m_length) <= m_pointer;
}
