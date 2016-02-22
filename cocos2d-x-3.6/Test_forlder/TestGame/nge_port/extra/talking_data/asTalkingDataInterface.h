//
//  asTalkingDataInterface.h (CTalkingGame.h)
//  extra
//
//  Created by Rui Xie 2014-09-10.
//  Copyright (c) 2014年 Anansi. All rights reserved.
//

#ifndef __ASTALKINGDATAINTERFACE_H__
#define __ASTALKINGDATAINTERFACE_H__

#include <core/NGE_Types.h>

#define CTalkingDataInit            asTalkingDataInterface::Init
#define CTalkingDataEvent           asTalkingDataInterface::Event

class asTalkingDataInterface {
public:
    /* 
     
     no need to called in Android (java interface will be called).
     
     */
    static void Init(NGCSTR appId, NGCSTR channelId);
    static void Event(NGCSTR eventId);
};


#endif /* defined(__ASTALKINGDATAINTERFACE_H__) */
