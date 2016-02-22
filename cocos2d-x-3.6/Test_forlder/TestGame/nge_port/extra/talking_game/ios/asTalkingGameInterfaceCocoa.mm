//
//  asTalkingDataInterfaceCocoa.mm (CTalkingGame.mm)
//  extra
//
//  Created by Chen Qi on 11-11-14.
//
//  Modified by Chen Zhen on 13-08-07
//      1. change name: CTalkingGame.mm -> asTalkingDataInterfaceCocoa.mm
//      2. update interface (compatible with old interface).
//
//  Copyright (c) 2011年 tendcloud. All rights reserved.
//

#include "asTalkingGameInterface.h"
#include <nsl/ngStringV2.h>
#include "TalkingDataGA.h"

#define ENABLE_TALKING_GAME    1

void CTalkingGameInit(NGCSTR appId, NGCSTR channelId)
{
    NSString *nsAppId = [[NSString alloc] initWithUTF8String:appId];
    NSString *nsChannelId = [[NSString alloc] initWithUTF8String:channelId];
    
#if ENABLE_TALKING_GAME
    [TalkingDataGA onStart:nsAppId withChannelId:nsChannelId];
#endif
    
    [nsAppId release];
    [nsChannelId release];
}

void CTalkingGameSetAccountInfo(int64 playerId, NGCSTR playerName, int32 type, int32 level, int32 age, int32 male, int32 serverIdx)
{
    ngStringV2 strPlayerId((int64)playerId);
    NSString *nsPlayerId = [[NSString alloc] initWithUTF8String:strPlayerId.GetCString()];
    NSString *nsPlayerName = [[NSString alloc] initWithUTF8String:playerName];
    
    ngStringV2 strServerIdx((int64)serverIdx);
    NSString *nsServerIdx = [[NSString alloc] initWithUTF8String:strServerIdx.GetCString()];
    
#if ENABLE_TALKING_GAME
    TDGAAccount *account = [TDGAAccount setAccount:nsPlayerId];
    [account setAccountName:nsPlayerName];

    switch (type) {
        case TALKINGGAME_ANONYMOUS: // CHttpClient::PLAYER_ANONYMOUS:
            [account setAccountType:kAccountAnonymous];
            break;
            
        case TALKINGGAME_REGISTERED: // CHttpClient::PLAYER_USERNAME:
            [account setAccountType:kAccountRegistered];
            break;
            
        case TALKINGGAME_FACEBOOK:
            [account setAccountType:kAccountType10];
            break;
            
        default:
            [account setAccountType:kAccountAnonymous];
            break;
    }

    [account setLevel:level];
    [account setAge:age];
    [account setGender:(male == 1? kGenderMale : kGenderFemale)];
    [account setGameServer:nsServerIdx];
#endif
    
    [nsPlayerId release];
    [nsPlayerName release];
    [nsServerIdx release];
}

void CTalkingGameBeginPurchase(NGCSTR orderId, NGCSTR iapId, double currencyAmount,
                                NGCSTR currencyType, double gameGoldAmount, NGCSTR paymentType)
{
    NSString *nsOrderId = [[NSString alloc] initWithUTF8String:orderId];
    NSString *nsIapId = [[NSString alloc] initWithUTF8String:iapId];
    NSString *nsCurrencyType = [[NSString alloc] initWithUTF8String:currencyType];
    NSString *nsPaymentType = [[NSString alloc] initWithUTF8String:paymentType];
    
#if ENABLE_TALKING_GAME
    [TDGAVirtualCurrency onChargeRequst:nsOrderId
                                  iapId:nsIapId
                         currencyAmount:currencyAmount
                           currencyType:nsCurrencyType
                  virtualCurrencyAmount:gameGoldAmount
                            paymentType:nsPaymentType];
#endif
    
    [nsOrderId release];
    [nsIapId release];
    [nsCurrencyType release];
    [nsPaymentType release];
}

void CTalkingGameEndPurchase(NGCSTR orderId)
{
    NSString *nsOrderId = [[NSString alloc] initWithUTF8String:orderId];
    
#if ENABLE_TALKING_GAME
    [TDGAVirtualCurrency onChargeSuccess:nsOrderId];
#endif
    
    [nsOrderId release];
}

void CTalkingGameUseBucks(NGCSTR item, int32 count, double price)
{
    NSString *nsItem = [[NSString alloc] initWithUTF8String:item];
    
#if ENABLE_TALKING_GAME
    [TDGAItem onPurchase:nsItem itemNumber:count priceInVirtualCurrency:price];
#endif
    
    [nsItem release];
}

void CTalkingGameMissionStart(int32 missionId)
{
    NSString *strMission = [[NSString alloc] initWithFormat:@"%d",missionId];
    
#if ENABLE_TALKING_GAME
    [TDGAMission onBegin:strMission];
#endif
}

void CTalkingGameMissionEnd(int32 missionId)
{
    NSString *strMission = [[NSString alloc] initWithFormat:@"%d",missionId];
    
#if ENABLE_TALKING_GAME
    [TDGAMission onCompleted:strMission];
#endif
}