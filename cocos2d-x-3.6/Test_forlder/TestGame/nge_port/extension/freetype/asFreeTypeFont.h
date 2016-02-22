/*
 *  asFreeTypeFont.h
 *  pnk
 *
 *  Created by Chen Zhen on 14-7-7.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asFreeTypeFont_h__
#define __asFreeTypeFont_h__

#include "../../core/NGE_Macros.h"
#include "../../core/NGE_Defs.h"
#include "../../core/NGE_Types.h"
#include "../../nsl/ngNOHashMap.h"
#include "../../nsl/ngString.h"

typedef struct __T_FREETYPE_FONT_INFO__ FreeTypeFontInfo;

typedef struct __T_FREETYPE_FONT_TEXT_INFO__ {
    typedef struct {
        uint8 r;
        uint8 g;
        uint8 b;
    } Color3B;
    Color3B mStroke;        /* 描边颜色 */
    int32   mStrokeSize;    /* 描边宽度，如果不设置，使用默认描边。 */
    boolean mStrokeEnable;  /* 开启描边 */
    
    Color3B mShadow;        /* 阴影颜色 */
    int32   mShadowX;       /* 阴影偏移x */
    int32   mShadowY;       /* 阴影偏移y */
    int32   mShadowAlpha; /* 阴影透明度 */
    boolean mShadowEnable;  /* 开启阴影 */
    
    float   mXRowSpace;   /* 行距倍数 */
    int32   mRowSpace;    /* 行距 pixels */

    Color3B mTextColor; /* 文本颜色 */

    int32 mWidthLimit;  /* 宽度限制，如果大于0，文本宽度超过时将会换行。 */

    int32 mInputPos; /* [输出]，文本更新后的输入光标位置。 */

    __T_FREETYPE_FONT_TEXT_INFO__();

} FreeTypeFontTextInfo;

/* 直接使用cocos2d::CCTexture2D接口生成的文本贴图。 */
#include <cocos2d.h>

/*!
 @class asFreeTypeFont
 @abstract FreeType字体操作类
 @discussion
 */
class asFreeTypeFont {
public:
    /*!
     通过ttf字体文件创建asFreeTypeFont实例，fontFilePath为字体文件名。
      - 为了使用字体缓存简单，限定字体大小。
      - 返回的asFreeTypeFont不需要释放。(asFreeTypeFontCache::GetInstance()->GetFont())。
     */
    static asFreeTypeFont* FontFromFile(NGCSTR fontFile, int32 fontSize);

//protected:
private:
    asFreeTypeFont();
public:
    virtual ~asFreeTypeFont();

public:
    /*! 判断是否可用。 */
    boolean IsAvailable() const;

    /*! 设置字体大小，pixel */
    void SetFontSize(int32 size);
    
    /*! 设置行距，pixel */
    void SetRowSpace(int32 rowSpace);

    /*! 获取UTF8文本对应的CCTexture2D对象。需要手动retain()。 */
    cocos2d::CCTexture2D* TextureWithUTF8String(NGCSTR str, FreeTypeFontTextInfo* info);

    /*! 清除文本贴图缓存。 */
    void ClearTextureCache();

protected:
    /*! 初始化，从FontFronFile()调用。 */
    void InitWithFontFile(NGCSTR fontFilePath);

    /*! 生成文本贴图 */
    cocos2d::CCTexture2D* TextureWithUTF8StringInternal(NGCSTR str, FreeTypeFontTextInfo* info);
    /*! 生成多行文本贴图 */
    cocos2d::CCTexture2D* TextureWithUTF8StringWithLineBreakInternal(NGCSTR str, FreeTypeFontTextInfo* info);
    
    /*! clear unused cache */
    void ClearUnusedCacheTexture();
    
private:
    bool JudgeIsEnlish(unsigned long world);
    
    bool ContainCharacter(unsigned long world, bool isWorldPrior);
private:
    FreeTypeFontInfo* m_pFTInfo;
    ngString m_fontName;
    int32 m_fontSize;
    int32 m_rowSpace;
    int32 m_realFontSize;

    ngNOHashMap m_textureCache;

    friend class asFreeTypeFontCache;
};

#pragma mark - inline implement

inline boolean asFreeTypeFont::IsAvailable() const {
    return m_pFTInfo != NULL;
}

#endif	//__asFreeTypeFont_h__
