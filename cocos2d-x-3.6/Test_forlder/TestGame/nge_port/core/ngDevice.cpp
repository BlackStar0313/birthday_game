/*
 *  ngDevice.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-7.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#include <core/ngDevice.h>

ngDevice* ngDevice::m_pThis = NULL;

ngDevice::ngDevice()
: ngSingleton(ReleaseInstance, this) {
	m_width = 0;
	m_height = 0;
    
    m_isRetina = FALSE;

    memset(m_deviceID, '\0', MAX_DEVICE_ID_LEN);

    /* init channel id. */
    memset(m_channelID, '\0', MAX_CHANNEL_ID_LEN);
    
    memset(m_releaseVersion, '\0', MAX_CHANNEL_ID_LEN);
}

ngDevice::~ngDevice() {
	
}

void ngDevice::SetDeviceDimesion(int32 width, int32 height) {
	m_width = width;
	m_height = height;
}

void ngDevice::SetIsRetina(bool isRetina)
{
    m_isRetina = isRetina;
}

bool ngDevice::GetIsRetina()
{
    return m_isRetina;
}
