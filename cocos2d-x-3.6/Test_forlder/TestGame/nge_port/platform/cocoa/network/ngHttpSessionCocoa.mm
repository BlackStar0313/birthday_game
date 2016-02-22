/*
 *  ngHttpSessionCocoa.cpp
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 5/22/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#include "ngHttpSessionCocoa.h"
#include "ngHttpRequest.h"
#include "ngHashMapIterator.h"
#include "ngHttpResponse.h"
#include "ngHttpHeader.h"
#include "nslUtils.h"
#include "md5.h"
#include "NSString+XOR.h"

@implementation HttpSessionDelegate

- (id)initWithParent:(ngHttpSessionCocoa *)parent
{
	if (self = [super init]) {
		m_pParent = parent;
	}
	return self;
}

- (void)connection:(NSURLConnection *)theConnection didReceiveResponse:(NSURLResponse *)response
// A delegate method called by the NSURLConnection when the request/response 
// exchange is complete.  We look at the response to check that the HTTP 
// status code is 2xx.  If it isn't, we fail right now.
{
	NSLog(@"didReceiveResponse");
	
	NSHTTPURLResponse* pHTTPURLResponse = (NSHTTPURLResponse *)response;
	ngHttpResponse* pResponse = DNEW(ngHttpResponse);
	
	// seems useless, will set such header in the follow.
	pResponse->SetStatusCode( [pHTTPURLResponse statusCode] );
	
	// set headers.
	NSDictionary* dict = nil;
	ngHttpHeader* pHeader = pResponse->GetHeader();
	if ([pHTTPURLResponse respondsToSelector:@selector(allHeaderFields)]) {
		dict = [pHTTPURLResponse allHeaderFields];
		NSLog(@"all response headers: %@", dict);
		for (NSString* key in dict) {
			NSString* value = (NSString *)[dict objectForKey:key];
			pHeader->Put( [key UTF8String], [value UTF8String] );
			
			// remember X-Anansi-Session
			if ([key isEqualToString:@"X-Anansi-Session"]) {
				[[NSUserDefaults standardUserDefaults] setObject:value forKey:key];
			}
		}
	}
		
	// set suggested filename
	NSString* filename = [response suggestedFilename];
	pResponse->SetSuggestedFilename([filename UTF8String]);
	
	// callback
	m_pParent->OnResponse(pResponse);
}

- (void)connection:(NSURLConnection *)theConnection didReceiveData:(NSData *)data
// A delegate method called by the NSURLConnection as data arrives.  The 
// response data for a POST is only for useful for debugging purposes, 
// so we just drop it on the floor.
{
	NSLog(@"didReceiveData");
	
	uint8* pBytes = (uint8 *)[data bytes];
	int length = [data length];
	
	m_pParent->OnData(pBytes, length);
	
}

- (void)connection:(NSURLConnection *)theConnection didFailWithError:(NSError *)error
// A delegate method called by the NSURLConnection if the connection fails. 
// We shut down the connection and display the failure.  Production quality code 
// would either display or log the actual error.
{
	NSLog(@"didFailWithError [%@][%@]", [error localizedDescription], [error localizedFailureReason]);
	
	// TODO: error code.
	m_pParent->OnErrorStop((NG_CONNECTION_ERROR)[error code]);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)theConnection
// A delegate method called by the NSURLConnection when the connection has been 
// done successfully.  We shut down the connection with a nil status, which 
// causes the image to be displayed.
{
	NSLog(@"didFinishLoading");
    
	ngHttpRequest *pRequest = (ngHttpRequest *)m_pParent->GetRequest();
    if (pRequest->GetMethod() != ngHttpRequest::HTTP_GET) {
        if (m_pParent->IsDESEncrypt()) {
            ngByteBuffer* pByteBuffer = ((ngHttpResponse *)(m_pParent->GetResponse()))->GetBuffer();
            uint8* buffer = pByteBuffer->GetBuffer();
            NG_DEBUG_LOG("[ngHttpSession] before:%s", (const char*)buffer);
            NSString* strBuffer = [[NSString alloc] initWithUTF8String:(const char *)buffer];

            NSString* strDecoded = NULL;
            ngStringV2 strKey = ngHttpSession::GetXorKey();
            if (strKey.IsEmpty()) {
                strDecoded = [NSString decryptXOR:strBuffer withKey:XORKEY];
            } else {
                NSString* strXorKey = [[NSString alloc] initWithUTF8String:strKey.GetCString()];
                strDecoded = [NSString decryptXOR:strBuffer withKey:strXorKey];
                [strXorKey release];
            }

            NGCSTR pDest = [strDecoded UTF8String];
            if (pDest == NULL) {
                NSLog(@"Didn't get correct data from server");
                [strBuffer release];
                return;
            }
            pByteBuffer->CopyBuffer((const uint8*)pDest, strlen(pDest));
            pByteBuffer->AppendBuffer((uint8*)"\0",1);
            NG_DEBUG_LOG("[ngHttpSession] (encrypt) response data: %s", (const char*)pByteBuffer->GetBuffer());
            [strBuffer release];
            NSLog(@"didFinishDecoding");
        } else {
            ngByteBuffer* pByteBuffer = ((ngHttpResponse *)(m_pParent->GetResponse()))->GetBuffer();
            uint8* buffer = pByteBuffer->GetBuffer();
            //printf("before:%s\n", (const char*)buffer);
            NSString* strBuffer = [[NSString alloc] initWithUTF8String:(const char *)buffer];
            NGCSTR pDest = [strBuffer UTF8String];
            if (pDest == NULL) {
                NSLog(@"Didn't get correct data from server");
                [strBuffer release];
                return;
            }
            pByteBuffer->CopyBuffer((const uint8*)pDest, strlen(pDest));
            pByteBuffer->AppendBuffer((uint8*)"\0",1);
            NG_DEBUG_LOG("[ngHttpSession] response data: %s", (const char*)pByteBuffer->GetBuffer());
            [strBuffer release];
        }
    }
	
	m_pParent->OnFinish();
}

@end

ngHttpSession* ngHttpSession::CreateSession() {
	ngHttpSession* pSession = DNEW(ngHttpSessionCocoa);
	return pSession;
}

ngHttpSessionCocoa::ngHttpSessionCocoa()
{
	m_pSessionDelegate = [[HttpSessionDelegate alloc] initWithParent:this];
}

ngHttpSessionCocoa::~ngHttpSessionCocoa()
{
	[m_pSessionDelegate release];
}

void ngHttpSessionCocoa::RequestInternal(ngStringV2& url)
{
	// set request
	const char* pUrl = url.GetCString();
	
	NSURL* pNSURL = [NSURL URLWithString:[NSString stringWithUTF8String:pUrl]];
	NSMutableURLRequest* pNSURLRequest = [NSMutableURLRequest requestWithURL:pNSURL];
	
	// 1. headers
	ngHttpRequest* pRequest = (ngHttpRequest *)GetRequest();
	ngHttpHeader* pHeader = pRequest->GetHeader();
	
	switch (pRequest->GetMethod()) {
		case ngHttpRequest::HTTP_GET:
			[pNSURLRequest setHTTPMethod:@"GET"];
			break;
		case ngHttpRequest::HTTP_POST:
			[pNSURLRequest setHTTPMethod:@"POST"];
			break;
		case ngHttpRequest::HTTP_PUT:
			[pNSURLRequest setHTTPMethod:@"PUT"];
			break;
		case ngHttpRequest::HTTP_DELETE:
			[pNSURLRequest setHTTPMethod:@"DELETE"];
			break;
		default:
			[pNSURLRequest setHTTPMethod:@"GET"];
			break;
	}
	
	// user agent
	[pNSURLRequest setValue:@"Anansi/1.0" forHTTPHeaderField:@"User-Agent"];
	
	// X-Anansi-Session
	NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
	NSString* pSessionValue = [userDefaults stringForKey:@"X-Anansi-Session"];
	if (pSessionValue != nil) {
		[pNSURLRequest setValue:pSessionValue forHTTPHeaderField:@"X-Anansi-Session"];
	}
	
	ngHashMapIterator* pIterator = (ngHashMapIterator *)pHeader->Iterator();
	while (pIterator->HasNext()) {
		ngHashMap::ngEntry* pEntry = pIterator->NextEntry();
		const char* pKey = (const char*)pEntry->GetKey();
		const char* pValue = (const char*)pEntry->GetValue();
		NSString* pStrKey = [[NSString alloc] initWithUTF8String:pKey];
		NSString* pStrValue = [[NSString alloc] initWithUTF8String:pValue];
		
		[pNSURLRequest setValue:pStrValue forHTTPHeaderField:pStrKey];
		
		[pStrKey release];
		[pStrValue release];
	}
	SAFE_DELETE(pIterator); 
	
	// 2. post body.
	ngHttpRequest::HTTP_METHOD pMethod = pRequest->GetMethod();
	if (pMethod == ngHttpRequest::HTTP_POST || pMethod == ngHttpRequest::HTTP_PUT) {
		ngByteBuffer* pPostBuffer = pRequest->GetBuffer();
		uint8* pPostBytes = pPostBuffer->GetBuffer();
		NSData* pPostData = [NSData dataWithBytes:(const void*)pPostBytes length:pPostBuffer->GetLimit()];
		
#ifdef DEBUG
		NSString* str = [[NSString alloc] initWithData:pPostData encoding:NSUTF8StringEncoding];
		NSLog(@"%@", str); 
		[str release];
#endif

        NSString* pEncryptedString = NULL;

        if (IsDESEncrypt()) {
            NSString* pPostString = [[NSString alloc] initWithData:pPostData encoding:NSUTF8StringEncoding];

            ngStringV2 strKey = ngHttpSession::GetXorKey();
            if (strKey.IsEmpty()) {
                pEncryptedString = [NSString encryptXOR:pPostString withKey:XORKEY];
            } else {
                NSString* strXorKey = [[NSString alloc] initWithUTF8String:strKey.GetCString()];
                pEncryptedString = [NSString encryptXOR:pPostString withKey:strXorKey];
                [strXorKey release];
            }

            [pPostString release];
            [pEncryptedString retain]; /* 这里retain，保持释放行为和下面else中alloc的一致。 */
        } else {
            pEncryptedString = [[NSString alloc] initWithData:pPostData encoding:NSUTF8StringEncoding];
        }

        if (IsMD5Check()) {
            char digest[33] = {0,};

            NSString* pFullString = NULL;
            ngStringV2 strTail = ngHttpSession::GetTailKey();
            if (strTail.IsEmpty()) {
                pFullString = [NSString stringWithFormat:@"%@%s", pEncryptedString, "-idodo"];
            } else {
                NSString* strTailKey = [[NSString alloc] initWithUTF8String:strTail.GetCString()];
                pFullString = [NSString stringWithFormat:@"%@%@", pEncryptedString, strTailKey];
                [strTailKey release];
            }

            int ret = toMD5(digest, [pFullString UTF8String]);
            //[pPostString release];

            if (ret == 0) {
                NSString* pDigest = [NSString stringWithUTF8String:(const char*)digest];
                [pNSURLRequest setValue:pDigest forHTTPHeaderField:@"X-Game-Checksum"];
                [pNSURLRequest setHTTPBody:[pEncryptedString dataUsingEncoding:NSUTF8StringEncoding]];
            }
        }
        
        [pEncryptedString release];
	}

	NSLog(@"[REQUEST] url=%@, method = %@", [[pNSURLRequest URL] absoluteURL], [pNSURLRequest HTTPMethod]);
	
	NSDictionary *dict = [pNSURLRequest allHTTPHeaderFields];
	NSLog(@"all request headers: %@", dict);
	
	// connect
	m_pConnection = [IZURLConnection connectionWithRequest:pNSURLRequest delegate:m_pSessionDelegate];
}

void ngHttpSessionCocoa::CancelInternal()
{
	if (m_pConnection && !(m_status == SESSION_FINISH || m_status == SESSION_ERROR || m_status == SESSION_USELESS)) {
		[m_pConnection cancel];
		m_status = SESSION_FINISH;
	}
}