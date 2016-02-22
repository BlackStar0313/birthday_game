/*
*  ngInputStream.h
*  NextGenEngine
*
*  Created by cooler on 10-5-19.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGINPUTSTREAM_H__
#define __NGINPUTSTREAM_H__

class ngInputStream
{
public:
	boolean ReadBoolean();
	uint8   ReadUInt8();
	int8    ReadInt8();
	int32	ReadInt32();
	int16	ReadInt16();
	uint16	ReadUInt16();
	int16	ReadInt16BE();
	uint16	ReadUInt16BE();
	boolean Available();

	uint8*	GetData() { return m_pData; }
	uint32	GetLength() { return m_nLength; }

protected:
	uint8* m_pData;
	uint32 m_nLength;
	uint8* m_pOffet;
};

#endif //__NGINPUTSTREAM_H__