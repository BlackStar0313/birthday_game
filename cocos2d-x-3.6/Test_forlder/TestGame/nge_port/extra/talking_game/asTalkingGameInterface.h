//
//  asTalkingDataInterface.h (CTalkingGame.h)
//  extra
//
//  Created by Chen QI on 13-7-19.
//
//  Modified by Chen Zhen on 13-08-07
//      1. change name: CTalkingGame.h -> asTalkingDataInterface.h
//      2. update interface (compatible with old interface).
//
//  Modified by Rui Xie on 14-09-10
//      1. change name: asTalkingDataInterface.h -> asTalkingGameInterface.h
//
//  Modified by Rui Xie on 15-03-13
//      1. remove old interface
//  Copyright (c) 2013年 Anansi. All rights reserved.
//

#ifndef __ASTALKINGGAMEINTERFACE_H__
#define __ASTALKINGGAMEINTERFACE_H__

#include <core/NGE_Types.h>

enum ETALKINGAME_PLAYER_TYPE {
    TALKINGGAME_ANONYMOUS = 0,
    TALKINGGAME_REGISTERED,
    TALKINGGAME_WEIBO,
    TALKINGGAME_QQ,
    TALKINGGAME_TWEIBO, //TencentWeibo
    TALKINGGAME_ND91,
    TALKINGGAME_TYPE1,   //type1
    TALKINGGAME_TYPE2,
    TALKINGGAME_TYPE3,
    TALKINGGAME_TYPE4,
    TALKINGGAME_TYPE5,
    TALKINGGAME_TYPE6,
    TALKINGGAME_TYPE7,
    TALKINGGAME_TYPE8,
    TALKINGGAME_TYPE9,
    TALKINGGAME_FACEBOOK, // type10
};

class asTalkingGameInterface {
public:
    /* 
     
     no need to called in Android (java interface will be called).
     
     */
    static void Init(NGCSTR appId, NGCSTR channelId);
    
    static void SetAccountInfo(int64 playerId, NGCSTR playerName, int32 type, int32 level, int32 age, int32 male, int32 serverIdx);
    
    static void BeginPurchase(NGCSTR orderId, NGCSTR iapId, double currencyAmount,
                                   NGCSTR currencyType, double gameGoldAmount, NGCSTR paymentType);
    static void EndPurchase(NGCSTR orderId);
    
    static void UseBucks(NGCSTR item, int32 count, double price);
    
    static void MissionStart(int32 missionId);
    static void MissionEnd(int32 missionId);
};


#endif /* defined(__ASTALKINGDATAINTERFACE_H__) */
