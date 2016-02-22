/*
 *  asWXHelperMetroImpl.cpp
 *  jianghu
 *
 *  Created by Chen Zhen on 13-12-09.
 *  Copyright (c) 2013年 Anansi Mobile. All rights reserved.
 */

#include "./asWXHelperMetroImpl.h"
#include "../asWXHelper.h"
#include "UtilsMetro.h"

namespace PhoneDirect3DXamlAppComponent {

    namespace Extra {

        /* impl for asWXHelperMetroImplRef */ 

        void asWXHelperMetroImplRef::RegisterApp(Platform::String^ appID) {
            cb_RegisterApp(appID);
        }

        void asWXHelperMetroImplRef::ShareText(Platform::String^ title, Platform::String^ content, int type) {
            cb_ShareText(title, content, type);
        }

        void asWXHelperMetroImplRef::ShareUrl(Platform::String^ title, Platform::String^ desc, Platform::String^ url, const Platform::Array<unsigned char>^ thumbData, int type) {
            cb_ShareUrl(title, desc, url, thumbData, type);
        }

        void asWXHelperMetroImplRef::ShareImage(
            const Platform::Array<unsigned char>^ data,
            const Platform::Array<unsigned char>^ thumbData,
            Platform::String^ title,
            Platform::String^ desc,
            int type) {
            cb_ShareImage(data, thumbData, title, desc, type);
        }

        void asWXHelperMetroImplRef::ShareImage(
            Platform::String^ imgUrl,
            const Platform::Array<unsigned char>^ thumbData,
            Platform::String^ title,
            Platform::String^ desc,
            int type) {
                cb_ShareImageUrl(imgUrl, thumbData, title, desc, type);
        }

        void asWXHelperMetroImplRef::OnSendToWXResp(int errCode, Platform::String^ errStr) {
            if (m_pEventListener == NULL) {
                return;
            }

            switch (errCode) {
                case 0:
                    //m_pEventListener->OnWXShareDone();
                    break;
                case -1:
                   // m_pEventListener->OnWXShareCanceled();
                    break;
                default:
                    //m_pEventListener->OnWXShareFailed(errCode, CCUnicodeToUtf8(errStr->Data()).c_str());
                    break;
            }
        }

        void asWXHelperMetroImplRef::SetEventListener(asWXHelperEventListener* l) {
            m_pEventListener = l;
        }

        asWXHelperMetroImplRef::asWXHelperMetroImplRef()
            : m_pEventListener(NULL) {

        }

        asWXHelperMetroImplRef^ asWXHelperMetroImplRef::GetInstance() {
            if (m_pThis == nullptr)
            {
                m_pThis = ref new asWXHelperMetroImplRef();
            }

            return m_pThis;
        }

        asWXHelperMetroImplRef^ asWXHelperMetroImplRef::m_pThis = nullptr;

    }

}
