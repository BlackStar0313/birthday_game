//
//  NSStringAdditions.m
//  iaptest
//
//  Created by Yuhui Wang on 6/5/10.
//  Copyright 2010 Anansi Mobile. All rights reserved.
//

#import "NSStringAdditions.h"


static char base64EncodingTable[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

@implementation NSString (Base64Encoding)

+ (NSString *) base64StringFromData: (NSData *)data length: (int)length {
	int lentext = [data length]; 
	if (lentext < 1) return @"";
	
	char *outbuf = malloc(lentext*4/3+4); // add 4 to be sure
	
	if ( !outbuf ) return nil;
	
	const unsigned char *raw = [data bytes];
	
	int inp = 0;
	int outp = 0;
	int do_now = lentext - (lentext%3);
	
	for ( outp = 0, inp = 0; inp < do_now; inp += 3 )
	{
		outbuf[outp++] = base64EncodingTable[(raw[inp] & 0xFC) >> 2];
		outbuf[outp++] = base64EncodingTable[((raw[inp] & 0x03) << 4) | ((raw[inp+1] & 0xF0) >> 4)];
		outbuf[outp++] = base64EncodingTable[((raw[inp+1] & 0x0F) << 2) | ((raw[inp+2] & 0xC0) >> 6)];
		outbuf[outp++] = base64EncodingTable[raw[inp+2] & 0x3F];
	}
	
	if ( do_now < lentext )
	{
		unsigned char tmpbuf[2] = {0,0};
		int left = lentext%3;
		for ( int i=0; i < left; i++ )
		{
			tmpbuf[i] = raw[do_now+i];
		}
		raw = tmpbuf;
		inp = 0;
		outbuf[outp++] = base64EncodingTable[(raw[inp] & 0xFC) >> 2];
		outbuf[outp++] = base64EncodingTable[((raw[inp] & 0x03) << 4) | ((raw[inp+1] & 0xF0) >> 4)];
		if ( left == 2 ) {
			outbuf[outp++] = base64EncodingTable[((raw[inp+1] & 0x0F) << 2) | ((raw[inp+2] & 0xC0) >> 6)];
		}else {
			outbuf[outp++] = '=';
		}
		outbuf[outp++] = '=';
	}
	
	NSString *ret = [[[NSString alloc] initWithBytes:outbuf length:outp encoding:NSASCIIStringEncoding] autorelease];
	free(outbuf);
	
	return ret;
}
		
@end