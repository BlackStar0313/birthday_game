/*
 *  StoreKitDelegate.cpp
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#import "StoreKitDelegate.h"
#import "ngStoreKit.h"
#import "SKProduct+LocalizedPrice.h"

@implementation StoreKitDelegate

enum StoreKitStatus {
    SK_LIST,
    SK_PURCHASE,
    SK_RESTORE,
};

- (id)init:(ngStoreKit *)kit {
	if ((self = [super init]) != nil) {
		storekit = kit;
		[StoreKitInterface sharedStore].delegate = self;
        status = SK_LIST;
	}
	return self;
	
}

- (void)list:(NSSet *)ids {
    status = SK_LIST;
    [[StoreKitInterface sharedStore] requestProducts:ids];
}

- (void)purchase:(const char*)productId {
	status = SK_PURCHASE;
	[[StoreKitInterface sharedStore] requestProduct:[NSString stringWithUTF8String:productId]];
}

- (void)restoreTransactions {
    status = SK_RESTORE;
    [[StoreKitInterface sharedStore] restoreTransactions];
}

#pragma mark callback
- (void)productRequestComplete {
    switch (status) {
        case SK_LIST:
        {
            for (SKProduct* p in [StoreKitInterface sharedStore].purchaseableProducts) {
                NSString* price = [p localizedPrice];
                NSLog(@"price :%@", price);
#ifdef USE_STOREKIT_ITEM
                storekit->OnProduct([price UTF8String], [p.productIdentifier UTF8String]);
#else
                storekit->OnProduct([price UTF8String]);
#endif
            }
            storekit->OnProductListFinished();
            break;
        }
            
        case SK_PURCHASE:
        {
            if ([[StoreKitInterface sharedStore].purchaseableProducts count] > 0) {
                [[StoreKitInterface sharedStore] purchaseProduct:[[StoreKitInterface sharedStore].purchaseableProducts objectAtIndex:0]];
            } else {
                NSLog(@"payment failed, no pruchaseableProduct.");                
                storekit->OnPaymentFailed();
            }
            break;
        }
            
        case SK_RESTORE:
        {
            break;
        }
            
        default:
            break;
    }
}

- (void)paymentDidFinish:(id)sender withProductId:(NSString *)productId {
	NSLog(@"payment finished");
	
	NSString* receipt = (NSString *)sender;
	storekit->OnPaymentFinished([receipt UTF8String], [productId UTF8String]);
	
}

- (void)paymentDidFailed:(id)sender {
	NSLog(@"payment failed");
	
	storekit->OnPaymentFailed();
}

- (void)paymentTransactionComplete:(SKPaymentTransaction *)transaction {
}

- (void)restoredAllTransactions {
}


@end