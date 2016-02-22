//
//  asUrl.h
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-8.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef __ASURL_H__
#define __ASURL_H__

/* 
 handle url string encode/decode with utf-8 format.
 */

#include <nsl/ngString.h>

#define NGX_INT_T_LEN   NGX_INT32_LEN
#define NGX_MAX_INT_T_VALUE  2147483647

#ifdef __cplusplus
extern "C" {
#endif
    
    int     as_url_decode(char *str, int len);
    char*   as_url_encode(const char *s, int len, int *new_length);
    
    int parse_url(char *url, char **serverstrp, int *portp, char **pathstrp);
    
    
    int parse_urlV2(ngString& url, ngString& shost, int& iport, ngString& spath);
    
    static inline u_char *
    ng_strlchr(u_char *p, u_char *last, u_char c)
    {
        while (p < last) {
            
            if (*p == c) {
                return p;
            }
            
            p++;
        }
        
        return NULL;
    }
    
    static inline int ng_atoi(u_char *line, size_t n)
    {
        int  value, cutoff, cutlim;
        
        if (n == 0) {
            return -1;
        }
        
        cutoff = NGX_MAX_INT_T_VALUE / 10;
        cutlim = NGX_MAX_INT_T_VALUE % 10;
        
        for (value = 0; n--; line++) {
            if (*line < '0' || *line > '9') {
                return -1;
            }
            
            if (value >= cutoff && (value > cutoff || *line - '0' > cutlim)) {
                return -1;
            }
            
            value = value * 10 + (*line - '0');
        }
        
        return value;
    }

#ifdef __cplusplus
}
#endif
    
#endif	//__ASURL_H__
