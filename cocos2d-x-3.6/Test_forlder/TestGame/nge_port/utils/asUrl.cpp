//
//  asUrl.cpp
//  NextGenEngine
//
//  Created by Chen Zhen on 13-6-8.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <utils/asUrl.h>



/* 
 
 1. "a"-"z"，"A"-"Z"，"0"-"9"，"."，"-"，"*"，"_" is keeped.
 
 2. " "(space) -> "%20"(hex)
 
 3. other char -> "%xy"

 */


static int _htoi(char *s)
{
	int value;
	int c;
    
	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
    
	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
    
	return (value);
}

static unsigned char hexchars[] = "0123456789ABCDEF";

int as_url_decode(char *str, int len) {
    char* dest = str;
	char* data = str;
    
	while (len--) {
		if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2)))	{
			*dest = (char) _htoi(data + 1);
			data += 2;
			len -= 2;
		} else {
			*dest = *data;
		}

		data++;
		dest++;
	}

	*dest = '\0';

	return dest - str;
}

char* as_url_encode(const char *s, int len, int *new_length) {
    unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;
	
	from = (unsigned char *)s;
	end  = (unsigned char *)s + len;
	start = to = (unsigned char *) malloc(3 * len + 1);
    memset(start, '\0', 3 * len + 1);   //calloc == malloc + memset

	while (from < end) {
		c = *from++;
        
		if (c == ' ')
		{
			//"%20", fixed, hex value.
			to[0] = '%';
			to[1] = hexchars[' ' >> 4];
			to[2] = hexchars[' ' & 0xF];
            to+= 3;
		} else if ((c < '0' && c != '-' && c != '.') ||
                   (c < 'A' && c > '9') ||
                   (c > 'Z' && c < 'a' && c != '_') ||
                   (c > 'z')) {
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 0xF];
			to += 3;
		} else {
			*to++ = c;
		}
	}
	*to = '\0';
	if (new_length) {
		*new_length = to - start;
	}

	return (char *) start;
}


int parse_url(char *url, char **serverstrp, int *portp, char **pathstrp)
{
    char buf[256];
    int serverlen, numread=0;
    
    // 跳过"http://" ，例"http://www.a.cn"->"www.a.cn"
    url = url+7;
    
    // 输入url直到遇到'/'和':'符号
    sscanf(url, "%255[^/:]", buf);
    serverlen = strlen(buf);
    *serverstrp = (char *)malloc(serverlen+1);
    strcpy(*serverstrp, buf);
    if(url[serverlen]==':')
    {
        // 获取port
        sscanf(&url[serverlen+1], "%d%n", portp, &numread);
        /* add one to go PAST it */
        
        numread++;
    }
    else
    {
        *portp = 80;
    }
    /* the path is a pointer into the rest of url */
    
    *pathstrp = &url[serverlen+numread];
    return 0;
}

int parse_urlV2(ngString& surl, ngString& shost, int& iport, ngString& spath)
{
    /*
     
     static ngx_inline u_char *
     ngx_strlchr(u_char *p, u_char *last, u_char c)
     {
     while (p < last) {
     
     if (*p == c) {
     return p;
     }
     
     p++;
     }
     
     return NULL;
     }

     host = u->url.data;
     
     last = host + u->url.len;
     
     port = ngx_strlchr(host, last, ':');
     
     uri = ngx_strlchr(host, last, '/');
     
     args = ngx_strlchr(host, last, '?');
     
     if (args) {
     if (uri == NULL || args < uri) {
     uri = args;
     }
     }
     
     if (uri) {
     if (u->listen || !u->uri_part) {
     u->err = "invalid host";
     return NGX_ERROR;
     }
     
     u->uri.len = last - uri;
     u->uri.data = uri;
     
     last = uri;
     
     if (uri < port) {
     port = NULL;
     }
     }
     
     if (port) {
     port++;
     
     len = last - port;
     
     n = ngx_atoi(port, len);
     
     if (n < 1 || n > 65535) {
     u->err = "invalid port";
     return NGX_ERROR;
     }
     
     u->port = (in_port_t) n;
     sin->sin_port = htons((in_port_t) n);
     
     u->port_text.len = len;
     u->port_text.data = port;
     
     last = port - 1;
     
     }
    */
    
    
    /* 算法参考 nginx-1.6.3 : nginx_inet.c: ngx_parse_inet_url */
    
    u_char *host, *port, *last, *uri;
    int len = 0, n = 0;
    
    host = (u_char*)surl.GetCString();
    
    /* 处理 url 中的协议前缀 */
    /* 目前仅处理http:// 后续添加其他类型 */
    if(strncmp((const char*)host, "http://", sizeof("http://")-1) == 0)
    {
        host = host + sizeof("http://")-1;
    }
    
    last = (u_char*)surl.GetCString() + surl.GetLength();
    
    port = ng_strlchr(host, last, ':');
    
    uri = ng_strlchr(host, last, '/');
    
    if (uri) {
        len = last - uri;
        last = uri;
        
        spath.SetBuffer((NGSTR)uri, len);
        
        if (uri < port) {
            port = NULL;
        }
    }
    
    if (port) {
        port++;
        
        len = last - port;
        
        n = ng_atoi(port, len);
        
        if (n < 1 || n > 65535) {
            return -1;
        }
        
        /* 获取 port */
        iport = n;
        
        last = port - 1;
    }
    
    len = last - host;
    
    if (len == 0) {
        return 01;
    }
    
    /* 获取host */
    shost.SetBuffer((NGSTR)host, len);
    
    return 0;
}


