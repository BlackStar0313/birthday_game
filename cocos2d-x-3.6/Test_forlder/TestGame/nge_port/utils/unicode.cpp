/*
 *  unicode.cpp
 *  NextGenEngine (pnk)
 *
 *  Created by Chen Zhen on 14-7-7.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "unicode.h"

#ifndef NULL
#define NULL (0x0)
#endif

#include <string.h>
#include <stdlib.h>

/* 
 
 UTF-8相关信息：
 
 WIKI: http://en.wikipedia.org/wiki/UTF-8
        ( 中文WIKI: http://zh.wikipedia.org/wiki/UTF-8 )
 RFC3629: http://tools.ietf.org/html/rfc3629
 
 char table:

     Char. number range  |        UTF-8 octet sequence
     (hexadecimal)    |              (binary)
     --------------------+---------------------------------------------
     0000 0000-0000 007F | 0xxxxxxx
     0000 0080-0000 07FF | 110xxxxx 10xxxxxx
     0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
     0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

 */

size_t utf8_str_len(const char* utf8_str) {

    if (utf8_str == NULL) {
        return 0;
    }

    const size_t len = strlen(utf8_str);

    if (len == 0) {
        return 0;
    }

    size_t cnt = 0;

    const char* tmpChr = utf8_str;

    do {
        if (0 == (*tmpChr >> 7 & 0x1)) {
            /* ascii char: 0xxxxxxx */
            cnt++;
            tmpChr += 1;
        } else if (0x1E == ((*tmpChr >> 3) & 0x1E)) {
            /* 4-bit char: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */

            cnt++;
            tmpChr += 4;

        } else if (0xE  == ((*tmpChr >> 4) & 0xE)) {
            /* 3-bit char: 1110xxxx 10xxxxxx 10xxxxxx */
            cnt++;
            tmpChr += 3;
        } else if (0x6  == ((*tmpChr >> 5) & 0x6)) {
            /* 2-bit char: 110xxxxx 10xxxxxx */
            cnt++;
            tmpChr += 2;
        } else {
            /* bad utf-8 string! */
            cnt = 0;
            break;
        }
    } while (tmpChr < utf8_str + len);

    return cnt;
}

int utf8_str_char_len(const char* utf8_str, int utf8_character_len) {
    
    if (utf8_str == NULL) {
        return 0;
    }
    
    const size_t len = strlen(utf8_str);
    
    if (len == 0) {
        return 0;
    }
    
    size_t cnt = 0;
    
    const char* tmpChr = utf8_str;
    
    int char_len = 0;
    
    do {
        
        if (cnt>=utf8_character_len) {
            break;
        }
        
        if (0 == (*tmpChr >> 7 & 0x1)) {
            /* ascii char: 0xxxxxxx */
            cnt++;
            tmpChr += 1;
            
            char_len += 1;
        } else if (0x1E == ((*tmpChr >> 3) & 0x1E)) {
            /* 4-bit char: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            
            cnt++;
            tmpChr += 4;
            
            char_len += 4;
        } else if (0xE  == ((*tmpChr >> 4) & 0xE)) {
            /* 3-bit char: 1110xxxx 10xxxxxx 10xxxxxx */
            cnt++;
            tmpChr += 3;
            
            char_len += 3;
        } else if (0x6  == ((*tmpChr >> 5) & 0x6)) {
            /* 2-bit char: 110xxxxx 10xxxxxx */
            cnt++;
            tmpChr += 2;
            
            char_len += 2;
        } else {
            /* bad utf-8 string! */
            cnt = 0;
            break;
        }
    } while (tmpChr < utf8_str + len);
    
    return char_len;
}


static const size_t unicode_buffer_size = 256;

unsigned short* utf8_decode(const char* utf8_str, size_t* len) {
    if (utf8_str == NULL) {
        if (len) {
            *len = 0;
        }
        return NULL;
    }

    const size_t utf8_len = strlen(utf8_str);

    if (utf8_len == 0) {
        if (len) {
            *len = 0;
        }
        return NULL;
    }

    unsigned short* buffer = (unsigned short*) malloc((unicode_buffer_size + 1) * sizeof(unsigned short));

    size_t cnt = 0;
    size_t buffer_size = unicode_buffer_size;

    const char* tmpChr = utf8_str;
    unsigned short* bufferChar = buffer;

    do {

        if (cnt == buffer_size) {
            /* 缓冲区满。 */
            unsigned short* tmp = buffer;
            buffer = (unsigned short*) malloc((cnt + unicode_buffer_size + 1) * sizeof(unsigned short));
            buffer_size = cnt + unicode_buffer_size;
            memcpy(buffer, tmp, cnt * sizeof(unsigned short));
            free(tmp);

            bufferChar = buffer + cnt;
        }

        if (0 == (*tmpChr >> 7 & 0x1)) {
            /* ascii char: 0xxxxxxx */
            *bufferChar = *tmpChr;
            cnt++;
            bufferChar++;
            tmpChr += 1;
        } else if (0x1E == ((*tmpChr >> 3) & 0x1E)) {
            /* 4-bit char: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */

            *bufferChar = (((unsigned short)*(tmpChr + 3)) & 0x3F)
                        | (((unsigned short)(*(tmpChr + 2)) & 0x3F) << 6)
                        | (((unsigned short)(*(tmpChr + 1)) & 0x3F) << 12)
                        | (((unsigned short)(*(tmpChr)) & 0x7) << 18);
            cnt++;
            bufferChar++;
            tmpChr += 4;

        } else if (0xE  == ((*tmpChr >> 4) & 0xE)) {
            /* 3-bit char: 1110xxxx 10xxxxxx 10xxxxxx */
            *bufferChar = (((unsigned short)*(tmpChr + 2)) & 0x3F)
                        | (((unsigned short)(*(tmpChr + 1)) & 0x3F) << 6)
                        | (((unsigned short)(*(tmpChr)) & 0xF) << 12);
            cnt++;
            bufferChar++;
            tmpChr += 3;
        } else if (0x6  == ((*tmpChr >> 5) & 0x6)) {
            /* 2-bit char: 110xxxxx 10xxxxxx */
            *bufferChar = (((unsigned short)*(tmpChr + 1)) & 0x3F)
                        | (((unsigned short)(*(tmpChr)) & 0x1F) << 6);
            cnt++;
            bufferChar++;
            tmpChr += 2;
        } else {
            /* bad utf-8 string! */
            cnt = 0;
            free(buffer);
            buffer = NULL;
            break;
        }

    } while (tmpChr < utf8_str + utf8_len);

    if (buffer) {
        buffer[cnt] = 0x0;
    }

    if (len) {
        *len = cnt;
    }
    return buffer;
}

static const size_t utf8_buffer_size = 1024;

char* utf8_encode(unsigned short* unicode_str, size_t unicode_len, size_t* len) {

    if (unicode_str == NULL || unicode_len <= 0) {
        if (len) {
            *len = 0;
        }
        return NULL;
    }

    char* buffer = (char*) malloc(utf8_buffer_size + 1);
    char* bufferChar = buffer;

    size_t buffer_size = utf8_buffer_size;
    size_t cnt = 0;

    unsigned short* tmpChr = unicode_str;

    do {

        if (cnt == buffer_size) {
            /* 缓冲区满。 */
            char* tmp = buffer;
            buffer = (char*) malloc(cnt + unicode_buffer_size + 1);
            buffer_size = cnt + utf8_buffer_size;
            memcpy(buffer, tmp, cnt);
            free(tmp);

            bufferChar = buffer + cnt;
        }

        if (*tmpChr <= 0x7F) {
            *bufferChar = *tmpChr;  /* 0xxxxxxx */
            cnt++;
            bufferChar++;
        } else if (*tmpChr <= 0x7FF) {
            *(bufferChar + 1) = 0x80 | (*tmpChr & 0x3F);    /* 10xxxxxx */
            *(bufferChar) = 0xC0 | ((*tmpChr >> 6) & 0x1F); /* 110xxxxx */
            cnt += 2;
            bufferChar += 2;
        } else if (*tmpChr <= 0xFFFF) {
            *(bufferChar + 2) = 0x80 | (*tmpChr & 0x3F);            /* 10xxxxxx */
            *(bufferChar + 1) = 0x80 | ((*tmpChr >> 6) & 0x3F);     /* 10xxxxxx */
            *(bufferChar) = 0xE0 | ((*tmpChr >> 12) & 0xF);        /* 1110xxxx */
            cnt += 3;
            bufferChar += 3;
        } else if (*tmpChr <= 0x10FFFF) {
            *(bufferChar + 3) = 0x80 | ((*tmpChr) & 0x3F);          /* 10xxxxxx */
            *(bufferChar + 2) = 0x80 | ((*tmpChr >> 6) & 0x3F);     /* 10xxxxxx */
            *(bufferChar + 1) = 0x80 | ((*tmpChr >> 12) & 0x3F);    /* 10xxxxxx */
            *(bufferChar) = 0xF0 | ((*tmpChr >> 18) & 0x7);        /* 11110xxx */
            cnt += 4;
            bufferChar += 5;
        } else {
            /* bad unicode char!~ */
            cnt = 0;
            free(buffer);
            buffer = NULL;
            break;
        }
    } while (++tmpChr < unicode_str + unicode_len);

    if (buffer) {
        buffer[cnt] = 0x0;
    }

    if (len) {
        *len = cnt;
    }

    return buffer;
}
