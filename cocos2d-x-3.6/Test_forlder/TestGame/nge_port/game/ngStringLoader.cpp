//
//  ngStringLoader.cpp
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-20.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include <game/ngStringLoader.h>
#include <nsl/ngArrayList.h>
#include <io/ngFileReader.h>

ngStringLoader::ngStringLoader()
: m_stringsNum(0)
, m_pStrings(NULL) {
    
}

ngStringLoader::~ngStringLoader() {
    Free();
}

void ngStringLoader::Free() {
	if (m_stringsNum > 0 && m_pStrings != NULL) {
        //clear list
        ARRAY_LIST_CLEAR((*m_pStrings), ngStringV2);
        //free list
		SAFE_DELETE(m_pStrings);
        //reset num
		m_stringsNum = 0;
	}
}

boolean ngStringLoader::Load(const char* localeStringFileName) {
    
    //protect free before load.
	Free();
    
	ngFileReader reader;
	if (reader.Load(localeStringFileName) == FALSE) {
		return FALSE;
	}
    
	m_stringsNum = reader.ReadInt16BE();
	m_pStrings = DNEW(ngArrayList)(m_stringsNum + 16);
    
	for (int32 i = 0; i < m_stringsNum; i++) {
		ngStringV2* str = DNEW(ngStringV2);
		reader.ReadUTF8(*str);
		str->GetWcs(TRUE);
//		str->GetCString(TRUE);
		m_pStrings->Add(str);
	}
	return TRUE;
}

ngStringV2* ngStringLoader::GetString(int32 index) {
	if (index >= 0 && index < m_stringsNum && m_pStrings != NULL) {
		return (ngStringV2*)(m_pStrings->Get(index));
	} else {
		return NULL;
	}
}

NGCSTR ngStringLoader::GetCString(int32 index) {
	if (index >= 0 && index < m_stringsNum && m_pStrings != NULL) {
		return ((ngStringV2*)m_pStrings->Get(index))->GetCString();
	} else {
		return "wulala";
	}
}
