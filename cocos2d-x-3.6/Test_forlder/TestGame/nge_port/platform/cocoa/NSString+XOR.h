//
//  NSString+XOR.h
//  template
//
//  Created by Yuhui Wang on 7/18/11.
//  Copyright 2011 Anansi Mobile. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef AGENT_GAME
#define XORKEY @"I love you not because of who you are, but because of who I am when I am with you."
#else
#define XORKEY @"One ring to rule them all, one ring to find them, one ring to bring them all and in the darkness bind them."
#endif

@interface NSString (XOR)

+ (NSString *)encryptXOR:(NSString *)aString withKey:(NSString *)aKey;
+ (NSString *)decryptXOR:(NSString *)aString withKey:(NSString *)aKey;

@end
