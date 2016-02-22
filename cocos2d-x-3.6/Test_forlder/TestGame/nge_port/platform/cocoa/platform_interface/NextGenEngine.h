//
//  NextGenEngine.h
//  NextGenEngine
//
//  Created by ppcfan on 10-9-19.
//  Copyright 2010 Anansi Mobile. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NextGenEngine : NSObject <UITextFieldDelegate, UITextViewDelegate> {
	UIViewController* glViewController;
    UIView* glView;
}

@property (nonatomic, retain) UIViewController* glViewController;
@property (nonatomic, retain) UIView* glView;

+ (NextGenEngine*)sharedInstance;
- (void)registerPushNotification;

@end
