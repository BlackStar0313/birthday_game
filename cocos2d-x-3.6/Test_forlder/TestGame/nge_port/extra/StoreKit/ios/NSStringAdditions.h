//
//  NSStringAdditions.h
//  iaptest
//
//  Created by Yuhui Wang on 6/5/10.
//  Copyright 2010 Anansi Mobile. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface NSString (Base64Encoding) 
+ (NSString *) base64StringFromData:(NSData *)data length:(int)length;
@end
