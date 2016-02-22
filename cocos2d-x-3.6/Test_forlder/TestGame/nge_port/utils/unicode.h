/*
 *  unicode.h
 *  NextGenEngine (pnk)
 *
 *  Created by Chen Zhen on 14-7-7.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __unicode_H__
#define __unicode_H__

/* unicode相关接口 */

#include <stddef.h>

#pragma mark - utf8

/*! 获取utf8文本中的unicode字符个数。 */
size_t utf8_str_len(const char* utf8_str);

/*! 获取utf8文本中指定utf8_character_len个unicode字符对应的字节数 [ruix] */
int utf8_str_char_len(const char* utf8_str, int utf8_character_len);

/*! 将utf8字符串解码为unicode文本。 */
unsigned short* utf8_decode(const char* utf8_str, size_t* len = NULL);

/*! 将unicode文本编码为utf8字符串。 */
char* utf8_encode(unsigned short* unicode_str, size_t unicode_len, size_t* len = NULL);

#pragma mark -

#endif //__unicode_H__
