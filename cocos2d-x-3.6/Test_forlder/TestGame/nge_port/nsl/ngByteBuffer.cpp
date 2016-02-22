#include <nsl/ngByteBuffer.h>
#include <math/ngMath.h>
#include <core/NGE_Macros.h>
#include <core/NGE_Defs.h>

#define MAX_ENLARGE_BYTES 1024

#define NG_BYTE_SIZE(_len) ((_len) * sizeof(uint8))

ngByteBuffer::ngByteBuffer()
{
	Init();
}

ngByteBuffer::~ngByteBuffer()
{
	DeleteBuffer();
}

void ngByteBuffer::Init()
{
	m_nCapacity = 0;
	m_nLimit = 0;
//	m_pBuffer = (uint8 *)DMALLOC(4 * sizeof(uint8));
	m_pBuffer = NULL;
}

uint8 ngByteBuffer::Get(int32 pos)
{
	NGASSERT(pos < m_nLimit);
	if (pos < m_nLimit) {
		return m_pBuffer[pos];
	}
	return 0;
}

int32 ngByteBuffer::Set(int32 pos, uint8 dat)
{
	if (pos < m_nLimit) {
		m_pBuffer[pos] = dat;
		return NG_OK;
	}
	return NG_ERROR_FAILTURE;
}

void ngByteBuffer::SetLimit(int32 limit) 
{
	NGASSERT( limit <= m_nCapacity );
	if (limit <= m_nCapacity) {
		m_nLimit = limit;
	}
}

void ngByteBuffer::SetCapacity(int32 capacity) {
	int32 newCapacity = CalcCapacity(capacity);
	DeleteBuffer();
	m_pBuffer = (uint8 *)DMALLOC(NG_BYTE_SIZE(newCapacity));
	m_nCapacity = newCapacity;
    
    DMEMSET(m_pBuffer, 0x0, NG_BYTE_SIZE(m_nCapacity));
}

int32 ngByteBuffer::CalcCapacity(int32 length) const {

	int32 newCapacity = m_nCapacity;

    if (length >= m_nCapacity) {
		if (newCapacity < MAX_ENLARGE_BYTES) {
			newCapacity = newCapacity * 2 + 8;
		}else{
			newCapacity += MAX_ENLARGE_BYTES;
		}
		if (newCapacity < length) {
			newCapacity = ((length + 3) / 4) * 4 + 8;
		}
	}

	return newCapacity;
}

void ngByteBuffer::DeleteBuffer()
{
	if (m_nCapacity > LOCAL_BUFFER_SIZE) {
		//DFREE(m_buffer.buffer);
		DFREE(m_pBuffer);
	}
	Init();
}

#ifdef WP8
//test use memset here
#ifndef ZeroMemory
#define ZeroMemory(ptr, len) memset((ptr), 0x0, (len));
#endif
#endif // WP8

int32 ngByteBuffer::AppendBuffer(uint8* src, int32 len) {
    int32 newCapacity = CalcCapacity(m_nLimit + len);

	if (m_nCapacity < newCapacity) {

		int32 limit = m_nLimit;
		uint8 *pOriginal = (uint8 *)DMALLOC(NG_BYTE_SIZE(newCapacity));
		DMEMCPY(pOriginal, m_pBuffer, NG_BYTE_SIZE(m_nLimit));

		DeleteBuffer();
		m_pBuffer = pOriginal;
		m_nCapacity = newCapacity;
		m_nLimit = limit;

	}

	DMEMCPY(m_pBuffer + m_nLimit, src, len);
    DMEMSET(m_pBuffer + m_nLimit + len, 0x0, m_nCapacity - (m_nLimit + len));   //'\0' terminal.

	m_nLimit = m_nLimit + len;
    
	return NG_OK;
}

int32 ngByteBuffer::CopyBuffer(const uint8* src, int32 len) {

    int32 newCapacity = CalcCapacity(len);

	if (m_nCapacity < newCapacity) {

		DeleteBuffer();

		m_pBuffer = (uint8 *)DMALLOC(NG_BYTE_SIZE(newCapacity));
		m_nCapacity = newCapacity;
	}

	DMEMCPY(m_pBuffer, src, len);
    DMEMSET(m_pBuffer + len, 0x0, NG_BYTE_SIZE(m_nCapacity - len));   //'\0' terminal.

	m_nLimit = len;
    
	return NG_OK;
}

int32 ngByteBuffer::ConsumeBuffer(uint8* dst, int32 len)
{
	int32 nConsumed = ngMath::Min(len, m_nLimit);
	if (dst != NULL && nConsumed > 0) {
		DMEMCPY(dst, m_pBuffer, nConsumed);
	}
	m_nLimit = m_nLimit - nConsumed;
	if (m_nLimit > 0) {
		DMEMCPY(m_pBuffer, m_pBuffer + nConsumed, m_nLimit);
	}
	return nConsumed;
}