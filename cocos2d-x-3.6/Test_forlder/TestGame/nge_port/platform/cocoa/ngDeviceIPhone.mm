/*
 *  ngDeviceIPhone.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#import <UIKit/UIKit.h>
#include "ngDeviceIPhone.h"
#import <mach/mach.h>
#import <mach/mach_host.h>
#include <utils/base64.h>
#include <core/NGE_Macros.h>
#import <platform/cocoa/platform_interface/NextGenEngine.h>

#define kLocalPushChannelKey @"localpushchannel"

#pragma mark - a temp keychain solution for device id.

#import <Security/Security.h>

#include <cocos2d.h>

static const char* kDeviceID = "asKeychainStorageKeyDeviceID";

@interface asKeychainStorage : NSObject

+ (void)save:(NSString *)service data:(id)data;
+ (id)load:(NSString *)service;
+ (void)remove:(NSString *)service;

@end

@implementation asKeychainStorage

+ (NSMutableDictionary *)getKeychainQuery:(NSString *)service {
    return [NSMutableDictionary dictionaryWithObjectsAndKeys:
            (id)kSecClassGenericPassword,(id)kSecClass,
            service, (id)kSecAttrService,
            service, (id)kSecAttrAccount,
            (id)kSecAttrAccessibleAfterFirstUnlock,(id)kSecAttrAccessible,
            nil];
}

+ (void)save:(NSString *)service data:(id)data {
    //Get search dictionary
    NSMutableDictionary *keychainQuery = [self getKeychainQuery:service];
    //Delete old item before add new item
    SecItemDelete((CFDictionaryRef)keychainQuery);
    //Add new object to search dictionary(Attention:the data format)
    [keychainQuery setObject:[NSKeyedArchiver archivedDataWithRootObject:data] forKey:(id)kSecValueData];
    //Add item to keychain with the search dictionary
    SecItemAdd((CFDictionaryRef)keychainQuery, NULL);
}

+ (id)load:(NSString *)service {
    id ret = nil;
    NSMutableDictionary *keychainQuery = [self getKeychainQuery:service];
    //Configure the search setting
    //Since in our simple case we are expecting only a single attribute to be returned (the password) we can set the attribute kSecReturnData to kCFBooleanTrue
    [keychainQuery setObject:(id)kCFBooleanTrue forKey:(id)kSecReturnData];
    [keychainQuery setObject:(id)kSecMatchLimitOne forKey:(id)kSecMatchLimit];
    CFDataRef keyData = NULL;
    if (SecItemCopyMatching((CFDictionaryRef)keychainQuery, (CFTypeRef *)&keyData) == noErr) {
        @try {
            ret = [NSKeyedUnarchiver unarchiveObjectWithData:(NSData *)keyData];
        } @catch (NSException *e) {
            NSLog(@"Unarchive of %@ failed: %@", service, e);
        } @finally {
        }
    }
    if (keyData)
        CFRelease(keyData);
    return ret;
}

+ (void)remove:(NSString *)service {
    NSMutableDictionary *keychainQuery = [self getKeychainQuery:service];
    SecItemDelete((CFDictionaryRef)keychainQuery);
}


@end

#pragma mark -

ngDevice* ngDevice::CreateInstance()
{
	ngDevice::m_pThis = DNEW(ngDeviceIPhone);
	return ngDevice::m_pThis;
}

void ngDevice::ReleaseInstance(void *pObj)
{
	ngDeviceIPhone* pThis = (ngDeviceIPhone *)pObj;
	DELETE_OBJECT(pThis, ngDeviceIPhone);
}

ngDeviceIPhone::ngDeviceIPhone() {

}

ngDeviceIPhone::~ngDeviceIPhone()
{
    
}

void ngDeviceIPhone::Initialize()
{
//	sprintf(m_deviceID, "%s", [[[UIDevice currentDevice] uniqueDeviceIdentifier] UTF8String]);
}

boolean ngDeviceIPhone::IsFileExist(const char* file)
{
    NSString* path = [[NSString alloc] initWithUTF8String: file];
	
	NSString* filePath = nil;
	
	if(![path isAbsolutePath])
	{
		NSArray *paths = nil;
		//search documents directory
		paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);		
		NSString *documentsDirectory = [paths objectAtIndex:0];
		filePath = [documentsDirectory stringByAppendingPathComponent:path];
		BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
        if (fileExists == YES) {
            [path release];
            return TRUE;
        }
    }
    
    [path release];
    return FALSE;
}

boolean ngDeviceIPhone::GetFilePath(ngStringV2& outPath, const char* file) {
	ngStringV2 fileName(file);
    return FindFilePath(outPath, fileName);
}

boolean ngDeviceIPhone::FindFilePath(ngStringV2& out, const ngStringV2& in)
{
#if 1
    /* 统一使用CCFileUtils处理文件加载。 */
    out = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(in.GetCString()).c_str();
    return TRUE; /* 忽略返回值 */
#else
	NSString* path = [[NSString alloc] initWithUTF8String: in.GetCString()];
	
	NSString* filePath = nil;
	
	if(![path isAbsolutePath])
	{
		NSArray *paths = nil;
		// 1. search documents directory
		paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);		
		NSString *documentsDirectory = [paths objectAtIndex:0];
		filePath = [documentsDirectory stringByAppendingPathComponent:path];
		BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
		if (fileExists == NO) {
			// 2. search cache directory
			paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
			documentsDirectory = [paths objectAtIndex:0];
			NSString* version = [NSString stringWithFormat:@"%d", 1 /*ngApp::GetGameVersion() */]; //FIXME: here temp disable.
			filePath = [documentsDirectory stringByAppendingPathComponent:version];
			filePath = [filePath stringByAppendingPathComponent:path];
			fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
			if (fileExists == NO)
			{
				// 3. search bundle.
				filePath = [[NSBundle mainBundle] pathForResource:path ofType:nil];	
				fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filePath];
				
				if (fileExists == NO)
				{
					[path release];
					return FALSE;
				}
			}
		}
	}
	
	out = [filePath UTF8String];
	
	[path release];
	return TRUE;
#endif
}

void ngDeviceIPhone::GetFileWritePath(ngStringV2& out, const char* file)
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	const char* dir = [documentsDirectory UTF8String];
	
	out = dir;
	out += "/";
	out += file;
	
	printf("%s\n", out.GetCString(TRUE));
	
}

void ngDeviceIPhone::GetFileCachePath(ngStringV2& out, const char* file)
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	
	const char* dir = [documentsDirectory UTF8String];
	
	out = dir;
	out += "/";
	out += file;
	
	printf("%s\n", out.GetCString(TRUE));
	
}

NGCSTR ngDeviceIPhone::CurrentLocaleLanguage()
{
	NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
	NSArray* languages = [defs objectForKey:@"AppleLanguages"];
	NSString* locale = [languages objectAtIndex:0];
	/*
	NSString* locale = [[NSLocale currentLocale] localeIdentifier];
	NSLog(@"%@", locale);
	 */
	NSArray* langs = [locale componentsSeparatedByString:@"_"];
	NSString* lang = (NSString *)[langs objectAtIndex:0];
	langs = [lang componentsSeparatedByString:@"-"];
	lang = (NSString *)[langs objectAtIndex:0];
	return [lang UTF8String];
}

NGCSTR ngDeviceIPhone::CurrentLocaleRegion()
{
	NSString* locale = [[NSLocale currentLocale] localeIdentifier];
	NSArray* langs = [locale componentsSeparatedByString:@"_"];
	NSString* lang = (NSString *)[langs objectAtIndex:0];
	langs = [lang componentsSeparatedByString:@"-"];
	lang = (NSString *)[langs objectAtIndex:0];
	return [lang UTF8String];
}

void ngDeviceIPhone::LaunchURL(const char* url)
{
	NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
	[[UIApplication sharedApplication] openURL:nsurl];
}

const char* ngDeviceIPhone::GetDeviceType()
{
	return [[[UIDevice currentDevice] model] UTF8String];
}

const char* ngDeviceIPhone::GetDeviceVersion() 
{
	return [[[UIDevice currentDevice] systemVersion] UTF8String];
}

float ngDeviceIPhone::GetDeviceVersionf() {
    return [[[UIDevice currentDevice] systemVersion] floatValue];
}

const char* ngDeviceIPhone::GetDeviceID() {
	//return [[[UIDevice currentDevice] uniqueIdentifier] UTF8String];


    if (m_deviceID[0] != '\0') {
        return m_deviceID;
    }

    //load from keychain
    NSString* savedDeviceID = [asKeychainStorage load: [NSString stringWithUTF8String: kDeviceID]];
    if (savedDeviceID != nil) {
        strncpy(m_deviceID, [savedDeviceID UTF8String], MAX_DEVICE_ID_LEN - 1);
        return m_deviceID;
    }

    /* here temp use uuid instead of udid. */
    CFUUIDRef uuidRef = CFUUIDCreate(kCFAllocatorDefault);
    NSString *uuidString = (NSString *) CFUUIDCreateString(NULL, uuidRef);
    CFRelease(uuidRef);

    strncpy(m_deviceID, [uuidString UTF8String], MAX_DEVICE_ID_LEN - 1);

    //save to keychain
    [asKeychainStorage save: [NSString stringWithUTF8String: kDeviceID] data: uuidString];

    [uuidString release];

    return m_deviceID;
}

const char* ngDeviceIPhone::GetDeviceFamily()
{
    return "iOS"; 
}

#if 0
const char* ngDeviceIPhone::GetDevicePlatform() {
    
    /*
     
     here is a list from https://github.com/erica/uidevice-extension/blob/master/UIDevice-Hardware.m , some is useful.
     
     Platforms
     
     iFPGA ->        ??
     
     iPhone1,1 ->    iPhone 1G, M68
     iPhone1,2 ->    iPhone 3G, N82
     iPhone2,1 ->    iPhone 3GS, N88
     iPhone3,1 ->    iPhone 4/AT&T, N89
     iPhone3,2 ->    iPhone 4/Other Carrier?, ??
     iPhone3,3 ->    iPhone 4/Verizon, TBD
     iPhone4,1 ->    (iPhone 4S/GSM), TBD
     iPhone4,2 ->    (iPhone 4S/CDMA), TBD
     iPhone4,3 ->    (iPhone 4S/???)
     iPhone5,1 ->    iPhone Next Gen, TBD
     iPhone5,1 ->    iPhone Next Gen, TBD
     iPhone5,1 ->    iPhone Next Gen, TBD
     
     iPod1,1   ->    iPod touch 1G, N45
     iPod2,1   ->    iPod touch 2G, N72
     iPod2,2   ->    Unknown, ??
     iPod3,1   ->    iPod touch 3G, N18
     iPod4,1   ->    iPod touch 4G, N80
     
     // Thanks NSForge
     iPad1,1   ->    iPad 1G, WiFi and 3G, K48
     iPad2,1   ->    iPad 2G, WiFi, K93
     iPad2,2   ->    iPad 2G, GSM 3G, K94
     iPad2,3   ->    iPad 2G, CDMA 3G, K95
     iPad3,1   ->    (iPad 3G, WiFi)
     iPad3,2   ->    (iPad 3G, GSM)
     iPad3,3   ->    (iPad 3G, CDMA)
     iPad4,1   ->    (iPad 4G, WiFi)
     iPad4,2   ->    (iPad 4G, GSM)
     iPad4,3   ->    (iPad 4G, CDMA)
     
     AppleTV2,1 ->   AppleTV 2, K66
     AppleTV3,1 ->   AppleTV 3, ??
     
     i386, x86_64 -> iPhone Simulator
     */

    if (m_platformStr.IsEmpty()) {
        size_t size;
        sysctlbyname("hw.machine", NULL, &size, NULL, 0);
        char* machine = (char*)malloc(size);
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
     
        m_platformStr = machine;
        
        free(machine);
    }

    NG_DEBUG_LOG("[device] device is : %s\n", m_platformStr.GetCString());

    /* return nick name */
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone1,1"))   return "iPhone 1G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone1,2"))   return "iPhone 3G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone2,1"))   return "iPhone 3GS";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone3,1"))   return "iPhone 4 (AT&T)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone3,2"))   return "iPhone 4 (Other Carrier)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone3,3"))   return "iPhone 4 (Verizon)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone4,1"))   return "iPhone 4S (GSM)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone4,2"))   return "iPhone 4S (CDMA)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone4,3"))   return "iPhone 4S (???)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPhone5,1"))   return "iPhone 5";
    if (0 == strcmp(m_platformStr.GetCString(), "iPod1,1"))     return "iPod Touch 1G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPod2,1"))     return "iPod Touch 2G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPod3,1"))     return "iPod Touch 3G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPod4,1"))     return "iPod Touch 4G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPod5,1"))     return "iPod Touch 5G";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad1,1"))     return "iPad 1";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad2,1"))     return "iPad 2 (WiFi)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad2,2"))     return "iPad 2 (GSM)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad2,3"))     return "iPad 2 (CDMA)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad3,1"))     return "new iPad (WiFi)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad3,2"))     return "new iPad (GSM)";
    if (0 == strcmp(m_platformStr.GetCString(), "iPad3,3"))     return "new iPad (CDMA)";

    if (0 == strcmp(m_platformStr.GetCString(), "i386"))        return "Simulator";
    if (0 == strcmp(m_platformStr.GetCString(), "x86_64"))      return "Simulator";

    /* return machine name if no nick name. */
    return m_platformStr.GetCString();
}
#endif

NGCSTR ngDeviceIPhone::GetChannelID() const {

    /* ignore channel id change in runtime. */
    if (strlen(m_channelID) > 0) {
        return m_channelID;
    }
    
    NSDictionary* dict = [[NSBundle mainBundle] infoDictionary];
    NSString* strChannelID = [dict objectForKey:@"ANANSI_CHANNEL"];
    
    if (strChannelID == Nil) {
        strncpy((char*)m_channelID, "ios", MAX_CHANNEL_ID_LEN);
    } else {
        strncpy((char*)m_channelID, [strChannelID UTF8String], MAX_CHANNEL_ID_LEN);
    }
    
    return m_channelID;
}

NGCSTR ngDeviceIPhone::GetReleaseVersion() const {
    /* ignore channel id change in runtime. */
    if (strlen(m_releaseVersion) > 0) {
        return m_releaseVersion;
    }
    
    NSDictionary* dict = [[NSBundle mainBundle] infoDictionary];
    NSString* strReleaseVersion = [dict objectForKey:@"CFBundleShortVersionString"];
    
    if (strReleaseVersion == Nil) {
        strncpy((char*)m_releaseVersion, "", MAX_CHANNEL_ID_LEN);
    } else {
        strncpy((char*)m_releaseVersion, [strReleaseVersion UTF8String], MAX_CHANNEL_ID_LEN);
    }
    
    return m_releaseVersion;
}

void ngDeviceIPhone::RegisterPushNotification()
{
    [[NextGenEngine sharedInstance] registerPushNotification];
}

#if 0

#define ENABLE_LOCALPUSH_ALONE  1

void ngDeviceIPhone::PushLocalNotification(int64 fireTimeFromNow, ngStringV2* msg, int32 channel, NGCSTR param)
{
#if !ENABLE_LOCALPUSH_ALONE
	UIRemoteNotificationType type = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
	if (type  == UIRemoteNotificationTypeNone) {    //if want to push local alert force, skip this check.
		return;
	}
#endif

    /* 如果注册的时间小于5分钟，则不注册 */
//    if (fireTimeFromNow <= 300 ) {
//        return;
//    }
	
	UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    if (localNotif == nil)
        return;
	NSDate* itemDate = [[NSDate alloc] initWithTimeIntervalSinceNow:fireTimeFromNow];
    localNotif.fireDate = itemDate;
    localNotif.timeZone = [NSTimeZone localTimeZone];
	
	// Notification details
	NSString* text = [NSString stringWithUTF8String:msg->GetCString()];
    localNotif.alertBody = text;
	// Set the action button
	//    localNotif.alertAction = @"View";

#if ENABLE_LOCALPUSH_ALONE
    localNotif.soundName = UILocalNotificationDefaultSoundName; //UILocalNotificationDefaultSoundName is available from iOS 4.0
#else
	if ((type & UIRemoteNotificationTypeSound) != 0) {
		localNotif.soundName = UILocalNotificationDefaultSoundName; //UILocalNotificationDefaultSoundName is available from iOS 4.0
	}
#endif
    localNotif.applicationIconBadgeNumber = 1;
    
    NSMutableDictionary *userDict = [NSMutableDictionary dictionaryWithObject: [NSNumber numberWithInt:channel] forKey:kLocalPushChannelKey];
	
	// Specify custom data for the notification
	if (param != NULL) {        
        [userDict setObject:[NSString stringWithUTF8String:param] forKey:@"default"];
	}
    
    localNotif.userInfo = userDict;

    // Cancel all notifications with the same channel
    NSArray* notifications = [UIApplication sharedApplication].scheduledLocalNotifications;
    for (UILocalNotification* noti in notifications) {
        NSDictionary* dict = noti.userInfo;
        NSNumber* number = (NSNumber *)[dict objectForKey:kLocalPushChannelKey];
        if (number != nil) {
            int ch = [number intValue];
            if (ch == channel) {
                [[UIApplication sharedApplication] cancelLocalNotification:noti];
            }
        }
    }
	
	// Schedule the notification
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
	
	[itemDate release];
    [localNotif release];
	
	NG_DEBUG_LOG("[Local Push]register in %lld seconds, message is %s", fireTimeFromNow, msg->GetCString());
}

void ngDeviceIPhone::CancelLocalNotification(int32 channel, NGCSTR param)
{
    NSArray* notifications = [UIApplication sharedApplication].scheduledLocalNotifications;
    for (UILocalNotification* noti in notifications) {
        NSDictionary* dict = noti.userInfo;
        if (param != NULL) {
            NSString* paramStr = (NSString *)[dict objectForKey:@"default"];
            NGCSTR localParam = [paramStr UTF8String];
            if (localParam != NULL && strncmp(localParam, param, strlen(param)) == 0) {
                [[UIApplication sharedApplication] cancelLocalNotification:noti];
            }
        }
        
        NSNumber* number = (NSNumber *)[dict objectForKey:kLocalPushChannelKey];
        if (number != nil) {
            int ch = [number intValue];
            if (ch == channel) {
                [[UIApplication sharedApplication] cancelLocalNotification:noti];
            }
        }
    }
}
#endif

int64 ngDeviceIPhone::GetFreeMemory() {
    mach_port_t           host_port = mach_host_self();
    mach_msg_type_number_t   host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t               pagesize;
    vm_statistics_data_t     vm_stat;
    
    host_page_size(host_port, &pagesize);
    
    if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS) NSLog(@"Failed to fetch vm statistics");
    
    natural_t   mem_free = vm_stat.free_count * pagesize;

#if defined (DEBUG) && 0
    natural_t   mem_used = (vm_stat.active_count + vm_stat.inactive_count + vm_stat.wire_count) * pagesize;
    natural_t   mem_total = mem_used + mem_free;    
    printf("MEM: %u TOTAL %u FREE\n", mem_total, mem_free);
#endif
    
    return mem_free;
}

