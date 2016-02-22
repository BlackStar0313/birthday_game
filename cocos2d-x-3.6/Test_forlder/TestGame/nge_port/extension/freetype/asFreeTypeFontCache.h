/*
 *  asFreeTypeFontCache.h
 *  pnk
 *
 *  Created by Chen Zhen on 14-7-8.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asFreeTypeFontCache_H__
#define __asFreeTypeFontCache_H__

#include <core/ngSingleton.h>
#include <nsl/ngLinkedList.h>

class asFreeTypeFont;

/*!
 @class asFreeTypeFontCache
 @abstract freetype字体缓存
 @discussion 维护一个asFreeTypeFont的列表
 */
class asFreeTypeFontCache
: public ngSingleton {

public:
    /*! 获取字体实例引用，返回的asFreeTypeFont指针不需要释放。 */
    asFreeTypeFont* GetFont(NGCSTR fontFile, int32 fontSize);

public:
    /*! 销毁所有字体实例，必须保证所有通过GetFont()获取到的引用已经无效。 */
    void ClearAllFont();

    /*! 更新接口，需要在主循环中调用。用于清理不再使用的贴图缓存。 */
    void Update();

private:
    ngLinkedList m_fonts;

#pragma mark - ngSingleton

public:
    static asFreeTypeFontCache* GetInstance();
    static void ReleaseInstance(void* pObj);

private:
    asFreeTypeFontCache();
    /*virtual */~asFreeTypeFontCache();

private:
    static asFreeTypeFontCache* m_pThis;
};

#endif //__asFreeTypeFontCache_H__
