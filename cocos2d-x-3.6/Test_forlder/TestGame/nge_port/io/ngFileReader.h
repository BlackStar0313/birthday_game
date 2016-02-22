/*
 *  ngFileReader.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-20.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGFILEREADER_H__
#define __NGFILEREADER_H__

#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>
#include <nsl/ngString.h>

#if defined (NGE_PLATFORM_METRO)
class ngFileReaderImpl;
#endif

class ngFileReader
{
public:
	ngFileReader();
	~ngFileReader();
	
    /*!
     @function Load
     @param file file name.
     @abstract load file from package.
     @discussion 
     */
	boolean	Load(const char* file);

#if 0 //port to cocos2dx
    /*!
     @function LoadFromPackage
     @param file file name without any path.
     @abstract load file from package.
     @discussion 
     */
    boolean LoadFromPackage(const char* file);
#endif

	boolean ReadBoolean();
	uint8   ReadUInt8();
	int8    ReadInt8();
	int32	ReadInt32();
	int32	ReadInt32BE();
	int64	ReadInt64();
	int64	ReadInt64BE();
	int16	ReadInt16();
	uint16	ReadUInt16();
	int16	ReadInt16BE();
	uint16	ReadUInt16BE();
	boolean Available();
	
	int32	ReadUTF8(ngStringV2& str);
    int32   ReadLargeUTF8(ngStringV2& str);
    int32   ReadBytes(ngStringV2& str, int32 len);
	
	int8*	GetData() { return m_pData; }
	int64	GetLength() { return m_length; }
    
    boolean AtEnd();

protected:
	int8*	m_pData;
	int8*	m_pointer;
	uint32	m_length;

#if defined (NGE_PLATFORM_METRO)
    ngFileReaderImpl* m_pImpl;
#endif
};

#if (defined NGE_PLATFORM_METRO) || (defined NGE_PLATFORM_ANDROID)
class ngFileReaderImpl {
public:
    static ngFileReaderImpl* GetInstance();
public:
    ngFileReaderImpl() {}
    virtual ~ngFileReaderImpl() {}

protected:
    virtual boolean Load(const char* file, uint32& nData, int8*& pData) = 0;

    friend class ngFileReader;
};

#endif

#endif