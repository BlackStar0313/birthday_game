/*
 *  asFreeTypeFontCache.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-7-8.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asFreeTypeFontCache.h"
#include <core/NGE_Defs.h>
#include <core/NGE_Macros.h>
#include <core/NGE_Types.h>
#include <extension/freetype/asFreeTypeFont.h>

#include <nsl/ngString.h>
#include <core/ngDevice.h>

asFreeTypeFont* asFreeTypeFontCache::GetFont(NGCSTR fontFile, int32 fontSize) {

    asFreeTypeFont* pNewFont = NULL;
    ngIterator* itr = m_fonts.Iterator();
    while (itr->HasNext()) {
        asFreeTypeFont* pFont = (asFreeTypeFont*) itr->Next();
        if (pFont->m_fontName == fontFile && pFont->m_fontSize == fontSize) {
            pNewFont = pFont;
            break;
        }
    }
    SAFE_DELETE(itr);

    if (pNewFont == NULL) {
        pNewFont = DNEW(asFreeTypeFont);
        pNewFont->m_fontName = fontFile;

        ngString fontFilePath;
        ngDevice::GetInstance()->GetFilePath(fontFilePath, fontFile);
        pNewFont->InitWithFontFile(fontFilePath.GetCString());
        pNewFont->SetFontSize(fontSize);
        m_fonts.Add(pNewFont);
    }

    return pNewFont;
}

void asFreeTypeFontCache::ClearAllFont() {
    LIST_CLEAR(&m_fonts, asFreeTypeFont);
}

void asFreeTypeFontCache::Update() {
    ngIterator* itr = m_fonts.Iterator();
    while (itr->HasNext()) {
        asFreeTypeFont* pFont = (asFreeTypeFont*) itr->Next();
        pFont->ClearUnusedCacheTexture();
    }
    SAFE_DELETE(itr);
}

#pragma mark - ngSingleton

asFreeTypeFontCache* asFreeTypeFontCache::GetInstance() {
    if (m_pThis == NULL) {
        m_pThis = DNEW(asFreeTypeFontCache);
    }
    return m_pThis;
}

void asFreeTypeFontCache::ReleaseInstance(void* pObj) {
    asFreeTypeFontCache* pThis = (asFreeTypeFontCache*) pObj;
    NGASSERT(pThis == m_pThis);
    SAFE_DELETE(pThis);
    m_pThis = NULL;
}

asFreeTypeFontCache::asFreeTypeFontCache()
: ngSingleton(ReleaseInstance, this) {

}

asFreeTypeFontCache::~asFreeTypeFontCache() {

}

asFreeTypeFontCache* asFreeTypeFontCache::m_pThis = NULL;
