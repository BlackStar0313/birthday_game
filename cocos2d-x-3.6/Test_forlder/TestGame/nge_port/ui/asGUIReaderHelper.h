/*
 *  asGUIReaderHelper.h
 *  pnk
 *
 *  Created by Chen Zhen on 14-5-4.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#ifndef __asGUIReaderHelper_h__
#define __asGUIReaderHelper_h__

#include <cocos2d.h>
#include <core/NGE_Types.h>
#include <core/NGE_Macros.h>
#include <ui/asCCView.h>
#include <nsl/ngNOHashMap.h>

NS_ASGUI_BEGIN

class asGUIReaderHelper {
public:
    static cocos2d::gui::Widget* LoadJsonUIFile(NGCSTR filename, asCCView* view);

private:
    static ngNOHashMap m_jsonDataCache;
};

NS_ASGUI_END

#endif //__asGUIReaderHelper_h__
