//
//  NSString+XOR.m
//  template
//
//  Created by Yuhui Wang on 7/18/11.
//  Copyright 2011 Anansi Mobile. All rights reserved.
//

#import "NSString+XOR.h"
#import "NSData+Base64.h"

@implementation NSString (XOR)

+ (NSString *)encryptXOR:(NSString *)aString withKey:(NSString *)aKey {
	
	// Create data object from the string
	NSData *data = [aString dataUsingEncoding:NSUTF8StringEncoding];
	
	// Get pointer to data to obfuscate
	char *dataPtr = (char *) [data bytes];
	
	// Get pointer to key data
	char *keyData = (char *) [[aKey dataUsingEncoding:NSUTF8StringEncoding] bytes];
	
	// Points to each char in sequence in the key
	char *keyPtr = keyData;
	int keyIndex = 0;
	
	// For each character in data, xor with current value in key
	for (int x = 0; x < [data length]; x++) 
	{
		// Replace current character in data with 
		// current character xor'd with current key value.
		// Bump each pointer to the next character
		*dataPtr = *dataPtr ^ *keyPtr; 
        dataPtr ++;
        keyPtr ++;
		
		// If at end of key data, reset count and 
		// set key pointer back to start of key value
		if (++keyIndex == [aKey length]) {
			keyIndex = 0, keyPtr = keyData;
		}
	}
	
	return [data base64EncodedString];
}

+ (NSString *)decryptXOR:(NSString *)aString withKey:(NSString *)aKey {
	NSData *data = [NSData dataFromBase64String:aString];
	
	// Get pointer to data to obfuscate
	char *dataPtr = (char *) [data bytes];
	
	// Get pointer to key data
	char *keyData = (char *) [[aKey dataUsingEncoding:NSUTF8StringEncoding] bytes];
	
	// Points to each char in sequence in the key
	char *keyPtr = keyData;
	int keyIndex = 0;
	
	// For each character in data, xor with current value in key
	for (int x = 0; x < [data length]; x++) 
	{
		// Replace current character in data with 
		// current character xor'd with current key value.
		// Bump each pointer to the next character
		*dataPtr = *dataPtr ^ *keyPtr; 
        dataPtr ++;
        keyPtr ++;
		
		// If at end of key data, reset count and 
		// set key pointer back to start of key value
		if (++keyIndex == [aKey length]) {
			keyIndex = 0, keyPtr = keyData;
		}
	}
	
	return  [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
	
}


@end
