/*
 *  ngStoreKitMetro.cpp
 *  template
 *
 *  Created by Yuhui Wang on 10/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ngStoreKitMetro.h"
#include "NGE_Core.h"
#include "NextGenEngineMetroLib.h"
#include <collection.h>
#include "UtilsMetro.h"

#ifdef MANAGED_ENGINE
#include "GameWrapper.h"
using namespace NextGenEngineMetro;
#endif

using namespace Platform::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace concurrency;

static ngStoreKitMetro* sInstance = NULL;

ngStoreKit* ngStoreKit::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = DNEW(ngStoreKitMetro);
    }
    return sInstance;
}

void ngStoreKit::ReleaseInstance(void *pObj)
{
      DELETE_OBJECT((ngStoreKitMetro*)pObj, ngStoreKitMetro);
}

ngStoreKitMetro::ngStoreKitMetro()
{

#ifdef MANAGED_ENGINE
	m_listToken =
		GameWrapper::GetInstance()->CallBackListPurchase += ref new Windows::Foundation::EventHandler<ListingInformation^>
		([this](Platform::Object^ sender , ListingInformation^ data){
			m_productInfoListing = data;
			int count = m_productInfoListing->ProductListings->Size;

			NG_DEBUG_LOG("--->Get virtual product from ms count[%d] ", count);
			int iIndex = 1;
			for (IIterator<IKeyValuePair<Platform::String^,ProductListing^> ^> ^ iter = m_productInfoListing->ProductListings->First(); iter->HasCurrent;) 
			{
				IKeyValuePair<Platform::String^,ProductListing^>^M = (IKeyValuePair<Platform::String^,ProductListing^>^) iter->Current;

				Platform::String ^ key = M->Key;
				ProductListing ^ value = M->Value;
				Platform::String ^ pid = value->ProductId;
				Platform::String ^ price = value->FormattedPrice;
				Platform::String ^ uri = value->ImageUri->AbsoluteUri;
				Platform::String ^ name = value->Name;
				Platform::String^ tag =  value->Tag;
				ngStringV2 strPrice;
				strPrice.Append(CCUnicodeToUtf8(price->Data()).c_str());

#ifdef USE_STOREKIT_ITEM
                ngStringV2 strPid;
                strPid.Append(CCUnicodeToUtf8(pid->Data()).c_str());

                OnProduct(strPrice.GetCString(), strPid.GetCString());
#else
				OnProduct(strPrice.GetCString());
#endif
				iter->MoveNext();

				ngStringV2 strProductID, strName;
				strProductID = CCUnicodeToUtf8(pid->Data()).c_str();
				strName = CCUnicodeToUtf8(name->Data()).c_str();

				NG_DEBUG_LOG("--->Virtual [%d] ProductID[%s] Name[%s] Price[%s]", 
							iIndex, strProductID.GetCString(), strName.GetCString(), strPrice.GetCString());
				iIndex++;
			}
			OnProductListFinished();
	});

	m_purchaseToken =
		GameWrapper::GetInstance()->CallbackPurchase += ref new Windows::Foundation::EventHandler<Platform::String^>
		([this](Platform::Object^ sender , Platform::String^ data){
			Platform::String^ receipts = data;
			std::string str = CCUnicodeToUtf8(receipts->Data());
			if (!str.empty()){
				auto licenseInformation = CurrentApp::LicenseInformation;
				if (licenseInformation->ProductLicenses->Lookup(m_pid)->IsActive)
				{

					CurrentApp::ReportProductFulfillment(m_pid);
                    OnPaymentFinished(str.c_str(), CCUnicodeToUtf8(m_pid->Data()).c_str());
				}
				else
				{
					OnPaymentFailed();
				}
			} else {
				OnPaymentFailed();
			}
			
	});

	m_purchaseFortumoToken = 
		GameWrapper::GetInstance()->CallBackFortumoPaymentAction += ref new Windows::Foundation::EventHandler<int32>
		([this](Platform::Object^ sender , int32 result){
			if (result == 1){
				// success,param null
				OnPaymentFinished("","");
			} else if (result == 0) {
				// faild
				OnPaymentFailed();
			}			
	});
#endif // MANAGED_ENGINE

}

void ngStoreKitMetro::Purchase(NGCSTR productId)
{
	ngStringV2 str ;
	str.Append(productId);
	Platform::String ^ pid  = ref new Platform::String(str.GetWcs());

	
	
#if defined MANAGED_ENGINE
	m_pid = pid;
	GameWrapper::GetInstance()->Purchase(m_pid);
#else 

	auto licenseInformation = CurrentApp::LicenseInformation;
	if (licenseInformation->ProductLicenses->Lookup(pid)->IsActive)
	{
		CurrentApp::ReportProductFulfillment(pid);
	}

	IAsyncOperation<Platform::String^> ^ purchaseAsync = CurrentApp::RequestProductPurchaseAsync(pid, true);
	purchaseAsync->Completed = ref new AsyncOperationCompletedHandler<Platform::String^>([this,pid]( IAsyncOperation<Platform::String^> ^operation, Windows::Foundation::AsyncStatus status){
		try
		{
			if (status == AsyncStatus::Completed){
				Platform::String^ receipts = operation->GetResults();
				std::string str = CCUnicodeToUtf8(receipts->Data());

				auto licenseInformation = CurrentApp::LicenseInformation;
				if (licenseInformation->ProductLicenses->Lookup(pid)->IsActive)
				{

					CurrentApp::ReportProductFulfillment(pid);
					OnPaymentFinished(str.c_str());
				}
				else
				{
					OnPaymentFailed();
				}
			}else
			{
				operation->Close();
				OnPaymentFailed();
			}
			
		}
		catch(Platform::Exception^ exception)
		{
			//OnPaymentFailed();//rootPage->NotifyUser("Unable to buy Product 1.", NotifyType::ErrorMessage);
		}
	});
#endif
}

void ngStoreKitMetro::List(ngList& productId)
{
	IVector<Platform::String^>^ pids  = ref new Vector<Platform::String^>();
	ngIterator* iterActor = productId.Iterator();
	while (iterActor->HasNext()) {
        
//#ifdef USE_STOREKIT_ITEM
//        ngStoreKitItem* pProduct = (ngStoreKitItem*)iterActor->Next();
//        ngStringV2* pid = pProduct->GetProductId();
//#else 
		ngStringV2* pid = (ngStringV2*)iterActor->Next();
//#endif

		pids->Append(ref new Platform::String(pid->GetWcs()));
	}
	iterActor->Destroy();
	
#ifdef MANAGED_ENGINE

	GameWrapper::GetInstance()->ListPurchase(pids);

#else

	try
	{
		//create_task(CurrentApp::LoadListingInformationByProductIdsAsync(pids)).then([this](task<ListingInformation^> currentTask)
		IAsyncOperation<ListingInformation^> ^ listAsync = CurrentApp::LoadListingInformationByProductIdsAsync(pids);
		listAsync->Completed = ref new AsyncOperationCompletedHandler<ListingInformation^>([this]( IAsyncOperation<ListingInformation^> ^operation, Windows::Foundation::AsyncStatus status)
		{
			try
			{
				if (status == AsyncStatus::Completed) {
					m_productInfoListing = operation->GetResults();
					int count = m_productInfoListing->ProductListings->Size;
					for (IIterator<IKeyValuePair<Platform::String^,ProductListing^> ^> ^ iter = m_productInfoListing->ProductListings->First(); iter->HasCurrent;) 
					{
						IKeyValuePair<Platform::String^,ProductListing^>^M = (IKeyValuePair<Platform::String^,ProductListing^>^) iter->Current;

						Platform::String ^ key = M->Key;
						ProductListing ^ value = M->Value;
						Platform::String ^ pid = value->ProductId;
						Platform::String ^ price = value->FormattedPrice;
						Platform::String ^ uri = value->ImageUri->AbsoluteUri;
						Platform::String ^ name = value->Name;
						Platform::String^ tag =  value->Tag;
						ngStringV2 strPrice;
						strPrice.Append(price->Data());
						OnProduct(strPrice.GetCString());
						iter->MoveNext();
					}
					OnProductListFinished();
				}

			}
			catch(Platform::Exception^ exception)
			{
				// make a notification?
			}

		});
	}
	catch(Platform::Exception^ exception)
	{

	}

#endif // MANAGED_ENGINE



}



#ifdef MANAGED_ENGINE

void ngStoreKitMetro::SetEventListener()
{
	//m_textBoxLostFocusToken =
	//	GameWrapper::GetInstance()->CallbackTextBoxLostFocus += ref new Windows::Foundation::EventHandler<Platform::Object ^>
	//	([this](Platform::Object^ sender , Platform::Object^ data){
	//		ResignFirstResponder();
	//});

	//m_textBoxTextChangedToken =
	//	GameWrapper::GetInstance()->CallbackTextBoxTextChanged += ref new Windows::Foundation::EventHandler<Platform::String ^>
	//	([this](Platform::Object^ sender , Platform::String ^ newText){
	//		std::string str = CCUnicodeToUtf8(newText->Data());
	//		m_textBuffer.Clear();
	//		m_textBuffer.Append(str.c_str());
	//		m_isTextChanged = TRUE;
	//});
}

void ngStoreKitMetro::RemoveEventListener()
{
	//GameWrapper::GetInstance()->CallbackTextBoxLostFocus -= m_textBoxLostFocusToken;
	//GameWrapper::GetInstance()->CallbackTextBoxTextChanged -= m_textBoxTextChangedToken;
}
#endif

void ngStoreKitMetro::PurchaseFortumo(NGCSTR extraInfo)
{
	ngStringV2 str ;
	str.Append(extraInfo);
	Platform::String ^ pid  = ref new Platform::String(str.GetWcs());

#if defined MANAGED_ENGINE
	m_pid = pid;
	GameWrapper::GetInstance()->FortumoPaymentAction(m_pid);
#else 
	// do something
#endif
}