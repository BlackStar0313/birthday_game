/*
 *  ngStoreKit.cpp
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */


#include "ngStoreKit.h"
#include <core/NGE_Types.h>


ngStoreKitItem::ngStoreKitItem(NGCSTR price, NGCSTR productId)
{
    m_price = price;
    m_prodcutId = productId;
}

ngStoreKitItem::~ngStoreKitItem()
{
}

ngStringV2& ngStoreKitItem::GetPrice()
{
    return m_price;
}

ngStringV2& ngStoreKitItem::GetProductId()
{
    return m_prodcutId;
}

ngStoreKit::ngStoreKit()
: ngSingleton(ReleaseInstance, this)
{
	m_pListener = NULL;
    m_status = SKIdle;
}

ngStoreKit::~ngStoreKit() {
    ClearProducts();
}

void ngStoreKit::SetListener(ngStoreKitListener* pListener)
{
	m_pListener = pListener;
}

void ngStoreKit::OnPaymentFinished(NGCSTR receipt, NGCSTR productId)
{
    if( m_pListener == NULL ) return;
	m_pListener->OnPurchaseSuccess(receipt, productId);
}

void ngStoreKit::OnPaymentFailed()
{
    if( m_pListener == NULL ) return;
	m_pListener->OnPurchaseFailed();
}

void ngStoreKit::OnProductListFinished()
{
    m_status = SKGotProducts;
    if( m_pListener == NULL ) return;
    m_pListener->OnProductListFinished(&m_products);
    ClearProducts();
}

#ifdef USE_STOREKIT_ITEM
void ngStoreKit::OnProduct(NGCSTR price, NGCSTR pid)
{
    
    ngStoreKitItem* pProduct = DNEW ngStoreKitItem(price, pid);
    m_products.InsertAtEnd(pProduct);

}
#else
void ngStoreKit::OnProduct(NGCSTR price)
{
    ngStringV2* pPrice = DNEW ngStringV2(price);
    m_products.InsertAtEnd(pPrice);
}
#endif

ngList* ngStoreKit::GetProducts()
{
    if (m_status == SKGotProducts) {
        return &m_products;
    }
    return NULL;
}

void ngStoreKit::ClearProducts() {
#ifdef USE_STOREKIT_ITEM
    LIST_CLEAR(&m_products, ngStoreKitItem);
#else
    LIST_CLEAR(&m_products, ngStringV2);
#endif

}