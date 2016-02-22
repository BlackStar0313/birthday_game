/*
 *  ngFileWriter.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-20.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGFILEWRITER_H__
#define __NGFILEWRITER_H__

#include <stdio.h>
#include <core/NGE_Types.h>
#include <nsl/ngString.h>

#ifdef NGE_PLATFORM_METRO
class ngFileWriterImpl;
#endif // NGE_PLATFORM_METRO


class ngFileWriter
{
public:
	ngFileWriter();
	~ngFileWriter();
	
	boolean	Open(const char* file/*, boolean bCache = FALSE*/);
#if 0 //port to cocos2dx
	boolean MkDir(NGCSTR dir/*, boolean bCache = FALSE*/);
#endif
    uint32  GetFileSize() { return m_length; }
	void	Close();
	
	void    WriteInt8(int8 data);
	void	WriteInt64(int64 data);
	void	WriteInt64BE(int64 data);
	void	WriteInt32(int32 data);
	void	WriteInt32BE(int32 data);
	void	WriteInt16(int16 data);
	void	WriteUInt16(uint16 data);
	void	WriteInt16BE(int16 data);
	void	WriteUInt16BE(uint16 data);
	void	Write(ngByteBuffer* pBuffer);
	void	Write(uint8* pData, uint32 nData);
	
	void	WriteUTF8(NGCSTR str);
    void    WriteLargeUTF8(NGCSTR str);

protected:
	FILE*	m_pFile;
	uint32	m_length;

#ifdef NGE_PLATFORM_METRO

    ngFileWriterImpl* m_pImpl;

#endif // NGE_PLATFORM_METRO

};

#if defined (NGE_PLATFORM_METRO)
//temp declare impl here, now just for metro. [zhen.chen]
#pragma mark - ngFileWriterImpl
class ngFileWriterImpl {
public:
    ngFileWriterImpl() {}
    virtual ~ngFileWriterImpl() {}
public:
    static ngFileWriterImpl* GetInstance();

    virtual boolean	Open(const char* file, boolean bCache) = 0;
    virtual boolean MkDir(NGCSTR dir, boolean bCache) = 0;
	virtual void Close() = 0;
    virtual uint32 WriteData(void* pData, int32 length, int32 offset) = 0;
};

#endif

#endif