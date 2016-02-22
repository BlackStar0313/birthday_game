/*
 *  ngStoreKitCocoa.cpp
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#import "ngStoreKitCocoa.h"
#import "StoreKitDelegate.h"
#import <core/NGE_Types.h>

static ngStoreKitCocoa* sInstance = NULL;
static StoreKitDelegate* storeKitDelegate = NULL;

ngStoreKit* ngStoreKit::GetInstance()
{
	if (sInstance == NULL) {
		sInstance = DNEW(ngStoreKitCocoa);		
	}
	return sInstance;
}

void ngStoreKit::ReleaseInstance(void *pObj)
{
	DELETE_OBJECT((ngStoreKitCocoa *)pObj, ngStoreKitCocoa);
}

ngStoreKitCocoa::ngStoreKitCocoa()
{
	if (storeKitDelegate != nil) {
		[storeKitDelegate release];
		storeKitDelegate = nil;
	}
	storeKitDelegate = [[StoreKitDelegate alloc] init:this];	
	m_pListener = NULL;
}

void ngStoreKitCocoa::Purchase(NGCSTR productId)
{
	[storeKitDelegate purchase:productId];
}

void ngStoreKitCocoa::List(ngList &productIds)
{
    ClearProducts();
    
    NSMutableSet* set = [[NSMutableSet alloc] initWithCapacity:productIds.Size()];
    ngIterator* pIterator = productIds.Iterator();
    while (pIterator->HasNext()) {
        ngStringV2* p = (ngStringV2 *)pIterator->Next();
        NSString* pp = [[NSString alloc] initWithUTF8String:p->GetCString()];
        [set addObject:pp];
        [pp release];
    }
    pIterator->Destroy();
    [storeKitDelegate list:set];
    [set release];
}

void ngStoreKitCocoa::RestoreAll()
{
    [storeKitDelegate restoreTransactions];
}

boolean ngStoreKitCocoa::IsFeaturePaid(NGCSTR productId)
{
    
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
	BOOL isPaid = [userDefaults boolForKey:[[[NSString alloc] initWithUTF8String:productId] autorelease]];
    
    if (YES == isPaid) {
        return TRUE;
    } else {
        return FALSE;
    }
    
    return FALSE;
}

void ngStoreKitCocoa::UpdateFeaturePaid(NGCSTR productId)
{
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    
	[userDefaults setBool:YES forKey:[[[NSString alloc] initWithUTF8String:productId] autorelease]];
}