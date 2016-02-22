//
//  asRegex.cpp
//  jianghu
//
//  Created by Xie Rui on 10/29/13.
//  Copyright (c) 2013 Anansi. All rights reserved.
//
#if defined( NGE_PLATFORM_METRO )

#else

#include "asRegex.h"
#include <stdio.h>
#include <string.h>

#include <nsl/ngString.h>

#define SUBSLEN 10
#define EBUFLEN 128
#define BUFLEN 1024

namespace asRegex {
    
#if USE_POSIX_PCRE
    
    ngLinkedList* grepText(NGCSTR pattern, NGCSTR text) {
        /*! TODO: 增加grep搜索 */
        
        return NULL;
    }
    
    int test()
    {
        size_t       len;
        regex_t       re;
        regmatch_t    subs [SUBSLEN];
        char          matched [BUFLEN];
        char          errbuf [EBUFLEN];
        int           err, i;
        char          src    [] = "sdfasdfsdf title color = \"red\">Hello</title> 222 <title color = \"green\">World</title 222 sdfsadf";
        char          pattern [] = "<title color = \"([^<>/]+)\">([^<>/]+)</title>";
//        const char pattern[] = "<([^<>]+)title([^<>/]+)>";
        
        printf("String : %s\n", src);
        printf("Pattern: \"%s\"\n", pattern);
        
        err = regcomp(&re, pattern, REG_EXTENDED);
        if (err)
        {
            len = regerror(err, &re, errbuf, sizeof(errbuf));
            printf("error: regcomp: %s\n", errbuf);
            return 1;
        }
        
        int startOffset = 0;
        
        /*! head 首次匹配 */
        //<<<
        err = regexec(&re, src, (size_t) SUBSLEN, subs, 0);
        if (err == REG_NOMATCH)
        {
            printf("Sorry, no match only head ...\n");
            regfree(&re);
            return 0;
        }
        else if (err)
        {
            len = regerror(err, &re, errbuf, sizeof(errbuf));
            printf("error: regexec: %s\n", errbuf);
            regfree(&re);
            return 1;
        }
        
        int32 fPos = subs[0].rm_so;
    
        if (fPos>0) {
            memcpy(matched, src, fPos);
            matched[fPos]='\0';
            printf("head : %s\n", matched);
        } else {
            printf("no head\n");
        }
        
        //>>>
        
        
        while (regexec(&re, src + startOffset, (size_t) SUBSLEN, subs, 0)==0) {
            printf("\nOK, has matched ...\n\n");
            for (i = 0; i <= re.re_nsub; i++)
            {
                len = subs[i].rm_eo - subs[i].rm_so;
                if (i == 0)
                {
                    printf ("begin: %d, len = %lu   ", subs[i].rm_so+startOffset, len);
                }
                else
                {
                    printf("subexpression_r_r %d begin: %d, len = %lu   ", i, subs[i].rm_so+startOffset, len);
                }
                
                memcpy (matched, src + subs[i].rm_so+startOffset, len);
                matched[len] = '\0';
                printf("match: %s\n", matched);
            }
            
            startOffset += subs[0].rm_eo;
        }
        
        /*! tail */
        if (startOffset<strlen(src)) {
            int l = strlen(src) - startOffset;
            char* s = src + startOffset;
            char* e = src + strlen(src);
            memcpy(matched, s, l);
            matched[l]='\0';
            printf("tail : %s\n", matched);
        } else {
            printf("no tail\n");
        }
        
        regfree(&re);
        
        return (0);
    }
#else
    
    int test () {
        size_t  len;
        pcre*   re;
        char    src    [] = "111中国 <title color = \"red\">Hello 中国 World</title> 中国 222 <title color = \"red\">Hello 中国 World</title>";
        char    pattern [] = "title";
        char    matched [100];
        int     ovector[100];
        
        printf("String : %s\n", src);
        printf("Pattern: \"%s\"\n", pattern);
        

        const char* error;
        int erroroffset;
        
        re = pcre_compile(pattern, 0, &error, &erroroffset, NULL);
        if (re == NULL)
        {
            printf("PCRE compilation failed at offset %d: %s\n", erroroffset, error);
            return 1;
        }
        
        int srcLen =(int)strlen(src);
        
        int rc = pcre_exec(re, NULL, src, srcLen, 0, PCRE_ANCHORED, ovector, 100);
        if (rc < 0)
        {
            switch(rc)
            {
                case PCRE_ERROR_NOMATCH: printf("No match\n"); break;
                    /*
                     Handle other special cases if you like
                     */
                default: printf("Matching error %d\n", rc); break;
            }
            pcre_free(re);     /* Release memory used for the compiled pattern */
            return 1;
        }
        
        /* Match succeded */
        
        printf("\nMatch succeeded at offset %d\n", ovector[0]);
        
        if (rc == 0)
        {
            rc = 100/3;
            printf("ovector only has room for %d captured substrings\n", rc - 1);
        }
        while (rc >0) {
            int start = 0;
            int end = 0;
            int len = 0;
            for (int i = 0; i<rc; i++) {
                start = ovector[2*i];
                end = ovector[2*i+1];
                len = end - start;
                memcpy(matched, src+start, len);
                matched[len] = '\0';
                printf("\n substring %d , start %d, len %d, %s", i, start, len, matched);
            }
            
            rc = pcre_exec(re, NULL, src, srcLen, end, 0, ovector, 100);
        }
        
        
        
        pcre_free(re);     /* Release memory used for the compiled pattern */

        return (0);
    };
#endif
}

#endif // NGE_PLATFORM_METRO
