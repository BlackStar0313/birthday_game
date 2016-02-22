//
//  StoreKitInterface.m
//  NinHao
//
//  Created by Yuhui Wang on 3/29/10.
//  Copyright Anansi Mobile 2010. All rights reserved.
//

#import "StoreKitInterface.h"
#import "JSON.h"
#import "NSStringAdditions.h"

// Notification!
NSString * const NinhaoProductPurchasedNotification = @"NinhaoProductPurchasedNotification";

//const NSString * GOTestProdcutId1 = @"com.galaxyodyssey.inapppurchasetest.p1";
//const NSString * GOTestProdcutId2 = @"com.galaxyodyssey.inapppurchasetest.p2";

@interface StoreKitInterface ()
@property (nonatomic, copy, readwrite) NSArray *purchaseableProducts;
@end

@implementation StoreKitInterface
@synthesize purchaseableProducts;
@synthesize delegate;
@synthesize receivedData;

- (id)init
{
	NSLog(@"StoreKitInterface is a singleton.  Use +sharedStore.");
	[self release];
	return nil;
}

- (id)initPrivate
{
	if (self = [super init])
	{
		// Double check we can make payments from this device!
		if ([SKPaymentQueue canMakePayments] == NO)
		{
			NSLog(@"OH FUCK WE CAN'T MAKE PAYMENTS!@!");
		}
		
		// Add ourselves as an observer of SKPaymentQueue
		[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
#if 0		
		// Request the list of IAP products.
		NSSet *productIDSet = [NSSet setWithObjects:GOTestProdcutId1, /*GOTestProdcutId2,*/ nil];
		
		currentProductRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIDSet];
		currentProductRequest.delegate = self;
		[currentProductRequest start];
#endif
		
	}
	return self;
}

+ (StoreKitInterface *)sharedStore
{
	static StoreKitInterface *store = nil;
#if !TARGET_IPHONE_SIMULATOR
	if (store == nil)
	{
		store = [[StoreKitInterface alloc] initPrivate];
	}
#endif
	return store;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)requestProducts:(NSSet *)productIDs
{
    currentProductRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIDs];
	currentProductRequest.delegate = self;
	[currentProductRequest start];
}

- (void)requestProduct:(NSString *)productID
{
	// Request the list of IAP products.
	NSSet *productIDSet = [NSSet setWithObjects:productID, nil];
	
	currentProductRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIDSet];
	currentProductRequest.delegate = self;
	[currentProductRequest start];
	
}

- (void)purchaseProduct:(SKProduct *)product
{
	if ([SKPaymentQueue canMakePayments] == NO)
	{
		NSLog(@"Attempted to purchaseProduct:%@ while unable to make payments!", product.productIdentifier);
		return;
	}
	
	NSLog(@"Adding payment for %@ to queue!", product.productIdentifier);
	
	SKPayment *payment = [SKPayment paymentWithProduct:product];
	[[SKPaymentQueue defaultQueue] addPayment:payment];
}

- (BOOL)hasPurchasedProductID:(NSString *)productID
{
	return NO; //[[KeychainWrapper sharedWrapper] productPurchased:productID];
}

- (void)finishTransaction:(NSString *)identifier {
//    [SKPaymentQueue defaultQueue] 
}

- (void)restoreTransactions
{
	[[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)resetPurchases
{
	for (SKProduct *product in self.purchaseableProducts)
	{
		//[[KeychainWrapper sharedWrapper] removePurchasedProduct:product.productIdentifier];
	}
}

#pragma mark SKRequestDelegate
- (void)requestDidFinish:(SKRequest *)request
{
		NSLog(@"requestDidFinish:%@", request);
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
	NSLog(@"request:%@ didFailWithError:%@", request, error);
}

#pragma mark SKProductsRequestDelegate
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
	NSLog(@"Received product request respose");
	
	// Initialize our purchaseableProducts array
	self.purchaseableProducts = response.products;
	
	// If we have a unlockFeaturesVC set, that means the VC was waiting for us to finish retreiving the product
	// information.  Tell it we're done!
	if ([self.delegate respondsToSelector:@selector(productRequestComplete)])
	{
		[self.delegate productRequestComplete];
	}
	
	if ([response.invalidProductIdentifiers count])
	{
#ifdef DEBUG_LOG
		NSLog(@"Got invalid ones: %@", response.invalidProductIdentifiers);
#endif
	}
	
	if (request == currentProductRequest)
	{
		[currentProductRequest release];
		currentProductRequest = nil;
	}
}

#pragma mark SKPaymentTransactionObserver
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	// Go through our transactions and, for the successful ones, unlock the features!
	for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
			NSLog(@"transaction state: %d", transaction.transactionState);
			case SKPaymentTransactionStatePurchasing:
			{
				break;
			}
				
			case SKPaymentTransactionStatePurchased:
			case SKPaymentTransactionStateRestored:
			{
				// We bought this feature! YAY!
				//[[KeychainWrapper sharedWrapper] setProductPurchased:transaction.payment.productIdentifier];
				
				// Send a notification to any living objects that may be interested in knowing about this,
				// in order to update their UI or state or something.
				
				[queue finishTransaction:transaction];
				
				// Verify the receipt
				NSData* receipt = transaction.transactionReceipt;
				NSString *base64 = [NSString base64StringFromData:receipt length:[receipt length]];
#ifdef DEBUG_LOG
				NSLog(@"Base64: %@", base64);
#endif
                NSString *productId = transaction.payment.productIdentifier;
				
//				[[NSNotificationCenter defaultCenter] postNotificationName:NinhaoProductPurchasedNotification object:[[base64 copy] autorelease]];
				
				if ([self.delegate respondsToSelector:@selector(paymentDidFinish: withProductId:)])
				{
					[self.delegate paymentDidFinish:base64 withProductId:productId];
				}
/*				
				//NSString* strReceipt =  [[[NSString alloc] initWithData:receipt encoding:NSUTF8StringEncoding] autorelease];
				NSDictionary *tempDict = [NSDictionary dictionaryWithObject:base64 forKey:@"receipt-data"];
				NSString *jsonValue = [tempDict JSONRepresentation];
				NSLog(@"Json: %@", jsonValue);
				//NSData *jsonData = [jsonValue dataUsingEncoding:NSUTF8StringEncoding];
				NSURL *sandboxStoreURL = [[NSURL alloc] initWithString: @"https://sandbox.itunes.apple.com/verifyReceipt"];
				NSData *postData = [NSData dataWithBytes:[jsonValue UTF8String] length:[jsonValue length]];
				NSMutableURLRequest *connectionRequest = [NSMutableURLRequest requestWithURL:sandboxStoreURL];
				[connectionRequest setHTTPMethod:@"POST"];
				[connectionRequest setTimeoutInterval:120.0];
				[connectionRequest setCachePolicy:NSURLRequestUseProtocolCachePolicy];
				[connectionRequest setHTTPBody:postData];
				NSURLConnection *connection = [[NSURLConnection alloc]
											   initWithRequest:connectionRequest
											   delegate:self];
				[connection release];
*/				
				
				break;
			}
				
			case SKPaymentTransactionStateFailed:
			{
				NSLog(@"SKPaymentTransactionStateFailed: %@", transaction.error);
				[queue finishTransaction:transaction];
				if ([self.delegate respondsToSelector:@selector(paymentDidFailed:)])
				{
					[self.delegate paymentDidFailed:nil];
				}
				break;
			}
		}
		
		if ([self.delegate respondsToSelector:@selector(paymentTransactionComplete:)])
		{
			[self.delegate paymentTransactionComplete:transaction];
		}
	}
}

- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
{
	//NSLog(@"paymentQueue:%@ removedTransactions:%@", queue, transactions);
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
	NSLog(@"paymentQueue:%@ restoreCompletedTransactionsFailedWithError:%@", queue, error);
	//[ErrorReporter reportError:error forSystem:@"StoreKitInterface"];
	
	if ([self.delegate respondsToSelector:@selector(restoredAllTransactions)])
	{
		[self.delegate restoredAllTransactions];
	}
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
	if ([self.delegate respondsToSelector:@selector(restoredAllTransactions)])
	{
		[self.delegate restoredAllTransactions];
	}
}




#pragma mark connection delegate
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
//	NSLog(@"%@",  [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]);
	[self.receivedData appendData:data];
}
- (void)connectionDidFinishLoading:(NSURLConnection *)connection{
	NSLog(@"%@",  [[[NSString alloc] initWithData:self.receivedData encoding:NSUTF8StringEncoding] autorelease]);
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response{
	switch([(NSHTTPURLResponse *)response statusCode]) {
		case 200:
			self.receivedData = [[NSMutableData alloc] init];
			break;
		case 206:
			break;
		case 304:
			
			break;
		case 400:
			
			break;
			
			
		case 404:
			break;
		case 416:
			break;
		case 403:
			break;
		case 401:
		case 500:
			break;
		default:
			break;
	}	
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
	
	
}


@end
