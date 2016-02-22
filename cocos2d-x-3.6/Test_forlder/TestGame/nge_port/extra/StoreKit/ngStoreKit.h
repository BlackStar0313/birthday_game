
/*
 *  ngStoreKit.h
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */
#ifndef __NGSTOREKIT_H__
#define __NGSTOREKIT_H__

#include "NGE_Types.h"
#include "ngStringV2.h"
#include "ngSingleton.h"
#include "ngArrayList.h"


#define USE_STOREKIT_ITEM

class ngStoreKitListener 
{
public:
    virtual void OnPurchaseSuccess(NGCSTR receipt, NGCSTR productId) = 0;
    virtual void OnPurchaseFailed() = 0;
    virtual void OnProductListFinished(ngArrayList *pList) = 0;
};

class ngStoreKitItem
{
public:
    ngStoreKitItem(NGCSTR price, NGCSTR productId);
    ~ngStoreKitItem();

    ngStringV2& GetPrice();
    ngStringV2& GetProductId();

private:
    ngStringV2 m_price;
    ngStringV2 m_prodcutId;
};

class ngStoreKit : ngSingleton
{
public:
    enum StoreKitStatus {
        SKIdle,
        SKGotProducts,
    };
public:
    static ngStoreKit* GetInstance();
    static void ReleaseInstance(void *pObj);
    virtual void Purchase(NGCSTR productId) = 0;
    virtual void List(ngList& productIds) = 0;
    virtual void RestoreAll() = 0;
	virtual void PurchaseFortumo(NGCSTR extraInfo){};
    void SetListener(ngStoreKitListener* pListener);
    ngList* GetProducts();
    
    /* load&update user purchase info reserved for non-consumable IAP */
    //<<
    virtual boolean IsFeaturePaid(NGCSTR productId) { return FALSE;};
    virtual void UpdateFeaturePaid(NGCSTR productId) {};
    //>>

public:
    void OnPaymentFinished(NGCSTR receipt, NGCSTR productId);
    void OnPaymentFailed();
    void OnProduct(NGCSTR price);
    void OnProduct(NGCSTR price, NGCSTR pid);
    void OnProductListFinished();

protected:
    ngStoreKit();
    virtual ~ngStoreKit();
    void ClearProducts();

protected:
    ngStoreKitListener* m_pListener;
    ngArrayList m_products;
    int m_status;
};


#endif //__NGSTOREKIT_H__
