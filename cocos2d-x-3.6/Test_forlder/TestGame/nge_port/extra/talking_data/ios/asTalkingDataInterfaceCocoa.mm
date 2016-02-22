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

#include "asTalkingDataInterface.h"
#include <nsl/ngStringV2.h>
#include "TalkingData.h"

#define ENABLE_TALKING_DATA    1

void CTalkingDataInit(NGCSTR appId, NGCSTR channelId)
{
    NSString *nsAppId = [[NSString alloc] initWithUTF8String:appId];
    NSString *nsChannelId = [[NSString alloc] initWithUTF8String:channelId];
    
#if ENABLE_TALKING_DATA
    [TalkingData sessionStarted:nsAppId withChannelId:nsChannelId];
#endif
    
    [nsAppId release];
    [nsChannelId release];
}


void CTalkingDataEvent(NGCSTR eventId)
{
    NSString *nsEventId = [[NSString alloc] initWithUTF8String:eventId];
    
#if ENABLE_TALKING_DATA
    [TalkingData trackEvent:nsEventId];
#endif
    
    [nsEventId release];
}
