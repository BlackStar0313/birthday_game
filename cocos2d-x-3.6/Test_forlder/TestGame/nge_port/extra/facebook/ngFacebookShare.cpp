//
//  ngFacebookShare.cpp
//  city
//
//  Created by liangliang wang on 13-7-3.
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#include "ngFacebookShare.h"
#include <cocos2d.h>
USING_NS_CC;

ngFacebookShare::ngFacebookShare()
: m_pFBShareListener(NULL)
, m_pFBLoginListener(NULL)
{

}

ngFacebookShare::~ngFacebookShare()
{
    
}

void ngFacebookShare::SetFBShareData(FBShareData_t &data)
{
    m_FBShareData.actionType = data.actionType;
    m_FBShareData.object = data.object;
    m_FBShareData.imageHead = data.imageHead;
    m_FBShareData.url = data.url;
    m_FBShareData.nameSpace = data.nameSpace;
    m_FBShareData.appUrl = data.appUrl;
    m_FBShareData.customObject = data.customObject;
}
