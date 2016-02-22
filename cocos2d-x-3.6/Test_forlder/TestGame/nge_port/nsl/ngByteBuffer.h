/*
*  ngByteBuffer.h
*  NextGenEngine
*
*  Created by cooler on 10-5-21.
*  Copyright 2010 DreaminGame. All rights reserved.
*
*/

#ifndef __NGBYTEBUFFER_H__
#define __NGBYTEBUFFER_H__

#include "../core/NGE_Types.h"

#define LOCAL_BUFFER_SIZE (sizeof(uint8*))

class ngByteBuffer
{
public:
	ngByteBuffer();
	virtual ~ngByteBuffer();

public:
	virtual uint8 Get(int32 pos);
	virtual int32 Set(int32 pos, uint8 dat);
	virtual uint8 operator[](int32 pos) { return Get(pos); }

	virtual uint8* GetBuffer() const { return m_pBuffer; }

	virtual void DeleteBuffer();
	virtual int32 AppendBuffer(uint8* src, int32 len);
	virtual int32 CopyBuffer(const uint8* src, int32 len);
	virtual int32 ConsumeBuffer(uint8* dst, int32 len);

	void SetLimit(int32 limit);
	int32 GetLimit(){ return m_nLimit; }

	// be carefull use this function: will reset all of the data.
	void SetCapacity(int32 capacity);
	int32 GetCapacity() { return m_nCapacity; }

private:
	virtual void Init();

protected:
    /*!
     @function CalcCapacity
     @param length 新增的缓冲区需求
     @result 返回足够容纳length的缓冲区大小
     @abstract 计算新的缓冲区大小
     @discussion 如果当前缓冲区大小足够，直接返回当前缓冲区大小(GetCapacity())。
     */
	virtual int32 CalcCapacity(int32 length) const;

protected:
#if 0
	typedef union {
		uint8  localBuffer[LOCAL_BUFFER_SIZE];
		uint8  *buffer;
	} Buffer;
#endif

//	Buffer m_buffer;
	int32 m_nLimit;
	int32 m_nCapacity;
	uint8 *m_pBuffer;
};

#endif //__NGBYTEBUFFER_H__