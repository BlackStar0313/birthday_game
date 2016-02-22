/*
 *  asIMEImplCocoa.mm
 *  NextGenEngine (in pnk)
 *
 *  Created by Chen Zhen on 14-6-24.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asIMEImplCocoa.h"
#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>

#include <core/ngDevice.h>
#include <platform/cocoa/platform_interface/NextGenEngine.h>

static const float sIMETextFieldPadding = 8.0f; /* 输入框padding */

@interface asIMETextField : UITextField


@end

@implementation asIMETextField

- (CGSize) sizeThatFits: (CGSize)size {
    return CGSizeMake(240, size.height);
}

@end

#pragma mark - 

@interface asIMEDelegate : NSObject<UITextFieldDelegate/*, UITextInputDelegate*/>

@property(nonatomic) asIMEImplCocoa* m_pImpl;

@end

@implementation asIMEDelegate

@synthesize m_pImpl;

- (id) initWithImpl: (asIMEImplCocoa*) pImpl {
    self = [super init];
    self.m_pImpl = pImpl;
    return self;
}

- (void) dealloc {
    [super dealloc];
}

- (void) hideKeyboard {
    m_pImpl->NotifyHideKeyboard();
}

- (void) cancel {
    //TODO: code here if need.
}

#pragma mark - impl UITextFiledDelegate
- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
    return YES;
}// return NO to disallow editing.

- (void)textFieldDidBeginEditing:(UITextField *)textField {
    //TODO: code here if need.
}// became first responder

- (BOOL)textFieldShouldEndEditing:(UITextField *)textField {
    return YES;
}// return YES to allow editing to stop and to resign first responder status. NO to disallow the editing session to end

- (void)textFieldDidEndEditing:(UITextField *)textField {
    //TODO: code here if need.
}// may be called if forced even if shouldEndEditing returns NO (e.g. view removed from window) or endEditing:YES called

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
    //m_pImpl->NotifyTextChanged(); //notified in textDidChange.
    return YES;
}// return NO to not change text

- (BOOL)textFieldShouldClear:(UITextField *)textField {
    return YES;
}// called when clear button pressed. return NO to ignore (no notifications)

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    m_pImpl->NotifyHideKeyboard();
    return NO;
}// called when 'return' key pressed. return NO to ignore.

#if 0
#pragma mark - UITextInputDelegate
- (void)selectionWillChange:(id <UITextInput>)textInput {

}

- (void)selectionDidChange:(id <UITextInput>)textInput {

}

- (void)textWillChange:(id <UITextInput>)textInput {

}

- (void)textDidChange:(id <UITextInput>)textInput {
//    if ([textInput isKindOfClass:[asIMETextField class]]) {
        m_pImpl->NotifyTextChanged();
//    }
}
#endif

- (void)textDidChange:(NSNotification*)notification {
    m_pImpl->NotifyTextChanged();
}

- (void)keyboardDidShow:(NSNotification*)notification {
    
    NSDictionary* userInfo = [notification userInfo];
    
    if (userInfo) {
        // get the size of the keyboard in points
//        CGSize keyboardSize = [[userInfo objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;
        CGSize keyboardSize = [[userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue].size;
        
        /* http://stackoverflow.com/questions/25756087/detecting-iphone-6-6-screen-sizes-in-point-values  need handle iphone6 & plus*/
        /* handle retina here!!!! [ruix] */
        
        
        /* FIXME: here need fix the issue of high resolution screen with retina */
        if (ngDevice::GetInstance()->GetIsRetina()) {
            if (keyboardSize.width>ngDevice::GetInstance()->GetScreenWidth()) {
                m_pImpl->NotifyShowKeyboard(keyboardSize.height);
            } else {
                m_pImpl->NotifyShowKeyboard(keyboardSize.height * 2);
            }
        } else {
            m_pImpl->NotifyShowKeyboard(keyboardSize.height);
        }
        
    }
}

- (void)keyboardDidHide:(NSNotification*)notification {
    //TODO: if need
//    NSDictionary* userInfo = [notification userInfo];
//    
//    if (userInfo) {
//        // get the size of the keyboard in points
//        CGSize keyboardSize = [[userInfo objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;
//        
//        m_pImpl->NotifyShowKeyboard(keyboardSize.height * 2);
//    }
    
    m_pImpl->NotifyHideKeyboard();
}

@end

#pragma mark - asIMEImpl

asIMEImpl* asIMEImpl::CreateInstance(asIME* pIME) {

    if (ngDevice::GetInstance()->GetDeviceVersionf() < 3.2f) {
        return NULL;
    }

    if ([NextGenEngine sharedInstance].glView == nil) {
        NG_DEBUG_LOG("[asIME] should init [NextGenEngine sharedInstance].glView before use asIME.");
        return NULL;
    }

    asIMEImplCocoa* pImpl = DNEW(asIMEImplCocoa);
    pImpl->m_pIME = pIME;
    pImpl->InitKeyboard();
    return pImpl;
}

#pragma mark - asIMEImplCocoa

asIMEImplCocoa::asIMEImplCocoa()
: m_pPlatformTextField(NULL)
, m_pToolbarTextField(NULL)
, m_keyboardHeight(0) {
    m_pIMEDelegate = [[asIMEDelegate alloc] initWithImpl:this];
}

asIMEImplCocoa::~asIMEImplCocoa() {
	[m_pPlatformTextField removeFromSuperview];
	[m_pPlatformTextField release];

    [m_pToolbarTextField removeFromSuperview];
    [m_pToolbarTextField release];

    [m_pIMEDelegate release];
}

void asIMEImplCocoa::NotifyShowKeyboard(float keyboardHeight) {
    OnShowKeyboard(keyboardHeight);
}

void asIMEImplCocoa::NotifyHideKeyboard() {
    OnHideKeyboard([m_pToolbarTextField.text UTF8String]);
    if ([m_pToolbarTextField canResignFirstResponder]) {
        [m_pToolbarTextField resignFirstResponder];
    }
    if ([m_pPlatformTextField canResignFirstResponder]) {
        [m_pPlatformTextField resignFirstResponder];
    }
}

void asIMEImplCocoa::NotifyTextChanged() {
    OnTextChanged([m_pToolbarTextField.text UTF8String]);
}

void asIMEImplCocoa::InitKeyboard() {
    
    /* 注册事件监听键盘弹出 */
    [[NSNotificationCenter defaultCenter] addObserver:m_pIMEDelegate
                                             selector:@selector(keyboardDidShow:)
//                                                 name:UIKeyboardDidShowNotification
                                             name:UIKeyboardWillShowNotification
                                               object:m_pToolbarTextField];
    //TODO: if need
    /* 注册事件监听键盘收起 */
//    [[NSNotificationCenter defaultCenter] addObserver:m_pIMEDelegate
//                                             selector:@selector(keyboardDidHide:)
//                                                 name:UIKeyboardDidHideNotification
//                                               object:m_pToolbarTextField];

#if 0
	UIToolbar* keyboardBar = [[UIToolbar alloc] initWithFrame:CGRectMake(0, 0, 320, 55)];
#else
    UIToolbar* keyboardBar = [[UIToolbar alloc] init];
    [keyboardBar sizeToFit];
#endif
    

	NSMutableArray* barItems = [[NSMutableArray alloc] init];

    /* text field */
    //<<
    m_pToolbarTextField = [[asIMETextField alloc] initWithFrame:CGRectMake(0, 0, 0, keyboardBar.frame.size.height - sIMETextFieldPadding * 2)];
	//m_pToolbarTextField.delegate = [NextGenEngine sharedInstance];
	m_pToolbarTextField.opaque = YES;
	m_pToolbarTextField.backgroundColor = [UIColor colorWithRed:1.0f green:1.0f blue:1.0f alpha:1.0f];
	//m_pToolbarTextField.autocorrectionType = UITextAutocorrectionTypeNo;
	//m_pToolbarTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
    //m_pToolbarTextField.bounces = NO;
    //m_pToolbarTextField.alwaysBounceHorizontal = NO;
    //m_pToolbarTextField.alwaysBounceVertical = NO;
    m_pToolbarTextField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;

    //m_pToolbarTextField.layer.masksToBounds = YES;
    //m_pToolbarTextField.layer.cornerRadius = 4.0f;

    [m_pToolbarTextField sizeToFit];

#if 0
    m_pToolbarTextField.leftView = [[UIView alloc] initWithFrame:CGRectMake(.0f, .0f, 5.0f, .0f)];
    m_pToolbarTextField.leftViewMode = UITextFieldViewModeAlways;

    m_pToolbarTextField.rightView = [[UIView alloc] initWithFrame:CGRectMake(.0f, .0f, 5.0f, .0f)];
    m_pToolbarTextField.rightViewMode = UITextFieldViewModeAlways;
#else
    m_pToolbarTextField.borderStyle = UITextBorderStyleRoundedRect;
#endif

    m_pToolbarTextField.clearButtonMode = UITextFieldViewModeAlways;            /* 关闭按钮：总是显示 */
    m_pToolbarTextField.autocorrectionType = UITextAutocorrectionTypeDefault;   /* 自动纠错：使用Default选项 */
    m_pToolbarTextField.delegate = m_pIMEDelegate; /* m_pIMEDelegate 接收键盘事件。 */
    
    //m_pToolbarTextField.inputDelegate = m_pIMEDelegate; /* m_pIMEDelegate 接收文本输入时间。 */
    [[NSNotificationCenter defaultCenter] addObserver:m_pIMEDelegate
                                             selector:@selector(textDidChange:)
                                                 name:UITextFieldTextDidChangeNotification
                                               object:m_pToolbarTextField];

    /* 添加输入框到toolbar */
    UIBarButtonItem* pTextFieldItem = [[UIBarButtonItem alloc] initWithCustomView:m_pToolbarTextField];
    [barItems addObject:pTextFieldItem];
    [pTextFieldItem release];
    //>>

	UIBarButtonItem* flexSpace = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil];
	[barItems addObject:flexSpace];
	[flexSpace release];

	UIBarButtonItem* btnDone = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:m_pIMEDelegate action:@selector(hideKeyboard)];
	[barItems addObject:btnDone];
	[btnDone release];

#if 0
    UIBarButtonItem* btnCancel = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:m_pIMEDelegate action:@selector(cancel)];
    [barItems addObject:btnCancel];
    [btnCancel release];
#endif

    /* 构造一个影子输入框 */
    m_pPlatformTextField = [[UITextField alloc] initWithFrame:CGRectMake(.0f, .0f, .0f, .0f)];
    [[NextGenEngine sharedInstance].glView addSubview:m_pPlatformTextField];
	m_pPlatformTextField.inputAccessoryView = keyboardBar;
    
	[keyboardBar setItems:barItems];
	[barItems release];
	[keyboardBar release];
    
}

void asIMEImplCocoa::ShowIME() {
    [m_pPlatformTextField becomeFirstResponder];
    [m_pToolbarTextField becomeFirstResponder];
}

void asIMEImplCocoa::HideIME() {
    if ([m_pToolbarTextField canResignFirstResponder]) {
        [m_pToolbarTextField resignFirstResponder];
    }
    if ([m_pPlatformTextField canResignFirstResponder]) {
        [m_pPlatformTextField resignFirstResponder];
    }
}

void asIMEImplCocoa::SetText(const char* text) {
    m_pToolbarTextField.text = [NSString stringWithUTF8String:text];
}

void asIMEImplCocoa::SetPlaceHolder(const char* text) {
    m_pToolbarTextField.placeholder = [NSString stringWithUTF8String:text];
}

void asIMEImplCocoa::SetSceretMode(boolean secret) {
    HideIME(); /* 更新Sceret状态需要resign输入框。 */

    m_pToolbarTextField.secureTextEntry = secret;// ? YES : NO;
}

void asIMEImplCocoa::SetKeyboardType(asIME::KeyboardType keyboardType) {
    switch (keyboardType) {
        case asIME::keyboardNum:
            m_pToolbarTextField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
            break;
        case asIME::keyboardDefault:
        default:
            m_pToolbarTextField.keyboardType = UIKeyboardTypeDefault;
            break;
    }
}

