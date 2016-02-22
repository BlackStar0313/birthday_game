/*
 *  ngStoreKitMetro.h
 *  template
 *
 *  Created by Yuhui Wang on 10/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __NGSTOREKITANDROID_H__
#define __NGSTOREKITANDROID_H__

#include "NGE_Types.h"
#include "ngStoreKit.h"
#include "ngStringV2.h"

using namespace Windows::ApplicationModel::Store;

class ngStoreKitMetro : public ngStoreKit 
{
public:
    ngStoreKitMetro();

    virtual void Purchase(NGCSTR productId);
    virtual void List(ngList& productIds);
	virtual void PurchaseFortumo(NGCSTR extraInfo);

private:
	ListingInformation^ m_productInfoListing;


#ifdef MANAGED_ENGINE
	Platform::String ^ m_pid;
	void SetEventListener();
	void RemoveEventListener();
	Windows::Foundation::EventRegistrationToken m_listToken;
	Windows::Foundation::EventRegistrationToken m_purchaseToken;
	Windows::Foundation::EventRegistrationToken m_purchaseFortumoToken;
#endif // MANAGED_ENGINE

};


#endif //__NGSTOREKITANDROID_H__
