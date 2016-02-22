/*
 *  ngStoreKit.h
 *  template
 *
 *  Created by Yuhui Wang on 9/7/10.
 *  Copyright 2010 Anansi Mobile. All rights reserved.
 *
 */
#ifndef __NGSTOREKITCOCOA_H__
#define __NGSTOREKITCOCOA_H__

#include <core/NGE_Types.h>
#include <extra/StoreKit/ngStoreKit.h>
#include <nsl/ngStringV2.h>

class ngStoreKitCocoa : public ngStoreKit
{
public:
	ngStoreKitCocoa();
	virtual void Purchase(NGCSTR productId);
    virtual void List(ngList& productIds);
    virtual void RestoreAll();
    
    /* load&update user purchase info */
    //<<
    virtual boolean IsFeaturePaid(NGCSTR productId);
    virtual void UpdateFeaturePaid(NGCSTR productId);
    //>>
};


#endif //__NGSTOREKITCOCOA_H__
