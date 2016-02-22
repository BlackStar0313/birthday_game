/*
 *  ngJsonRoot.h
 *  NextGenEngine
 *
 *  Created by Yuhui Wang on 5/24/10.
 *  Copyright 2010 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGJSONROOT_H__
#define __NGJSONROOT_H__

#include <core/NGE_Types.h>
#include "./ngJsonElement.h"

class ngJsonRoot 
{
public:
	ngJsonRoot();
	virtual ~ngJsonRoot();
public:
	JSON_TYPE GetType() { return m_pNode->GetType(); }
	void SetType(JSON_TYPE type);
	void SetNode(ngJsonNode* pNode);
	ngJsonNode* GetNode() { return m_pNode; }
	
	int32 Parse(NGCSTR json);
	ngStringV2* ToString();
	
protected:
	ngJsonNode* m_pNode;
	ngStringV2 m_stringValue;
};


#endif //__NGJSONROOT_H__