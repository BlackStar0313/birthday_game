/*
 *  NGEDefs.h
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-10.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */

#ifndef __NGEDEFS_H__
#define __NGEDEFS_H__

#include "./NGE_Types.h"

#define kFilteringFactor 0.1


// NGE Error Code
#define NG_OK							 0
#define NG_ERROR_FAILTURE                -1
#define NG_ERROR_NOMEM                  1    /* not enough memory */
#define NG_ERROR_IO                     2    /* I/O error */
#define NG_ERROR_NETWORK                3    /* network error */
#define NG_ERROR_WOULDBLOCK             4    /* operation would block */
#define NG_ERROR_INVALID_ARGUMENT       5    /* invalid argument */
#define NG_ERROR_INVALID_SOCKET         6    /* invalid socket */
#define NG_ERROR_NO_ENOUGH_RESOURCE	 7    /* not enough timer */
#define NGE_ERROR_TIMEOUT_NETWORK		 8	  /* time out */
#define NG_TOBECONTINUE						101
#define NG_EXPIRED_DATA						102
#define NG_ERROR_FINISH_HANDLE             103

#define	EVENT_ID_APPLET_START	0
#define EVENT_ID_APPLET_STOP	1
#define EVENT_ID_APPLET_SUSPEND	2
#define EVENT_ID_APPLET_RESUME	3

#define EVENT_ID_AUDIO_SESSION_END_INTERRUPTION 4
#define EVENT_ID_VOLUME_UP		5
#define EVENT_ID_VOLUME_DOWN	6

#define EVENT_ID_ALERT_CLIKED	7

#define EVENT_ID_TEXTFIELD_RETURNED     8
#define EVENT_ID_TEXTFIELD_BEGIN_EDIT   9
#define EVENT_ID_TEXTFIELD_END_EDIT     10
#define EVENT_ID_TEXTFIELD_CHANGED      11
#define EVENT_ID_TEXTFIELD_MAX_CHAR     12
#define EVENT_ID_TEXTFIELD_INVALID_CHAR 13

#define EVENT_ID_VIEW_CUSTOM		100
#define EVENT_ID_BUTTON_CUSTOM		200
#define EVENT_ID_SCROLLVIEW_CUSTOM	300
#define EVENT_ID_TABLEVIEW_CUSTOM	400
#define EVENT_ID_COMBOBOX_CUSTOM	500
#define EVENT_ID_TABVIEW_CUSTOM		600
#define EVENT_ID_SWITCH_CUSTOM      700
#define EVENT_ID_VIEW_TOUCH_CUSTOM  800
#define EVENT_ID_SPINBOX_CUSTOM     900
#define EVENT_ID_PICKERVIEW_CUSTOM  1000
#define EVENT_ID_SLIDEBOX_CUSTOM    1100
#define EVENT_ID_CHECKBOX_CUSTOM    1200

/* 自定义事件为避免和引擎事件冲突，建议定义ID大于20000。[zhen.chen] */
#define EVENT_ID_CUSTOM_OFFSET  20000

#define MAX_TOUCHES_NUM			15

#define CHAR_COUNT				223

#define HASH_BASE				5381

const uint32 kMediaChannels = 1;
const uint32 kMediaBitRate = 16;
const uint32 kMediaFrequency = 8000;

#if 0

/* enable this code to debug iterator memory leaks. */

class ngLinkedList;
extern ngLinkedList* iteratorList;

extern boolean __debug_iterator;

#endif

#endif
