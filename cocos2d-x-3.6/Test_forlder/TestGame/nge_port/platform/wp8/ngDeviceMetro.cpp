/*
 *  ngDeviceMetro.cpp
 *  NextGenEngine
 *
 *  Created by ppcfan on 09-3-17.
 *	Modified by lg2000 on 09-04-13.
 *  Copyright 2009 DreaminGame. All rights reserved.
 *
 */
#include "ngDeviceMetro.h"
#include <NGE_Core.h>
#include "UtilsMetro.h"

#include <io/ngFileWriter.h>
#include <io/ngFileReader.h>

#include <windows.h>
#include <collection.h>
#include "GameWrapper.h"

namespace NextGenEngineMetro {

    public ref class LocalNotificationInfo sealed{
	public:
		Platform::String ^ Content() { return content; }
		void Content(Platform::String ^ val) { content = val; }
		int Channel() { return channel; }
		void Channel(int val) { channel = val; }
		int DeltaSecond() { return deltaSecond; }
		void DeltaSecond(int val) { deltaSecond = val; }

	private:
		Platform::String ^ content;
		int channel;
		int deltaSecond;
		
	};

}

using namespace NextGenEngineMetro;

using namespace Windows::Phone::System::Memory;
using namespace Windows::Storage;

static const char* GetResourceRoot() {
    static char sResourceRoot[MAX_PATH] = {'\0'};
    if (sResourceRoot[0] == 0) {

        Windows::ApplicationModel::Package^ package = Windows::ApplicationModel::Package::Current;
		Windows::Storage::StorageFolder^ installedLocation = package->InstalledLocation;
		Platform::String^ resPath = installedLocation->Path + "\\" ;//+ "\\Assets\\";
		std::string pathStr = CCUnicodeToUtf8(resPath->Data());
        strcpy(sResourceRoot, pathStr.c_str());
    }

    return sResourceRoot;
}

ngDevice* ngDevice::CreateInstance() {
    ngDevice::m_pThis = DNEW(ngDeviceMetro);
    return ngDevice::m_pThis;
}
 
void ngDevice::ReleaseInstance(void *pObj)
{
    ngDeviceMetro* pThis = (ngDeviceMetro*)pObj;
    DELETE_OBJECT(pThis, ngDeviceMetro);
}

ngDeviceMetro::ngDeviceMetro()
{
	ngStringV2 strChannel = DoGetChannelID();
	m_strChannelID = strChannel;
}

ngDeviceMetro::~ngDeviceMetro() {
}

void ngDeviceMetro::Initialize() {
	SetDeviceInfo();	
}

void ngDeviceMetro::GetFileWritePath(ngStringV2& out, const char* file) {
	StorageFolder^ folder = ApplicationData::Current->LocalFolder;
	Platform::String^ resPath = folder->Path + "\\" ;
	std::string pathStr = CCUnicodeToUtf8(resPath->Data());
	out.Append(pathStr.c_str());
	out += file;
}


const char* ngDeviceMetro::GetDeviceFamily() {
    return m_deviceFamily.GetCString();
}

const char* ngDeviceMetro::GetDeviceType() {
    return m_deviceType.GetCString();
}

const char* ngDeviceMetro::GetDeviceVersion() {
    return m_deviceVersion.GetCString();
}

const char* ngDeviceMetro::GetDeviceID() {
    return m_deviceId.GetCString();
}

const char* ngDeviceMetro::GetDeviceMacID() {
	return m_deviceMacId.GetCString();
}

NGCSTR ngDeviceMetro::CurrentLocaleLanguage()
{
	// s_szLang = en
	static char s_szLang[10];

	NGCSTR rt = NULL;
	ULONG numLanguages = 0;
	DWORD cchLanguagesBuffer = 0;
	BOOL hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, NULL, &cchLanguagesBuffer);

	if (hr) {
		WCHAR* pwszLanguagesBuffer = new WCHAR[cchLanguagesBuffer];
		hr = GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, pwszLanguagesBuffer, &cchLanguagesBuffer);
		if (hr) {
			if (wcsncmp(pwszLanguagesBuffer, L"en-", 3) == 0) { // any English: en-*
				 rt = "en";
			} else if (wcsncmp(pwszLanguagesBuffer, L"zh-",3) == 0) { // any Chinese 
				rt = "cn";
			}
			else
			{
				s_szLang[0] = pwszLanguagesBuffer[0];
				s_szLang[1] = pwszLanguagesBuffer[1];
				s_szLang[2] = 0;
				rt = s_szLang;
			}
		}
		delete pwszLanguagesBuffer;
	}
	return rt;
}

void ngDeviceMetro::LaunchURL( const char* url )
{
	std::wstring wUrl = CCUtf8ToUnicode(url);
	Windows::System::Launcher::LaunchUriAsync(ref new Windows::Foundation::Uri(ref new Platform::String(wUrl.c_str())));
}

int64 ngDeviceMetro::GetFreeMemory()
{
	return MemoryManager::ProcessCommittedLimit - MemoryManager::ProcessCommittedBytes;
}

//TODO: CHECK_LATER
/* 这里判断文件是否存在和获取文件完整路径的接口有一些歧义，稍后需要再确认。[zhen.chen] */
boolean ngDeviceMetro::IsFileExist(const char* file) {
	ngStringV2 out;
	return GetFilePath(out,file);
}

boolean ngDeviceMetro::GetFilePath(ngStringV2& out, const char* file) {

	out.Clear();
	
    /* 1. 检查写目录 */
	GetFileWritePath(out, file);
    if (IsFileExistWithFullPath(out)) {
        return TRUE;
    }

    /* 2. 检查只读的资源目录(res\) */
	ngStringV2 resPath;
    resPath.Format("%s%s%s", GetResourceRoot(), "res\\", file);
	if (IsFileExistWithFullPath(resPath)) {
		out = resPath;
		return TRUE;
	}

    /*3. 检查根目录 */
	ngStringV2 rootPath;
    rootPath.Format("%s%s", GetResourceRoot(), file);
	if (IsFileExistWithFullPath(rootPath)) {
		out = rootPath;
		return TRUE;
	}

    out.Clear();
	out = file;
	return FALSE;
}

void ngDeviceMetro::GetFileCachePath( ngStringV2& out, const char* file )
{
	StorageFolder^ folder = ApplicationData::Current->LocalFolder;
	Platform::String^ resPath = folder->Path + "\\" ;
	std::string pathStr = CCUnicodeToUtf8(resPath->Data());
	out.Append(pathStr.c_str());
	out += file;

}

double ngDeviceMetro::GetCurrentSystemTime()
{
	return 0;
}

NGCSTR ngDeviceMetro::CurrentLocaleRegion()
{
	return "cn";
}

float ngDeviceMetro::GetDeviceVersionf()
{
	return 8.0f;
}

const char* ngDeviceMetro::GetDevicePlatform()
{
	return "Windows Phone";
}

ngStringV2 ngDeviceMetro::DoGetChannelID()
{
	ngStringV2 strChannelID;

    Platform::Array<Platform::Object^>^ args = ref new Platform::Array<Platform::Object^>(2);
    args[0] = ref new Platform::String(L"testparam1");
    args[1] = ref new Platform::String(L"testparam1");

    Platform::String ^refStrReturn = CS_Call_For_Result("GetChannelID", args, Platform::String^);

	strChannelID = CCUnicodeToUtf8(refStrReturn->Data()).c_str();

	return strChannelID;
}

NGCSTR ngDeviceMetro::GetChannelID() const
{
	return m_strChannelID.GetCString();
}

int32 ngDeviceMetro::GetRotationAngle()
{
	return 0;
}

int32 ngDeviceMetro::GetKeyboardHeight()
{
	return 550;
}

void ngDeviceMetro::SetKeyboardHeight( int32 height )
{
	
}

//
//boolean ngDeviceMetro::FindFilePath( ngStringV2& out,const ngStringV2& in )
//{
//	out.Clear();
//	if (IsFileExist(in.GetCString())) {
//		out.Append(in.GetCString());
//		return TRUE;
//	} else  {
//		GetFileWritePath(out,in.GetCString());
//		//const char* fullPath = CCFileUtils::fullPathFromRelativePath(in.GetCString());
//		if (IsFileExist(out)) {
//			//out.Append(fullPath);
//			return TRUE;
//		} else {
//			ngStringV2 resPath("res/");
//			resPath.Append(in.GetCString());
//			if (IsFileExist(resPath)) {
//				out.Clear();
//				out.Append(resPath.GetCString());
//				return TRUE;
//			} else {
//				out.Clear();
//				out.Append(in.GetCString());
//				return FALSE;
//			}
//			
//		}
//		
//	}
//}

void ngDeviceMetro::SetDeviceInfo() {
    Platform::String^ strDeviceID = CS_Call_For_Result("wrapper_OnGetDeviceId", nullptr, Platform::String^);
	m_deviceId.Clear();
	m_deviceId.Append(strDeviceID->Data());

	m_deviceMacId.Clear();
	m_deviceMacId.Append(strDeviceID->Data());

    Platform::String^ strDeviceFamily = CS_Call_For_Result("wrapper_OnGetDeviceName", nullptr, Platform::String^);
	m_deviceFamily.Clear();
	m_deviceFamily.Append(strDeviceFamily->Data());

    Platform::String^ strDeviceType = CS_Call_For_Result("wrapper_OnGetDeviceType", nullptr, Platform::String^);
	m_deviceType.Clear();
	m_deviceType.Append(strDeviceType->Data());

    Platform::String^ strDeviceVersion = CS_Call_For_Result("wrapper_OnGetDeviceVersion", nullptr, Platform::String^);
	m_deviceVersion.Clear();
    m_deviceVersion.Append(strDeviceVersion->Data());
}

void ngDeviceMetro::RegisterPushNotification()
{
	// TODO: implement cancel local notification
	//throw std::exception("The method or operation is not implemented.");
}

void ngDeviceMetro::PushLocalNotification( int64 fireTimeFromNow, ngStringV2* msg, int32 channel /*= 0*/, NGCSTR param /*= NULL */ ) {
	LocalNotificationInfo^ info = ref new LocalNotificationInfo();
	info->Channel(channel);
	info->DeltaSecond(fireTimeFromNow);
	info->Content(ref new Platform::String(CCUtf8ToUnicode(msg->GetCString()).data()));

    Platform::Array<Platform::Object^>^ args = ref new Platform::Array<Platform::Object^>(1);
    args[0] = info;
    CS_Call("wrapper_OnPushLocalNotification", args);


}

void ngDeviceMetro::CancelLocalNotification( int32 channel, NGCSTR param /*= NULL */ ) {
	//throw std::exception("The method or operation is not implemented.");
}

NGCSTR ngDeviceMetro::GetReleaseVersion() const {
	static char s_strVersionNumber[256];

	memset(s_strVersionNumber, 0, sizeof(s_strVersionNumber));
	memmove(s_strVersionNumber, "1.0.0.0", 7);
	

	const wchar_t *pwcRet = NULL;

    Platform::String ^refStrReturn = CS_Call_For_Result("GetVersionNumber", nullptr, Platform::String^);

	pwcRet = refStrReturn->Data();
	if(pwcRet != NULL && pwcRet[0] != 0)
	{
		ngStringV2 strTemp = pwcRet;
		
		const char *pVerBuf = strTemp.GetCString(TRUE);
		if(pVerBuf != NULL)
		{
			int iCopyLen = strTemp.GetLength();
			if(iCopyLen >= sizeof(s_strVersionNumber))
			{
				iCopyLen = sizeof(s_strVersionNumber) - 1;
			}

			if( iCopyLen > 0 && iCopyLen < sizeof(s_strVersionNumber) )
			{
				memmove(s_strVersionNumber, pVerBuf, iCopyLen);
				s_strVersionNumber[iCopyLen] = 0;
			}
		}
		return s_strVersionNumber;
	}

	return s_strVersionNumber;
}

const char* ngDeviceMetro::GetDeviceVendorID() {
	return m_deviceId.GetCString();
}

boolean ngDeviceMetro::IsFileExistWithFullPath(const char* full_path) {
    //bool ret = false;
    //FILE * pf = 0;
    //if (resPath && strlen(resPath) && (pf = fopen(resPath, "rb")))
    //{
    //    ret = true;
    //    fclose(pf);
    //}
    //return ret;

	std::wstring path = CCUtf8ToUnicode(full_path);

    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {0};
	extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
	extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
	extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
	extendedParams.lpSecurityAttributes = nullptr;
	extendedParams.hTemplateFile = nullptr;

    HANDLE hFile = CreateFile2(path.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams);
	if (INVALID_HANDLE_VALUE == hFile) {
		return FALSE;
	}

    CloseHandle(hFile);
	return TRUE;

}

