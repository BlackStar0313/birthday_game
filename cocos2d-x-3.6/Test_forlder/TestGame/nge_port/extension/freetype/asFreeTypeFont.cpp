/*
 *  asFreeTypeFont.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-7-7.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include <extension/freetype/asFreeTypeFont.h>
#include <extension/freetype/asFreeTypeFontCache.h>

#include <core/NGE_Macros.h>

#include <nsl/ngHashMapIterator.h>
#include <utils/unicode.h>
#include <nsl/ngArrayList.h>

#include <precompile_lib/freetype/include/ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <nsl/ngNOHashMap.h>


#define isbefore(c) (ContainCharacter(c,true))
#define isafter(c) (ContainCharacter(c,false))

/* 用于保存一个字体名称的对应表，如果不为NULL，尝试在加载字体时使用value保存的字体名替换key保存的字体名。 */
ngNOHashMap* g_freeTypeFontReplaceRule = NULL;

#pragma mark - FreeTypeFontInfo
typedef struct __T_FREETYPE_FONT_INFO__ {
    FT_Library mFTLibrary;
    FT_Face mFTFace;


    __T_FREETYPE_FONT_INFO__()
    : mFTLibrary(NULL)
    , mFTFace(NULL) {

    }

    ~__T_FREETYPE_FONT_INFO__() {
        FT_Done_Face(mFTFace);
        FT_Done_FreeType(mFTLibrary);
    }
} FreeTypeFontInfo;

#pragma mark - asFreeTypeFont

static const int32 sDefaultDPI = 72;
static const int32 sDefaultFontSize = 24;
static const int32 sDefaultTextureMaxWidth = 4096; /* 文本贴图的最大宽度。暂时hardcode个2048。[zhen.chen] */
static const int32 sDefaultTextureMaxHeight = 128; /* 文本贴图的最大高度。暂时hardcode个128。[zhen.chen] */

/* 由于目前游戏设计通用以640x960(640x1136)为基准，所以认为多行文本宽度不超过640，高度不超过960。设定1024x1024的贴图足够。 */
static const int32 sDefaultTextureWithLineBreakMaxWidth     = 1024; /* 文本贴图的最大宽度。暂时hardcode个1024。[zhen.chen] */
static const int32 sDefaultTextureWithLineBreakMaxHeight    = 1024; /* 文本贴图的最大高度。暂时hardcode个1024。[zhen.chen] */

asFreeTypeFont* asFreeTypeFont::FontFromFile(NGCSTR fontFile, int32 fontSize) {

    if (g_freeTypeFontReplaceRule) {
        ngString* strFontFile = (ngString*) g_freeTypeFontReplaceRule->Get(fontFile);
        if (strFontFile) {
            /* 如果存在替换字体，则载入替换字体。 */
            return asFreeTypeFontCache::GetInstance()->GetFont(strFontFile->GetCString(), fontSize);
        }
    }

    return asFreeTypeFontCache::GetInstance()->GetFont(fontFile, fontSize);
}

asFreeTypeFont::asFreeTypeFont()
: m_pFTInfo(NULL)
, m_rowSpace(0)
, m_fontSize(sDefaultFontSize) {

}

asFreeTypeFont::~asFreeTypeFont() {
    SAFE_DELETE(m_pFTInfo);
    ClearTextureCache();
}

void asFreeTypeFont::InitWithFontFile(NGCSTR fontFilePath) {

    SAFE_DELETE(m_pFTInfo);
    m_pFTInfo = DNEW(FreeTypeFontInfo);

    do {
        FT_Error error = FT_Init_FreeType(&m_pFTInfo->mFTLibrary);
        if (error) {
            NG_DEBUG_LOG("[FreeType] errCode %d", error);
            break;
        }

        error = FT_New_Face(m_pFTInfo->mFTLibrary,
                            fontFilePath,
                            0,
                            &m_pFTInfo->mFTFace);
        if (error == FT_Err_Unknown_File_Format)
        {
            //... the font file could be opened and read, but it appears
            //... that its font format is unsupported
            NG_DEBUG_LOG("[FreeType] unknown font format, file: %s", fontFilePath);
            break;
        } else if (error) {
            //... another error code means that the font file could not
            //... be opened or read, or simply that it is broken...
            NG_DEBUG_LOG("[FreeType] load font face failed, file: %s", fontFilePath);
            break;
        }

        return; /* success, return. */
    } while (0);

    /* error */
    SAFE_DELETE(m_pFTInfo);
}

void asFreeTypeFont::SetFontSize(int32 size) {

    m_fontSize = size;

    if (!IsAvailable()) {
        //NGASSERT(IsAvailable() && "font is not available!~");
        return;
    }
    
#if 1
    FT_Error error = FT_Set_Char_Size(m_pFTInfo->mFTFace, 0, size << 6, sDefaultDPI, sDefaultDPI);
#else
    FT_Error error = FT_Set_Pixel_Sizes(m_pFTInfo->mFTFace, 0, size);
#endif
    if (error) {
        NG_DEBUG_LOG("[FreeType] set font size failed!~");
        return;
    }

#if 0
    m_realFontSize = m_pFTInfo->mFTFace->size->metrics.height >> 6;
#else

    /*
     
     m_pFTInfo->mFTFace->size->metrics.height获取到的字体高度比较高，下面两行代码用于得到一个比较合适的高度。

     出处不详。
     http://www.freetype.org/freetype2/docs/tutorial/step2.html 官方文档的解释是.height的到的高度大于实际高度(ascent+descent)，但是不保证够高。
     
     [zhen.chen]
     
     */
    const int32 ascender = m_pFTInfo->mFTFace->size->metrics.ascender >> 6;   /* 基线到字符轮廓最高点的距离 */
    const int32 descender = m_pFTInfo->mFTFace->size->metrics.descender >> 6;   /* 基线到字符轮廓最低点的距离 */
    m_realFontSize = ((m_pFTInfo->mFTFace->size->metrics.height >> 6) + ascender - descender) / 2 + 3; /* +3 防止下方切边 */

#endif
    ClearTextureCache();
}

void asFreeTypeFont::SetRowSpace(int32 rowSpace)
{
    m_rowSpace = rowSpace;
}

void asFreeTypeFont::ClearTextureCache() {
    ngHashMapIterator* itr = (ngHashMapIterator*) m_textureCache.Iterator();
    while (itr->HasNext()) {
        ngMap::ngEntry* pEntry = itr->NextEntry();
        cocos2d::CCTexture2D* pTexture = (cocos2d::CCTexture2D*) pEntry->GetValue();
        SAFE_DELETE(pTexture);
    }
    SAFE_DELETE(itr);
    m_textureCache.RemoveAll();
}

/*! 生成图像数据时的回调数据结构，用于直接往内存中写入颜色数据。 */
typedef struct __T_RASTERCALLBACK_DATA__ {
    uint8* mBuffer;
    int32 mBufferWidth; /* 缓冲区宽度(这里把缓冲区看作一个二维数组)。 */
    int32 mBufferHeight;
    int32 mOffsetX;
    int32 mOffsetY; /* baseline. */
    uint8 mColorR;
    uint8 mColorG;
    uint8 mColorB;
    uint8 mAlpha;
} RasterCallbackData;

/*! freetype渲染回调。 */
static void RasterCallback(const int y,
                           const int count,
                           const FT_Span * const spans,
                           void * const user) {
    RasterCallbackData *data = (RasterCallbackData *)user;

    if (data->mOffsetY + (-y) >= data->mBufferHeight) {
        return; /* 超出缓冲区的部分，忽略。[zhen.chen] */
    }

    for (int32 i = 0; i < count; ++i) {
        //this log show how this data desc a character.
//        NG_DEBUG_LOG("[span] x: %d, len: %d, y: %d, coverage: %d", spans[i].x, spans[i].len, y, spans[i].coverage);

        for (int32 x = 0; x < spans[i].len; ++x) {
            uint8* pixel = data->mBuffer + (data->mBufferWidth * (data->mOffsetY - y) + data->mOffsetX + spans[i].x + x) * sizeof(uint32);
#if 0
            *(pixel + 0) = spans[i].coverage * data->mColorR / 255;
            *(pixel + 1) = spans[i].coverage * data->mColorG / 255;
            *(pixel + 2) = spans[i].coverage * data->mColorB / 255;
            *(pixel + 3) = spans[i].coverage;
#else
            /* 使用混色，优化描边字的边缘处理。 */
            *(pixel + 0) = spans[i].coverage * data->mColorR / 255 + (255 - spans[i].coverage) * (*(pixel + 0)) / 255;
            *(pixel + 1) = spans[i].coverage * data->mColorG / 255 + (255 - spans[i].coverage) * (*(pixel + 1)) / 255;
            *(pixel + 2) = spans[i].coverage * data->mColorB / 255 + (255 - spans[i].coverage) * (*(pixel + 2)) / 255;
            *(pixel + 3) = spans[i].coverage * data->mAlpha / 255 + (255 - spans[i].coverage) * (*(pixel + 3)) / 255;
#endif
        }
    }
}

/*! 使用freetype渲染一个字的outline，调用RasterCallback将生成的颜色数据写入data。 */
static void RenderSpans(FT_Library &library,
                        FT_Outline * const outline,
                        RasterCallbackData* data) {
    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user = data;

    FT_Outline_Render(library, outline, &params);
}

cocos2d::CCTexture2D* asFreeTypeFont::TextureWithUTF8String(NGCSTR str, FreeTypeFontTextInfo* info) {

    if (m_realFontSize <= 0) {
        return NULL;
    }

    if (str == NULL || strlen(str) <= 0) {
        return NULL;
    }

    if (info == NULL) {
        return NULL;
    }

    NGASSERT(IsAvailable() && "font is not available!~");
    
    m_rowSpace = info->mRowSpace;

    size_t encode_len = 0;
    char* encode_info = NewBase64Encode(info, sizeof(*info), FALSE, &encode_len);
    ngString strInfoKey;
    strInfoKey.Format("%s|%s", str, encode_info);
    SAFE_FREE(encode_info);

    cocos2d::CCTexture2D* pTex = (cocos2d::CCTexture2D*) m_textureCache.Get(strInfoKey);
    if (pTex == NULL) {
        if (info->mWidthLimit > 0) {
            pTex = TextureWithUTF8StringWithLineBreakInternal(str, info);
        } else {
            pTex = TextureWithUTF8StringInternal(str, info);
        }
        if (pTex == NULL) {
            NGASSERT(FALSE);
        }
        m_textureCache.Put(strInfoKey, pTex);
    } else {
        /* 更新光标位置，这里在多行文本时似乎不是合适的做法。稍后处理。[zhen.chen] */
        info->mInputPos = pTex->getContentSizeInPixels().width;
    }

    return pTex;
}

#define GENERATE_STROKE_TEXT_WITH_TWO_FOR (1)

#define CACHE_TEMP_BUFFER (0)

cocos2d::CCTexture2D* asFreeTypeFont::TextureWithUTF8StringInternal(NGCSTR str, FreeTypeFontTextInfo* info) {

    NGASSERT(m_realFontSize > 0);
    NGASSERT(str && strlen(str) > 0);
    NGASSERT(info != NULL);

    FT_Error err = 0;

    int32 texWidth = sDefaultTextureMaxWidth; /*! FIXME: 可以先计算出来当前文本长度后再进行内存分配，能节省不少 [ruix] */
    int32 texHeight = m_realFontSize;
    int32 textWidth = 0;
    int32 baseline = m_pFTInfo->mFTFace->size->metrics.ascender >> 6;

    const int32 strokeWidth = info->mStrokeSize != -1 ? info->mStrokeSize : (m_realFontSize >= 32 ? 4 : 2);

    if (info->mStrokeEnable) {
        /* 描边 */
        //texWidth += strokeWidth;
        texHeight += strokeWidth;

        textWidth += strokeWidth;
        baseline += strokeWidth;
    }
    
    if (info->mShadowEnable) {
        /* 阴影 */
        textWidth += info->mShadowX;
        texHeight += info->mShadowY;
    }

    /*
     内存保护，似乎某些字形超出了一点儿。
     比如: "BDZYJT.ttf"中的 65288 "（"
     也有其他字体有这个问题，暂时改成+2保险一点儿吧。
     [zhen.chen]
     */
    //texHeight += 2;

#if CACHE_TEMP_BUFFER
    static uint8* pData = (uint8*) DMALLOC(sDefaultTextureMaxWidth * sDefaultTextureMaxHeight * sizeof(uint32));
    memset(pData, 0x0, sDefaultTextureMaxWidth * sDefaultTextureMaxHeight * sizeof(uint32));
#else
    uint8* pData = (uint8*) DMALLOC(texWidth * texHeight * sizeof(uint32));
    memset(pData, 0x0, texWidth * texHeight * sizeof(uint32));
#endif

    size_t len = 0;
    uint16* unicode_text = utf8_decode(str, &len);
    if (len == 0) {
        return NULL;
    }

    FT_Stroker stroker;
    FT_Glyph strokeGlyph;
    if (info->mStrokeEnable) {
        FT_Stroker_New(m_pFTInfo->mFTLibrary, &stroker);
        FT_Stroker_Set(stroker, strokeWidth << 6, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    }

    RasterCallbackData callback_data;
    callback_data.mBuffer = pData;
    callback_data.mOffsetY = baseline;
    callback_data.mBufferWidth = sDefaultTextureMaxWidth;
    callback_data.mBufferHeight = texHeight;
    const int32 textStart = textWidth; /* 保存文本开始位置。 */

    /* 1. 描边 */
    textWidth = 0;
    if (info->mStrokeEnable) {
        for (int32 i = 0; i < len; i++) {
            FT_GlyphSlot slot = m_pFTInfo->mFTFace->glyph;
            FT_UInt idx = FT_Get_Char_Index(m_pFTInfo->mFTFace, unicode_text[i]);
            if (idx == 0) {
                /* 没有发现对应的字 */
                /* TODO */
                continue;
            } else {
                err = FT_Load_Glyph(m_pFTInfo->mFTFace, idx, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
                NGASSERT(slot->format == FT_GLYPH_FORMAT_OUTLINE);
                if (err) {
                    NG_DEBUG_LOG("[FreeType] load char '%c' (at index %d of %s) failed!~", str[i], i, str);
                    /* TODO */
                    continue;
                }

                callback_data.mOffsetX = textWidth;

                if (0 == FT_Get_Glyph(slot, &strokeGlyph)) {
                    FT_Glyph_StrokeBorder(&strokeGlyph, stroker, FALSE, TRUE);
                    // Again, this needs to be an outline to work.
                    if (strokeGlyph->format == FT_GLYPH_FORMAT_OUTLINE) {
                        // Render the outline spans to the span list
                        FT_Outline *o =
                        &reinterpret_cast<FT_OutlineGlyph>(strokeGlyph)->outline;

                        callback_data.mColorR = info->mStroke.r;
                        callback_data.mColorG = info->mStroke.g;
                        callback_data.mColorB = info->mStroke.b;
                        callback_data.mAlpha = 255;
                        RenderSpans(m_pFTInfo->mFTLibrary, o, &callback_data);
                    }
                    FT_Done_Glyph(strokeGlyph);
                }

                textWidth += slot->advance.x >> 6;
            }
        }
    
        FT_Stroker_Done(stroker);
    }

    textWidth = textStart; /* 使用textStart重置textWidth，用于生成无描边字数据。 */
    /* 2. 无描边 */
    for (int32 i = 0; i < len; i++) {
        FT_GlyphSlot slot = m_pFTInfo->mFTFace->glyph;
        FT_UInt idx = FT_Get_Char_Index(m_pFTInfo->mFTFace, unicode_text[i]);
        if (idx == 0) {
            /* 没有发现对应的字 */
            /* TODO */
            continue;
        } else {
            err = FT_Load_Glyph(m_pFTInfo->mFTFace, idx, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
            NGASSERT(slot->format == FT_GLYPH_FORMAT_OUTLINE);
            if (err) {
                NG_DEBUG_LOG("[FreeType] load char '%c' (at index %d of %s) failed!~", str[i], i, str);
                /* TODO */
                continue;
            }
        
            
            /*! 处理实际宽度同间隔宽度不同的特殊情况, 例如字符 [ \/j ][ruix] */
            if (i==0 && slot->metrics.width > slot->advance.x) {
                textWidth += ceil((slot->metrics.width - slot->advance.x)>>6);
            }

            callback_data.mOffsetX = textWidth;
            
            /* 2. 阴影处理 */
            if (info->mShadowEnable) {
                
                int32 oriOffsetX = callback_data.mOffsetX;
                int32 oriOffsetY = callback_data.mOffsetY;
                
                callback_data.mOffsetX = oriOffsetX + info->mShadowX;
                callback_data.mOffsetY = oriOffsetY + info->mShadowY;
                
                callback_data.mColorR = info->mShadow.r;
                callback_data.mColorG = info->mShadow.g;
                callback_data.mColorB = info->mShadow.b;
                callback_data.mAlpha = info->mShadowAlpha;
                RenderSpans(m_pFTInfo->mFTLibrary, &slot->outline, &callback_data);
                
                callback_data.mOffsetX = oriOffsetX;
                callback_data.mOffsetY = oriOffsetY;
            }

            callback_data.mColorR = info->mTextColor.r;
            callback_data.mColorG = info->mTextColor.g;
            callback_data.mColorB = info->mTextColor.b;
            callback_data.mAlpha = 255;
            RenderSpans(m_pFTInfo->mFTLibrary, &slot->outline, &callback_data);

            textWidth += slot->advance.x >> 6;
            
            /*! 处理实际宽度同间隔宽度不同的特殊情况, 例如字符 [ \/j ][ruix] */
            if ((i==len-1)&& slot->metrics.width > slot->advance.x) {
                textWidth += ceil((slot->metrics.width - slot->advance.x)>>6);
            }
        }
    }

    SAFE_FREE(unicode_text);
    
    if (info->mStrokeEnable) {
        /* 描边 */
        textWidth += strokeWidth; /*! 再次增加结尾处描边预留空间 */
    }

#if 1

    /* 缩小图片大小到正好容纳图片 */
    const int32 nTextDataWidth = textWidth * sizeof(uint32);
    uint8* pTextData = (uint8*) DMALLOC(nTextDataWidth * texHeight);
    for (int32 i = 0; i < texHeight; ++i) {
        memcpy(pTextData + nTextDataWidth * i, pData + (texWidth) * sizeof(uint32)* i, nTextDataWidth);
    }

#   if ! CACHE_TEMP_BUFFER
    SAFE_FREE(pData);
#   endif

    cocos2d::CCTexture2D* pTex = DNEW(cocos2d::CCTexture2D);
    

    pTex->initWithData(pTextData, cocos2d::kTexture2DPixelFormat_RGBA8888, (textWidth), texHeight, cocos2d::CCSizeMake(textWidth , texHeight));
    SAFE_FREE(pTextData);

#endif

    /* 更新光标位置 */
    info->mInputPos = textWidth + strokeWidth;

    return pTex;

}

cocos2d::CCTexture2D* asFreeTypeFont::TextureWithUTF8StringWithLineBreakInternal(NGCSTR str, FreeTypeFontTextInfo* info) {

    NGASSERT(m_realFontSize > 0);
    NGASSERT(str && strlen(str) > 0);
    NGASSERT(info != NULL);
    NGASSERT(info->mWidthLimit > 0);

    FT_Error err = 0;

    size_t len = 0;
    uint16* unicode_text = utf8_decode(str, &len);
    if (len == 0) {
        return NULL;
    }
    
    std::string curText = str;

    const int32 strokeWidth = info->mStrokeSize != -1 ? info->mStrokeSize : (m_realFontSize >= 32 ? 4 : 2);

    FT_Stroker stroker;
    FT_Glyph strokeGlyph;
    if (info->mStrokeEnable) {
        FT_Stroker_New(m_pFTInfo->mFTLibrary, &stroker);
        FT_Stroker_Set(stroker, strokeWidth << 6, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    }

    /* 先遍历一次，确定换行位置，用于计算最终的文本贴图大小。(FIXME: 也可以考虑和渲染合并在一起。稍后优化。[zhen.chen]) */
    int32 lineWidth = 0;
    if (info->mStrokeEnable) {
        lineWidth += strokeWidth;
    }
    
    int32 rememberPos = -1; /* 用来记录上一个单词的首字母位置,如果是-1，在下一次遍历的时候就需更新位置 */
    int32 everyWordWidth = 0;   /* 遍历中记录每一个单词当前字母之前的所有字母的图形宽度 */
    bool isAllEnglishWorld = true;  /* 处理中英文分词情况，如果两个空格之间全都是a-z的话就是一个完整的单词，有其他的字符出现就不去回溯到前面，直接在长度位置切掉 */
    
    ngArrayList linebreaks;
    for (int32 i = 0; i < len; i++) {
        FT_GlyphSlot slot = m_pFTInfo->mFTFace->glyph;
        FT_UInt idx = FT_Get_Char_Index(m_pFTInfo->mFTFace, unicode_text[i]);
        if (idx == 0) {
            /* 没有发现对应的字 */
            /* TODO */
            continue;
        } else {
            err = FT_Load_Glyph(m_pFTInfo->mFTFace, idx, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
            if (err) {
                NG_DEBUG_LOG("[FreeType] load char '%c' (at index %d of %s) failed!~", str[i], i, str);
                /* TODO */
                continue;
            }

            int32 word_width = slot->advance.x >> 6;

            /* 增加英文分词 */    /* TODO:增加其他语言的处理 */
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            everyWordWidth += word_width;
            int32 curPos = i;
            
            if (rememberPos == -1) {
                rememberPos = curPos;
            }
            
            FT_ULong curChar = unicode_text[curPos];
            
            if (!JudgeIsEnlish(curChar)) {
                isAllEnglishWorld = false;
            }
            
            if (curChar == 0x20 || isbefore(unicode_text[curPos]) || isafter(unicode_text[curPos])) {      /* 空格 标点符号 unicode编码 0x20 下一段文字的开始*/
                rememberPos = -1;
                everyWordWidth = 0;
                isAllEnglishWorld = true;
            }
            
            if (info->mWidthLimit < (lineWidth + word_width) ) {
                curPos = rememberPos;
                
                if (curPos != 0 && isAllEnglishWorld) {
                    /* 换行 */
                    linebreaks.Add((void*) (rememberPos - 1)); /* ruix: br should be at pos i - 1, and new line should contains current char */
                    lineWidth = word_width + everyWordWidth;
                    
                    if (info->mStrokeEnable) {
                        lineWidth += strokeWidth;
                    }
                }
                else {              /* 所有字母里没有空格 */
                    /* 换行 */
                    linebreaks.Add((void*) (i-1)); /* ruix: br should be at pos i-1, and new line should contains current char */
                    lineWidth = word_width;
                    
                    if (info->mStrokeEnable) {
                        lineWidth += strokeWidth;
                    }
                }
            } else {
                    lineWidth += word_width;
            }
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        }
    }

    if (linebreaks.IsEmpty() || (len - 1) > ((int32) linebreaks.Last())) {
        linebreaks.Add((void*) (len - 1));
    }

    int32 lineHeight = m_rowSpace > m_realFontSize ? m_rowSpace:m_realFontSize;
    int32 texHeight = lineHeight * linebreaks.Size();
    int32 texLimit = info->mWidthLimit; /* FIXME: 这里可以考虑使用最长的行宽，在单行情况下能省点儿。 */
    int32 texWidth = 0;

    int32 baseline = m_pFTInfo->mFTFace->size->metrics.ascender >> 6;

    int32 lineOffsetStart = 0;

    if (info->mStrokeEnable) {
        /* 描边 */
        texWidth += strokeWidth;
        lineHeight += strokeWidth;
        texHeight += strokeWidth * linebreaks.Size();

        baseline += strokeWidth;
        lineOffsetStart += strokeWidth;
    }
    
    if (info->mShadowEnable) {
        /* 阴影 */
        texWidth += info->mShadowX;
        texHeight += info->mShadowY;
//        baseline += info->mShadowX;
//        lineOffsetStart += info->mShadowX;
    }
    
    int32 lineOffsetX = lineOffsetStart;

    /* 分配贴图内存。 */
    uint8* pData = (uint8*) DMALLOC(texLimit * texHeight * sizeof(uint32));
    memset(pData, 0x0, texLimit * texHeight * sizeof(uint32));

    RasterCallbackData callback_data;
    callback_data.mBuffer = pData;
    callback_data.mOffsetY = baseline;
    callback_data.mBufferWidth = texLimit;
    callback_data.mBufferHeight = texHeight;


    int32 lineIdx = 0;
    int32 lineEnd = (int32) linebreaks[lineIdx];
    /* 1. 描边 */
    if (info->mStrokeEnable) {
        for (int32 i = 0; i <= lineEnd; ++i) {
            FT_GlyphSlot slot = m_pFTInfo->mFTFace->glyph;
            FT_UInt idx = FT_Get_Char_Index(m_pFTInfo->mFTFace, unicode_text[i]);
            if (idx == 0) {
                /* 没有发现对应的字 */
                /* TODO */
                continue;
            } else {
                err = FT_Load_Glyph(m_pFTInfo->mFTFace, idx, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
                NGASSERT(slot->format == FT_GLYPH_FORMAT_OUTLINE);
                if (err) {
                    NG_DEBUG_LOG("[FreeType] load char '%c' (at index %d of %s) failed!~", str[i], i, str);
                    /* TODO */
                    continue;
                }
                
                /*! 处理实际宽度同间隔宽度不同的特殊情况, 例如字符 [ \/j ][ruix] */
                if (i==0 && slot->metrics.width > slot->advance.x) {
                    lineOffsetX += ceil((slot->metrics.width - slot->advance.x)>>6);
                }

                callback_data.mOffsetX = lineOffsetX;
                callback_data.mOffsetY = lineIdx * lineHeight + baseline;

                if (0 == FT_Get_Glyph(slot, &strokeGlyph)) {
                    FT_Glyph_StrokeBorder(&strokeGlyph, stroker, FALSE, TRUE);
                    // Again, this needs to be an outline to work.
                    if (strokeGlyph->format == FT_GLYPH_FORMAT_OUTLINE) {
                        // Render the outline spans to the span list
                        FT_Outline *o =
                        &reinterpret_cast<FT_OutlineGlyph>(strokeGlyph)->outline;

                        callback_data.mColorR = info->mStroke.r;
                        callback_data.mColorG = info->mStroke.g;
                        callback_data.mColorB = info->mStroke.b;
                        callback_data.mAlpha = 255;
                        RenderSpans(m_pFTInfo->mFTLibrary, o, &callback_data);
                    }
                    FT_Done_Glyph(strokeGlyph);
                }

                lineOffsetX += slot->advance.x >> 6;
            }

            if (i == lineEnd && lineIdx < linebreaks.Size() - 1) {
                lineEnd = (int32) linebreaks[++lineIdx];
                lineOffsetX = lineOffsetStart;
            }
        }
    
        FT_Stroker_Done(stroker);
    }

    lineOffsetX = lineOffsetStart; /* 使用lineOffsetStart重置lineOffsetX，用于生成无描边字数据。 */

    /* 2. 无描边 */
    lineIdx = 0;
    lineEnd = (int32) linebreaks[lineIdx];
    for (int32 i = 0; i <= lineEnd; ++i) {
        FT_GlyphSlot slot = m_pFTInfo->mFTFace->glyph;
        FT_UInt idx = FT_Get_Char_Index(m_pFTInfo->mFTFace, unicode_text[i]);
        if (idx == 0) {
            /* 没有发现对应的字 */
            /* TODO */
            continue;
        } else {
            err = FT_Load_Glyph(m_pFTInfo->mFTFace, idx, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
            NGASSERT(slot->format == FT_GLYPH_FORMAT_OUTLINE);
            if (err) {
                NG_DEBUG_LOG("[FreeType] load char '%c' (at index %d of %s) failed!~", str[i], i, str);
                /* TODO */
                continue;
            }
            
            /*! 处理实际宽度同间隔宽度不同的特殊情况, 例如字符 [ \/j ][ruix] */
            if (i==0 && slot->metrics.width > slot->advance.x) {
                lineOffsetX += ceil((slot->metrics.width - slot->advance.x)>>6);
            }

            callback_data.mOffsetX = lineOffsetX;
            callback_data.mOffsetY = lineIdx * lineHeight + baseline;
            
            /* 2. 阴影处理 */
            if (info->mShadowEnable) {
                
                int32 oriOffsetX = callback_data.mOffsetX;
                int32 oriOffsetY = callback_data.mOffsetY;
                
                callback_data.mOffsetX = oriOffsetX + info->mShadowX;
                callback_data.mOffsetY = oriOffsetY + info->mShadowY;
                
                callback_data.mColorR = info->mShadow.r;
                callback_data.mColorG = info->mShadow.g;
                callback_data.mColorB = info->mShadow.b;
                callback_data.mAlpha = info->mShadowAlpha;
                
                RenderSpans(m_pFTInfo->mFTLibrary, &slot->outline, &callback_data);
                
                callback_data.mOffsetX = oriOffsetX;
                callback_data.mOffsetY = oriOffsetY;
            }

            callback_data.mColorR = info->mTextColor.r;
            callback_data.mColorG = info->mTextColor.g;
            callback_data.mColorB = info->mTextColor.b;
            callback_data.mAlpha = 255;
            RenderSpans(m_pFTInfo->mFTLibrary, &slot->outline, &callback_data);

            lineOffsetX += slot->advance.x >> 6;
            
            /*! 处理实际宽度同间隔宽度不同的特殊情况, 例如字符 [ \/j ][ruix] */
            if (i==lineEnd && slot->metrics.width > slot->advance.x) {
                lineOffsetX += ceil((slot->metrics.width - slot->advance.x)>>6);
            }
            
            /* 获取最大宽度，这样可以计算当前文本真实宽度 [ruix] */
            if (lineOffsetX>texWidth) {
                texWidth = lineOffsetX;
            }
        }

        if (i == lineEnd && lineIdx < linebreaks.Size() - 1) {
            lineEnd = (int32) linebreaks[++lineIdx];
            lineOffsetX = lineOffsetStart;
        }
    }

    SAFE_FREE(unicode_text);
    
    /*! 增加行尾描边处理 */
    if (info->mStrokeEnable) {
        texWidth += strokeWidth;
    }

    cocos2d::CCTexture2D* pTex = DNEW(cocos2d::CCTexture2D);
    pTex->initWithData(pData, cocos2d::kTexture2DPixelFormat_RGBA8888, texLimit, texHeight, cocos2d::CCSizeMake(texWidth, texHeight));

    SAFE_FREE(pData);

    /* 更新光标位置 */
    info->mInputPos = lineOffsetX;

    return pTex;
}

bool asFreeTypeFont::JudgeIsEnlish(unsigned long world)
{
    if ((world >= 0x41 && world <= 0x5A) || (world >= 0x61 && world <= 0x7A)) {   /* 英文字母 A-Z a-z */
        return true;
    }
    return false;
}

bool asFreeTypeFont::ContainCharacter(unsigned long world, bool isWorldPrior)
{
    /* 判断是不是存在[.?!]["')\]] 这些字符 */
    if (isWorldPrior) {    /* .?!]'")\ ; 将标点符号放在上一行 */
        if (world == 0x2E || world == 0x3F || world == 0x21 ||
            world == 0x5D || world == 0x27 || world == 0x22 ||
            world == 0x29 || world == 0x5C || world == 0x3B) {
            return true;
        }
    }
    else {                  /* [('" 将标点放在下一行*/
        if (world == 0x5B || world == 0x28 || world == 0x27 || world == 0x22) {
            return true;
        }
    }
    
    return false;
}

void asFreeTypeFont::ClearUnusedCacheTexture() {
    ngHashMapIterator* itr = (ngHashMapIterator*) m_textureCache.Iterator();
    while (itr->HasNext()) {
        ngMap::ngEntry* pEntry = itr->NextEntry();
        cocos2d::CCTexture2D* pTexture = (cocos2d::CCTexture2D*) pEntry->GetValue();
        if (pTexture->retainCount() == 1) { /* only 1 ref from this font instance. */
            SAFE_DELETE(pTexture);
            m_textureCache.Remove((const char*) pEntry->GetKey());
        }
    }
    SAFE_DELETE(itr);
}

#pragma mark - FreeTypeFontTextInfo

__T_FREETYPE_FONT_TEXT_INFO__::__T_FREETYPE_FONT_TEXT_INFO__()
: mStrokeSize(-1)
, mStrokeEnable(FALSE)
, mWidthLimit(-1) {
    memset(&mStroke, 0x0, sizeof(Color3B));
    memset(&mTextColor, 0x0, sizeof(Color3B));
}