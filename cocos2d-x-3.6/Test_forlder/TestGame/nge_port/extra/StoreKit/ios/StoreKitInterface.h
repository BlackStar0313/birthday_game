//
//  StoreKitInterface.h
//  NinHao
//
//  Created by Yuhui Wang on 3/29/10.
//  Copyright Anansi Mobile 2010. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@interface StoreKitInterface : NSObject <SKRequestDelegate, SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
	SKProductsRequest *currentProductRequest;
	NSArray *purchaseableProducts;
	NSMutableData *receivedData;
	
	id delegate;
}

@property (nonatomic, copy, readonly) NSArray *purchaseableProducts;
@property (nonatomic, retain) NSMutableData* receivedData;
@property (nonatomic, assign) id delegate;

+ (StoreKitInterface *)sharedStore;

- (void)requestProducts:(NSSet *)productIDs;
- (void)requestProduct:(NSString *)productID;
- (void)purchaseProduct:(SKProduct *)product;
- (BOOL)hasPurchasedProductID:(NSString *)productID;

- (void)finishTransaction:(NSString *)identifier;
- (void)restoreTransactions;
- (void)resetPurchases;	// NOTE: Debugging only!

// SKRequestDelegate
- (void)requestDidFinish:(SKRequest *)request;
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error;

// SKProductsRequestDelegate
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response;

// SKPaymentTransactionObserver
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;
- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions;
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error;
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue;

@end

@protocol StoreKitInterfaceCallbacks

- (void)productRequestComplete;
- (void)paymentTransactionComplete:(SKPaymentTransaction *)transaction;
- (void)restoredAllTransactions;
- (void)paymentDidFinish:(id)sender withProductId:(NSString *)productId;
- (void)paymentDidFailed:(id)sender;

@end

extern NSString * const NinhaoProductPurchasedNotification;
