//
//  ngBit.cpp
//  islands
//
//  Created by 陈 琦 on 12-1-31.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#include <nsl/ngBit.h>
#include <core/NGE_Macros.h>
#include <utils/base64.h>

#if NGBITCOMPRESS
#include "zlib.h"
#endif

#define BYTELEN 8

//ngBit::ngBit(int32 length, uint8 cellLen)
//: m_bitMap(NULL)
//, m_width(0)
//, m_height(0)
//, m_compressed(NULL)
//, m_cellLen(cellLen)
//, m_outString(NULL)
//{
//    Create(length);
//}

ngBit::ngBit(int32 width, int32 height, uint8 cellLen)
: m_bitMap(NULL)
, m_width(width)
, m_height(height)
, m_compressed(NULL)
, m_cellLen(cellLen)
, m_outString(NULL)
{
    Create(width, height);
}

ngBit::~ngBit()
{
    SAFE_DELETE(m_bitMap);
    SAFE_DELETE(m_compressed);
    SAFE_DELETE(m_outString);
}

void ngBit::Set(int32 pos, NGCHAR val)
{
    NGASSERT(m_bitMap);
    
    int32 bitPos = pos * m_cellLen;
    
    int32 byteIndex = bitPos / BYTELEN;
    int32 startPos = bitPos % BYTELEN;
    
    for (int32 i=0; i<m_cellLen; i++) {
        int32 realIndex = byteIndex;
        int32 realPos = startPos + (m_cellLen - i - 1);
        if (realPos >= BYTELEN) {
            realPos -= BYTELEN;
            realIndex += 1;
        }
        realPos = BYTELEN - realPos - 1;
        
        if (GetBit(val, i)) {
            SetBit(m_bitMap[realIndex], realPos);
        } else {
            ReleaseBit(m_bitMap[realIndex], realPos);
        }
    }
}

void ngBit::Release(int32 pos)
{
    NGASSERT(m_bitMap);
    
    int32 bitPos = pos * m_cellLen;
    
    int32 byteIndex = bitPos / BYTELEN;
    int32 startPos = bitPos % BYTELEN;
    
    for (int32 i=0; i<m_cellLen; i++) {
        int32 realIndex = byteIndex;
        int32 realPos = startPos + (m_cellLen - i - 1);
        if (realPos >= BYTELEN) {
            realPos -= BYTELEN;
            realIndex += 1;
        }
        realPos = BYTELEN - realPos - 1;
        ReleaseBit(m_bitMap[realIndex], realPos);
    }
}

NGCHAR ngBit::Get(int32 pos)
{
    NGASSERT(m_bitMap);
    
    NGCHAR ret = 0;
    int32 bitPos = pos * m_cellLen;
    
    int32 byteIndex = bitPos / BYTELEN;
    int32 startPos = bitPos % BYTELEN;
    
    for (int32 i=0; i<m_cellLen; i++) {
        int32 realIndex = byteIndex;
        int32 realPos = startPos + (m_cellLen - i - 1);
        if (realPos >= BYTELEN) {
            realPos -= BYTELEN;
            realIndex += 1;
        }
        realPos = BYTELEN - realPos - 1;
        
        if (GetBit(m_bitMap[realIndex], realPos)) {
            SetBit(ret, i);
        }
    }
    
    return ret;
}

void ngBit::Set(int32 x, int32 y, NGCHAR val)
{
    int32 pos = m_width * y + x;
    Set(pos, val);
}

void ngBit::Release(int32 x, int32 y)
{
    int32 pos = m_width * y + x;
    Release(pos);
}

NGCHAR ngBit::Get(int32 x, int32 y)
{
    int32 pos = m_width * y + x;
    return Get(pos);
}

void ngBit::SetLine(int32 pos, int32 length, NGCHAR val)
{
    for (int32 i=0; i<length; i++) {
        Set(pos + i, val);
    }
}

void ngBit::ReleaseLine(int32 pos, int32 length)
{
    for (int32 i=0; i<length; i++) {
        Release(pos + i);
    }
}

void ngBit::SetLine(int32 x, int32 y, int32 length, NGCHAR val)
{
    int32 pos = m_width * y + x;
    SetLine(pos, length, val);
}

void ngBit::ReleaseLine(int32 x, int32 y, int32 length)
{
    int32 pos = m_width * y + x;
    ReleaseLine(pos, length);
}

void ngBit::SetRect(int32 x, int32 y, int32 width, int32 height, NGCHAR val)
{
    for (int32 i=0; i<height; i++) {
        SetLine(x, y + i, width, val);
    }
}

void ngBit::ReleaseRect(int32 x, int32 y, int32 width, int32 height)
{
    for (int32 i=0; i<height; i++) {
        ReleaseLine(x, y + i, width);
    }
}

void ngBit::ReleaseAll()
{
    for (int32 i=0; i<m_length * m_cellLen; i++) {
        Release(i);
    }
}

void ngBit::SetAll(NGCHAR val)
{
    for (int32 i=0; i<m_length * m_cellLen; i++) {
        Set(i, val);
    }
}

ngStringV2 ngBit::ToString()
{
    int32 len = 0;
    int32 byteLen = (m_length * m_cellLen) / BYTELEN + ((m_length * m_cellLen) % BYTELEN != 0 ? 1 : 0);
    NGSTR str = NewBase64Encode((const void*)m_bitMap, byteLen, FALSE, (size_t *)&len);
    ngStringV2 ret(str);
    SAFE_FREE(str);
    
    return ret;
    
//    ngByteBuffer buf;
//    buf.CopyBuffer((const uint8 *)Compress(), GetCompressedLen());
//    SAFE_DELETE(m_outString);
//    m_outString = ngStringV2::Base64Encode(&buf);
//    return m_outString;
}

void ngBit::GetFromString(ngStringV2 &string)
{
    int32 len = 0;
    void * val = NewBase64Decode(string.GetCString(), string.GetLength(), (size_t *)&len);
    if (m_bitMap) {
        SAFE_DELETE(m_bitMap);
    }
    
    m_length = len * BYTELEN / m_cellLen;
    m_bitMap = DNEW NGCHAR[len];
    memmove(m_bitMap, val, len);
    SAFE_FREE(val);
    
//    ngByteBuffer *pBuf = string.Base64Decode();
//    Decompress((NGCSTR)pBuf->GetBuffer(), pBuf->GetLimit());
}

void ngBit::Append(int32 width, int32 height)
{
    int32 length = width * height;
    if (m_length >= length) return;
    
    int32 bitLen = m_length * m_cellLen;
    
    int32 byteLen = bitLen / BYTELEN + (bitLen % BYTELEN != 0 ? 1 : 0);
    NGCHAR *tmpBit = DNEW NGCHAR[byteLen];
    memset(tmpBit, 0, byteLen);
    memmove(tmpBit, m_bitMap, byteLen);
    
    Create(width, height);
    memmove(m_bitMap, tmpBit, byteLen);
}

#if NGBITCOMPRESS

//Compress & Decompress are learnt from http://www.cocoadev.com/index.pl?NSDataCategory
void* ngBit::Compress()
{
    if (m_length == 0) return m_bitMap;
    
    int32 byteLen = (m_length * m_cellLen) / BYTELEN + ((m_length * m_cellLen) % BYTELEN != 0 ? 1 : 0);
	
	z_stream strm;
    
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.total_out = 0;
	strm.next_in=(Bytef *)m_bitMap;
	strm.avail_in = byteLen;
    
	// Compresssion Levels:
	//   Z_NO_COMPRESSION
	//   Z_BEST_SPEED
	//   Z_BEST_COMPRESSION
	//   Z_DEFAULT_COMPRESSION
    
	if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) != Z_OK) return NULL;
    
    SAFE_DELETE(m_compressed);
    m_compressed = DNEW NGCHAR[byteLen];
    memset(m_compressed, 0, byteLen);
    
	do {
		strm.next_out = (Bytef *)(m_compressed + strm.total_out);
		strm.avail_out = byteLen - strm.total_out;
		
		deflate(&strm, Z_FINISH);  
		
	} while (strm.avail_out == 0);
	
	deflateEnd(&strm);
    
    m_compressLen = strm.total_out;
    
    return m_compressed;
}

void* ngBit::Decompress(NGCSTR buf, int32 len)
{
    if (len == 0) return NULL;
    
	int32 full_length = len;
	int32 half_length = len / 2;
    int32 total_len = full_length + half_length;
    
    NGCHAR *outBuf = (NGCHAR *)malloc(total_len * sizeof(NGCHAR));
    if (outBuf == NULL) return NULL;

	boolean done = FALSE;
	int status;
    
	z_stream strm;
	strm.next_in = (Bytef *)buf;
	strm.avail_in = len;
	strm.total_out = 0;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
    
	if (inflateInit (&strm) != Z_OK) return NULL;
    
	while (!done) {
		// Make sure we have enough room and reset the lengths.
		if (strm.total_out >= total_len) {
            total_len += half_length;
            outBuf = (NGCHAR *)realloc(outBuf, total_len * sizeof(NGCHAR));
        }
			
		strm.next_out = (Bytef *)(outBuf + strm.total_out);
		strm.avail_out = total_len - strm.total_out;
        
		// Inflate another chunk.
		status = inflate (&strm, Z_SYNC_FLUSH);
		if (status == Z_STREAM_END) done = TRUE;
		else if (status != Z_OK) break;
	}
	if (inflateEnd (&strm) != Z_OK) return NULL;
    
	// Set real length.
	if (done) {
        m_bitMap = DNEW NGCHAR[strm.total_out];
        memmove(m_bitMap, outBuf, strm.total_out);
        SAFE_FREE(outBuf);
		return m_bitMap;
	} else { 
        SAFE_FREE(outBuf);
        return NULL;
    }
}

#endif //NGBITCOMPRESS

void ngBit::Create(int32 length)
{
    if (length == 0) return;

    m_length = length;
    int32 bitLen = m_length * m_cellLen;
    
    int32 byteLen = bitLen / BYTELEN + (bitLen % BYTELEN != 0 ? 1 : 0);
    m_bitMap = DNEW NGCHAR[byteLen];
    memset(m_bitMap, 0, byteLen);
}

void ngBit::Create(int32 width, int32 height)
{
    int32 length = width * height;
    Create(length);
}

void ngBit::SetBit(int32 pos)
{
    NGASSERT(m_bitMap);
    
    if (pos >= m_length * m_cellLen) return;
    
    int32 byteIndex = pos / BYTELEN;
    int32 posOf1 = BYTELEN - (pos % BYTELEN) - 1;
    
    NGCHAR setter = 1 << posOf1;
    m_bitMap[byteIndex] |= setter;
}

void ngBit::ReleaseBit(int32 pos)
{
    NGASSERT(m_bitMap);
    
    if (pos >= m_length * m_cellLen) return;
    
    int32 byteIndex = pos / BYTELEN;
    int32 posOf0 = BYTELEN - (pos % BYTELEN) - 1;
    
    NGCHAR setter = ~(1 << posOf0);
    m_bitMap[byteIndex] &= setter;
}

boolean ngBit::GetBit(int32 pos)
{
    NGASSERT(m_bitMap);
    
    if (pos >= m_length * m_cellLen) return FALSE;
    
    int32 byteIndex = pos / BYTELEN;
    int32 bitPos = BYTELEN - (pos % BYTELEN) - 1;
    
    NGCHAR checker = 1 << bitPos;
    
    return m_bitMap[byteIndex] & checker;
}

void ngBit::SetBit(NGCHAR &val, uint8 pos)
{
    if (pos < 0 || pos >= BYTELEN) return;
    
    NGCHAR setter = 1 << pos;
    val |= setter;
}

void ngBit::ReleaseBit(NGCHAR &val, uint8 pos)
{
    if (pos < 0 || pos >= BYTELEN) return;
    
    NGCHAR setter = ~(1 << pos);
    val &= setter;
}

boolean ngBit::GetBit(NGCHAR val, uint8 pos)
{
    if (pos < 0 || pos >= BYTELEN) {
        return FALSE;
    }
    
    NGCHAR checker = 1 << pos;
    return val & checker;
}

void ngBit::DebugValue()
{
#ifdef DEBUG
    NGCHAR ch;
    for (int32 i=0; i<m_length; i++) {
        ch = Get(i);
        NG_DEBUG_LOG("%d: %d", i, ch);
    }
#endif
}
