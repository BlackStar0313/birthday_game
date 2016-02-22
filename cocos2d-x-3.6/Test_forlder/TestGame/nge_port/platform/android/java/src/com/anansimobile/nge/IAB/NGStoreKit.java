package com.anansimobile.nge.IAB;

import java.util.LinkedList;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;

import com.anansimobile.nge.NextGenEngine;
import com.anansimobile.nge.IAB.util.IabHelper;
import com.anansimobile.nge.IAB.util.IabHelper.OnConsumeFinishedListener;
import com.anansimobile.nge.IAB.util.IabHelper.OnIabPurchaseFinishedListener;
import com.anansimobile.nge.IAB.util.IabResult;
import com.anansimobile.nge.IAB.util.IabHelper.QueryInventoryFinishedListener;
import com.anansimobile.nge.IAB.util.Inventory;
import com.anansimobile.nge.IAB.util.Purchase;
import com.anansimobile.nge.IAB.util.SkuDetails;

public class NGStoreKit {
	
	public static final int STOREKIT_REQ_PURCHASE = 4001;
	
	private static IabHelper mIabHelper = null;
	private static LinkedList<String> mProductIds = new LinkedList<String>();
	private static String savedProductId = null;
	
	private static StoreKitMsgHandler mStoreKitMsgHandler = new StoreKitMsgHandler();
	
	private static Inventory mInv = null;	//store purchase info map.
	
	public static void close() {
    	if (mIabHelper != null) {
    		mIabHelper.dispose();
    		mIabHelper = null;
    	}
	}

	/**
	 * init IabHelper instance, for In-App Billing.
	 * @param ctx activity context
	 * @param base64EncodedPublicKey public key form Developer Console.
	 */
	public static void initIabHelper(Context ctx, String base64EncodedPublicKey) {
		
		clearProductIds();

		try {
		mIabHelper = new IabHelper(ctx, base64EncodedPublicKey);
		
//		mIabHelper.enableDebugLogging(true);
		
		mIabHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
			
			@Override
			public void onIabSetupFinished(IabResult result) {
			      if (!result.isSuccess()) {
			          NextGenEngine.nge_log("[IAB] Problem setting up In-app Billing: " + result);
			       } else {
			    	   NextGenEngine.nge_log("[IAB] In-app Billing setup done!~");
			       }
			    }
			});
		
		} catch (Exception e) {
			//some exception catched.
    		mIabHelper.dispose();
			mIabHelper = null;
			NextGenEngine.nge_logf("[IAB] exception %s", e.getMessage());
		}

    }
	
	/**
	 * add product id for query detail.
	 * @param pid	product id
	 * @see queryProducts()
	 */

	public static void addProductId(String pid) {
		mProductIds.add(pid);
	}

	public static boolean isProductIdsEmpty() {
		return mProductIds.isEmpty();
	}

	public static void clearProductIds() {
		mProductIds.clear();
	}

	/**
	 * query product detail, product id is provided by arg ids. 
	 * @param ids product id array.
	 */
//	public static void queryProducts(String[] ids) {
//		final LinkedList<String> productIds = new LinkedList<String>();
//		for (int i = 0; i < ids.length; i++) {
//			productIds.add("android.test.purchased");
//		}
//		mIabHelper.queryInventoryAsync(true, productIds, mQueryFinishedListener);
//	}
	
	/**
	 * query products detail, use ids added before by invoke {@link #addProductID(String)}
	 * @see addProductID(String).
	 */
	public static void queryProducts() {
		Message msg = new Message();
		msg.what = StoreKitMsgHandler.STOREKIT_MSG_LIST_PRODUCT_DETAIL;
		msg.getData().putInt("size", mProductIds.size());
		for (int i = 0; i < mProductIds.size(); i++) {
			msg.getData().putString("id" + String.valueOf(i), mProductIds.get(i));
		}
		mStoreKitMsgHandler.sendMessage(msg);
	}
	
	/*
	 * four reserved product IDs for testing static In-app Billing responses:
	 * 
	 * android.test.purchased
	 * android.test.canceled
	 * android.test.refunded
	 * android.test.item_unavailable
	 */
	
	public static void purchaseProduct(String pid, String payload) {
		savedProductId = pid;
		if (NextGenEngine.isDebugMode()) {
			//测试环境下使用的测试productID
			pid = "android.test.purchased";
		}
		Message msg = new Message();
		msg.what = StoreKitMsgHandler.STOREKIT_MSG_PURCHASE;
		msg.getData().putString("pid", pid);
		msg.getData().putString("payload", payload);
		mStoreKitMsgHandler.sendMessage(msg);
	}
	
	public static void consumeProduct(Purchase purchase) {
		try {
			mIabHelper.consumeAsync(purchase, mConsumeFinishedListener);
		} catch (Exception e) {
			//some exception catched.
			NextGenEngine.nge_logf("[IAB] exception %s", e.getMessage());
		}
	}
	
	public static boolean HandleActivityResult(int resultCode, int requestCode, Intent data) {
		if (mIabHelper != null) {
			try {
				return mIabHelper.handleActivityResult(requestCode, resultCode, data);
			} catch (Exception e) {
				//some exception catched.
				NextGenEngine.nge_logf("[IAB] exception %s", e.getMessage());
				return false;
			}
		} else {
			return false;
		}
	}
	
	private static class StoreKitMsgHandler extends Handler {
		public static final int STOREKIT_MSG_PURCHASE			 	= 1001;
		public static final int STOREKIT_MSG_LIST_PRODUCT_DETAIL	= 1002;

	    @Override
		public void handleMessage(android.os.Message msg) {
	    	switch (msg.what) {
	    	case STOREKIT_MSG_PURCHASE:
	    		
	    		Purchase consumePurchase = null;
	    		if (mInv != null) {
	    			consumePurchase = mInv.getPurchase(msg.getData().getString("pid"));
	    		}
	    		if (consumePurchase != null) {
	    			//有未消费的product
	    			if (NGStoreKit.shouldConsumeProduct(consumePurchase.getSku())) {
	    				//如果是多次消费的product，走消费流程。
		    			consumeProduct(consumePurchase);
	    			} else {
	    				//如果是一次消费的product，直接返回成功。
	    				NotifyPurchaseFinished(consumePurchase);
	    			}
	    			break;
	    		}
	    		
	    		try {
		    		mIabHelper.launchPurchaseFlow(
		    				NextGenEngine.sMainActivity,
		    				msg.getData().getString("pid"),
		    				NGStoreKit.STOREKIT_REQ_PURCHASE,
		    				mPurchaseFinishedLinstener,
		    				msg.getData().getString("payload"));
	    		} catch (Exception e) {
	    			//some exception catched.
	    			NextGenEngine.nge_logf("[IAB] exception %s", e.getMessage());
	    			OnPaymentFailed();
	    		}
	    		break;
	    	case STOREKIT_MSG_LIST_PRODUCT_DETAIL:
	    		int size = msg.getData().getInt("size");
	    		final LinkedList<String> productIds = new LinkedList<String>();
	    		for (int i = 0; i < size; i++) {
	    			productIds.add(msg.getData().getString("id" + String.valueOf(i)));
	    		}
	    		try {
	    			mIabHelper.queryInventoryAsync(true, productIds, mQueryFinishedListener);
	    		} catch (Exception e) {
	    			//some exception catched.
	    			NextGenEngine.nge_logf("[IAB] exception %s", e.getMessage());
	    		}

	    		break;
	    	}
	    }
	}
	
	private static native boolean shouldConsumeProduct(String pid);
	private static native void OnPurchaseFinished(String receipt, String productId);
	private static native void OnPaymentFailed();
	private static native void OnProduct(String price);
	private static native void OnProductListFinished();

	private static void NotifyPurchaseFinished(Purchase purchase) {
	  //TODO: callback to lib with purchase info.
	  //craete json data, if need encript purchase as receipt
	  JSONObject receiptJson = new JSONObject();
	  try {
			if (NextGenEngine.isDebugMode()) {
				  receiptJson.put("purchaseId", purchase.getOrderId() + "." + String.valueOf(System.currentTimeMillis()));
				  receiptJson.put("productId", savedProductId);
				  receiptJson.put("purchaseDate", String.valueOf(System.currentTimeMillis()));
				  receiptJson.put("payload", purchase.getDeveloperPayload());
				  receiptJson.put("token", purchase.getToken());
			} else {
				  receiptJson.put("purchaseId", purchase.getOrderId() + "." + String.valueOf(System.currentTimeMillis()));
				  receiptJson.put("productId", savedProductId);
				  receiptJson.put("purchaseDate", String.valueOf(purchase.getPurchaseTime()));
				  receiptJson.put("payload", purchase.getDeveloperPayload());
				  receiptJson.put("token", purchase.getToken());
			}
	  } catch (JSONException e) {
		  // TODO Auto-generated catch block
		  e.printStackTrace();
	  }
	  OnPurchaseFinished(receiptJson.toString(), purchase.getSku());
	}

	/*
	private static OnConsumeFinishedListener mAutoConsumeFinishedListener = new OnConsumeFinishedListener() {

		@Override
		public void onConsumeFinished(Purchase info, IabResult result) {
			if (!result.isSuccess()) {
		         NextGenEngine.nge_logf("[IAB] Error consuming: " + result);
		      } else {

		      }
		}
		
	};
	
	*/
	
	private static OnConsumeFinishedListener mConsumeFinishedListener = new OnConsumeFinishedListener() {

		@Override
		public void onConsumeFinished(Purchase info, IabResult result) {
			if (!result.isSuccess()) {
		         NextGenEngine.nge_logf("[IAB] Error consuming: " + result);
		         OnPaymentFailed();
			} else {
				NotifyPurchaseFinished(info);
			}
		}
		
	};
	
	private static OnIabPurchaseFinishedListener mPurchaseFinishedLinstener = new OnIabPurchaseFinishedListener() {

		@Override
		public void onIabPurchaseFinished(IabResult result, Purchase info) {
			 if (result.isFailure()) {
		         NextGenEngine.nge_logf("[IAB] Error purchasing: " + result);
		         /*
				if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED) {
					//may be can not reach here.
					if (info == null) {
						Purchase savedInfo = mInv.getPurchase(savedProductId);
						if (savedInfo != null) {
							NGStoreKit.consumeProduct(savedInfo);
						} else {
							//may be can not reach here.
							OnPaymentFailed();
		        		}
					} else {
						NGStoreKit.consumeProduct(info);
		        	}
		        } else {
		        	OnPaymentFailed();
		        }
		        */
		         OnPaymentFailed();
			 } else {
				 if (NGStoreKit.shouldConsumeProduct(info.getSku())) {
					 /* 检查商品是否消费商品，如果是则直接消费 */
		    		 NGStoreKit.consumeProduct(info);
		    	 } else {
		    		 NotifyPurchaseFinished(info);
		    	 }
			 }
		}
	};

	private static QueryInventoryFinishedListener mQueryFinishedListener = new QueryInventoryFinishedListener() {

		@Override
		public void onQueryInventoryFinished(IabResult result, Inventory inv) {
			 if (result.isFailure()) {
				 NextGenEngine.nge_logf("[IAB] product list failed, err code: %s!~", result.getMessage());
		         return;
		       }
			 
			 mInv = inv;

			 for (String pid : mProductIds) {
				 SkuDetails sd = inv.getSkuDetails(pid);
				 if (sd == null) {
					 NextGenEngine.nge_logf("[IAB] get product info failed, pid: %s", pid);
					 OnProduct("");
					 continue;
				 }

				 NextGenEngine.nge_logf("[IAB] product info: id(%s), title(%s), desc(%s), price(%s)",
						 pid,
						 sd.getTitle(),
						 sd.getDescription(),
						 sd.getPrice());
				 
				 OnProduct(sd.getPrice());
			 }

			 //callback to notify list done.
			 OnProductListFinished();
		}
		
	};
}
