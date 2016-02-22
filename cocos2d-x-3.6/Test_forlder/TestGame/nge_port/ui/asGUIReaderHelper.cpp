/*
 *  asGUIReaderHelper.cpp
 *  pnk
 *
 *  Created by Chen Zhen on 14-5-4.
 *  Copyright (c) 2014年 Anansi Mobile. All rights reserved.
 */

#include "asGUIReaderHelper.h"

USING_NS_ASGUI;

cocos2d::gui::Widget* asGUIReaderHelper::LoadJsonUIFile(NGCSTR filename, asCCView* view) {

    if (filename == NULL || strlen(filename) <= 0) {
        NGASSERT(0 && "[GUI Reader Helper] bad json file name!~");
        return NULL;
    }

    rapidjson::Document* jsonDict = (rapidjson::Document*) m_jsonDataCache.Get(filename);

    if (jsonDict == NULL) {
        jsonDict = cocos2d::extension::GUIReader::shareReader()->parseJsonFile(filename);
        if (jsonDict->HasParseError()) {
            NG_DEBUG_LOG("[GUI Reader Helper] GetParseError %s\n",jsonDict->GetParseError());
            SAFE_DELETE(jsonDict);
            NGASSERT(0);
            return NULL;
        } else {
            m_jsonDataCache.Put(filename, jsonDict);
        }
    }

    return cocos2d::extension::GUIReader::shareReader()->widgetFromJsonDict(jsonDict, filename, view);

}

ngNOHashMap asGUIReaderHelper::m_jsonDataCache;
