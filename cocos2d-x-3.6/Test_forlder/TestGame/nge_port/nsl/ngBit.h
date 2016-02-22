//
//  ngBit.h
//  islands
//
//  Created by 陈 琦 on 12-1-31.
//  Copyright (c) 2012年 Anansi. All rights reserved.
//

#ifndef _NGBIT_H_
#define _NGBIT_H_

#include "../core/NGE_Types.h"
#include "../nsl/ngStringV2.h"


class ngBit 
{
public:
    //ngBit(int32 length, uint8 cellLen);
    ngBit(int32 width, int32 height = 1, uint8 cellLen = 1);
    virtual ~ngBit();
    
    void    Set(int32 pos, NGCHAR val);
    void    Release(int32 pos);
    NGCHAR  Get(int32 pos);
    
//    void    Release(int32 pos);
//    boolean Get(int32 pos);
    
    void    Set(int32 x, int32 y, NGCHAR val);
    void    Release(int32 x, int32 y);
    NGCHAR  Get(int32 x, int32 y);
    
    void    SetLine(int32 pos, int32 length, NGCHAR val);
    void    ReleaseLine(int32 pos, int32 length);
    
    void    SetLine(int32 x, int32 y, int32 length, NGCHAR val);
    void    ReleaseLine(int32 x, int32 y, int32 length);
    
    void    SetRect(int32 x, int32 y, int32 width, int32 height, NGCHAR val);
    void    ReleaseRect(int32 x, int32 y, int32 width, int32 height);
    
    void    ReleaseAll();
    void    SetAll(NGCHAR val);
    
    ngStringV2 ToString();
    void    GetFromString(ngStringV2 &string);
    
#if NGBITCOMPRESS
    void*   Compress();
    void*   Decompress(NGCSTR buf, int32 len);
    int32   GetCompressedLen() { return m_compressLen; }
#endif

    int32   GetLength() { return m_length; }
    void    Append(int32 width, int32 height);
    
    void    DebugValue();
    
private:
    void    Create(int32 length);
    void    Create(int32 width, int32 height);
    void    SetBit(int32 pos);
    void    ReleaseBit(int32 pos);
    boolean GetBit(int32 pos);
    
    void    SetBit(NGCHAR &val, uint8 pos);
    void    ReleaseBit(NGCHAR &val, uint8 pos);
    boolean GetBit(NGCHAR val, uint8 pos);
    
private:
    NGCHAR *m_bitMap;
    NGCHAR *m_compressed;
    
    ngStringV2 *m_outString;
    
    int32 m_width;
    int32 m_height;
    int32 m_length;
    uint8 m_cellLen;
    
#if NGBITCOMPRESS
    int32 m_compressLen;
#endif
};

#endif
