/*
 *  asWXHelperMetroImpl.h
 *  jianghu
 *
 *  Created by Chen Zhen on 13-12-09.
 *  Copyright (c) 2013年 Anansi Mobile. All rights reserved.
 */

#pragma once

#ifndef __asWXHelperMetroImpl_h__
#define __asWXHelperMetroImpl_h__

using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Devices::Geolocation;
//using namespace Windows::UI::Core;
//using namespace Windows::Phone::UI::Input;

class asWXHelperEventListener;

namespace PhoneDirect3DXamlAppComponent {

    namespace Extra {

        public delegate void delegate_RegisterApp(Platform::String^);
        public delegate Platform::Boolean delegate_IsWXAppInstalled();
        public delegate Platform::Boolean delegate_IsWXAppSupportAPI();

        public delegate void delegate_ShareText(Platform::String^, Platform::String^, int);
        public delegate void delegate_ShareUrl(
            Platform::String^,
            Platform::String^,
            Platform::String^,
            const Platform::Array<unsigned char>^,
            int);
        public delegate void delegate_ShareImage(
            const Platform::Array<unsigned char>^,
            const Platform::Array<unsigned char>^,
            Platform::String^,
            Platform::String^,
            int);

        public delegate void delegate_ShareImageUrl(
            Platform::String^,
            const Platform::Array<unsigned char>^,
            Platform::String^,
            Platform::String^,
            int);        

        public ref class asWXHelperMetroImplRef sealed {

        public:
            static asWXHelperMetroImplRef^ GetInstance();
        private:
            asWXHelperMetroImplRef();
        private:
            static asWXHelperMetroImplRef^ m_pThis;

        public:
            /* callback here */
            event delegate_RegisterApp^ cb_RegisterApp;
            event delegate_IsWXAppInstalled^ cb_IsWXAppInstalled;
            event delegate_IsWXAppSupportAPI^ cb_IsWXAppSupportAPI;

            event delegate_ShareText^ cb_ShareText;
            event delegate_ShareUrl^ cb_ShareUrl;
            event delegate_ShareImage^ cb_ShareImage;
            event delegate_ShareImageUrl^ cb_ShareImageUrl;

        public:

            void RegisterApp(Platform::String^ appID);

            void ShareText(Platform::String^ title, Platform::String^ content, int type);
            void ShareUrl(Platform::String^ title, Platform::String^ desc, Platform::String^ url, const Platform::Array<unsigned char>^ thumbData, int type);
            void ShareImage(
                const Platform::Array<unsigned char>^ data,
                const Platform::Array<unsigned char>^ thumbData,
                Platform::String^ title,
                Platform::String^ desc,
                int type);

			
            void ShareImage(
                Platform::String^ imgUrl,
                const Platform::Array<unsigned char>^ thumbData,
                Platform::String^ title,
                Platform::String^ desc,
                int type);
				

            void OnSendToWXResp(int errCode, Platform::String^ errStr);

        private:
            void SetEventListener(asWXHelperEventListener* l);

        private:
            asWXHelperEventListener* m_pEventListener; //asWXHelperEventListener;

            friend class asWXHelper;
        };

    }
}

#endif //__asWXHelperMetroImpl_h__
