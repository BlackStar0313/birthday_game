/*
 *  StoreKitDelegate.h
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */

#import "StoreKitInterface.h"

class ngStoreKit;
@interface StoreKitDelegate : NSObject <StoreKitInterfaceCallbacks> {	
	ngStoreKit* storekit;
    int status;
}

- (id)init:(ngStoreKit *)kit;
- (void)purchase:(const char*)productId;
- (void)list:(NSSet *)ids;
- (void)restoreTransactions;

#pragma mark callback
- (void)productRequestComplete;
- (void)paymentDidFinish:(id)sender withProductId:(NSString *)productId;
- (void)paymentDidFailed:(id)sender;
- (void)paymentTransactionComplete:(SKPaymentTransaction *)transaction;
- (void)restoredAllTransactions;


@end