/*
 *  ngFileWriter.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-20.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#include <io/ngFileWriter.h>
#if defined (NGE_PLATFORM_METRO)
//#include "ngFileWriteImpl.h" //if have
#pragma message ("[NGE WARNING] some \"write\" interface is not impl for METRO platform. ")
#endif

#if ! defined (NGE_PLATFORM_METRO)
#include <sys/stat.h>
#endif

#include <cocos2d.h>
#include <core/ngDevice.h>


ngFileWriter::ngFileWriter()
{
	m_pFile = NULL;
	m_length = 0;
#ifdef NGE_PLATFORM_METRO
    m_pImpl = ngFileWriterImpl::GetInstance();
#endif
}

ngFileWriter::~ngFileWriter() {
	Close();
}

boolean ngFileWriter::Open(const char* file/*, boolean bCache*/) {

	NG_DEBUG_LOG("[ngFileWriter] open file: %s!~\n", file);
    
#ifdef NGE_PLATFORM_METRO

    boolean ret = m_pImpl->Open(file, bCache);
    m_length = 0;

    return ret;

#else
	ngStringV2 path;
	if (/*bCache == TRUE*/FALSE) {
		ngDevice::GetInstance()->GetFileCachePath(path, file);
	}else{
		ngDevice::GetInstance()->GetFileWritePath(path, file);
	}

	NG_DEBUG_LOG("[ngFileWriter] file path: %s!~\n", path.GetCString());
    
	// open the file
	m_pFile = fopen( path, "wb" );
	m_length = 0;

	if (m_pFile)
		return TRUE;
	else
		return FALSE;
#endif
}

#if 0 //port to cocos2dx
boolean ngFileWriter::MkDir(NGCSTR dir, boolean bCache) {
#ifdef NGE_PLATFORM_METRO
    return m_pImpl->MkDir(dir, bCache);
#else
	ngStringV2 path;
	if (bCache == TRUE) {
		ngDevice::GetInstance()->GetFileCachePath(path, dir);
	}else{
		ngDevice::GetInstance()->GetFileWritePath(path, dir);
	}
    
    NG_DEBUG_LOG("mkdir: %s\n", path.GetCString());
	
	return mkdir(path,0777);
#endif
}
#endif

void ngFileWriter::Close() {
#ifdef NGE_PLATFORM_METRO
    m_pImpl->Close();
#else
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
#endif
}

void ngFileWriter::WriteInt8(int8 data)
{
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);
}

void ngFileWriter::WriteInt32(int32 data)
{
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);
}

void ngFileWriter::WriteInt32BE(int32 data)
{
	int8 a = (int8)((data >> 24) & 0xFF);
	int8 b = (int8)((data >> 16) & 0xFF);
	int8 c = (int8)((data >> 8) & 0xFF);
	int8 d = (int8)(data & 0xFF);
	
	WriteInt8(a);
	WriteInt8(b);
	WriteInt8(c);
	WriteInt8(d);
}

void ngFileWriter::WriteInt64(int64 data)
{
	fwrite(&data, sizeof(data), 1, m_pFile);
	m_length += sizeof(data);
}

void ngFileWriter::WriteInt64BE(int64 data)
{
	int8 a = (int8)((data >> 56) & 0xFF);
    int8 h = (int8)((data >> 48) & 0xFF);
	int8 b = (int8)((data >> 40) & 0xFF);
	int8 c = (int8)((data >> 32) & 0xFF);
	int8 d = (int8)((data >> 24) & 0xFF);
	int8 e = (int8)((data >> 16) & 0xFF);
	int8 f = (int8)((data >> 8) & 0xFF);
	int8 g = (int8)(data & 0xFF);
	
	WriteInt8(a);
    WriteInt8(h);
	WriteInt8(b);
	WriteInt8(c);
	WriteInt8(d);
	WriteInt8(e);
	WriteInt8(f);
	WriteInt8(g);	
}

void ngFileWriter::WriteInt16(int16 data)
{
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);
}

void ngFileWriter::WriteUInt16(uint16 data)
{
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);
}

void ngFileWriter::WriteInt16BE(int16 data)
{

	data = (int16)((data << 8) | (data >> 8));
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);

}

void ngFileWriter::WriteUInt16BE(uint16 data)
{

	data = (uint16)((data << 8) | (data >> 8));
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(&data,sizeof(data),m_length);
#else
	fwrite(&data, sizeof(data), 1, m_pFile);
#endif
	m_length += sizeof(data);

}

void ngFileWriter::WriteUTF8(NGCSTR str)
{
	if (str)
	{
		uint16 len = strlen(str);
		WriteUInt16BE(len);
#ifdef NGE_PLATFORM_METRO
		m_pImpl->WriteData((void*)str,len,m_length);
#else
		fputs(str, m_pFile);
#endif
		m_length += len;
	}
}

void ngFileWriter::WriteLargeUTF8(NGCSTR str)
{
    if (str)
	{
		int32 len = strlen(str);
		WriteInt32BE(len);
#ifdef NGE_PLATFORM_METRO
		m_pImpl->WriteData((void*)str,len,m_length);
#else
		fputs(str, m_pFile);
#endif
		m_length += len;
	}
}

void ngFileWriter::Write(ngByteBuffer* pBuffer)
{
	int32 limit = pBuffer->GetLimit();
	uint8* buffer = pBuffer->GetBuffer();

    Write(buffer, limit);
}

void ngFileWriter::Write(uint8* pData, uint32 nData) {
#ifdef NGE_PLATFORM_METRO
	m_pImpl->WriteData(pData, nData, m_length);
#else
	fwrite(pData, nData, 1, m_pFile);
#endif
	m_length += nData;
}