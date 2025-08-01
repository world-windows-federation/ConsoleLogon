#include "pch.h"

#include "DefaultSelector.h"

#include <initguid.h>
#include <windows.foundation.collections.h>

#include "windowscollections.h"

using namespace Microsoft::WRL;

DEFINE_GUID(CLSID_PasswordCredentialProvider,        0x60B78E88, 0xEAD8, 0x445C, 0x9C, 0xFD, 0x0B, 0x87, 0xF7, 0x4E, 0xA6, 0xCD);
DEFINE_GUID(CLSID_CWLIDCredentialProvider,           0xF8A0B131, 0x5F68, 0x486C, 0x80, 0x40, 0x7E, 0x8F, 0xC3, 0xC8, 0x5B, 0xB6);
DEFINE_GUID(CLSID_PicturePasswordCredentialProvider, 0x2135F72A, 0x90B5, 0x4ED3, 0xA7, 0xF1, 0x8B, 0xB7, 0x05, 0xAC, 0x27, 0x6A);

HRESULT DefaultSelector::RuntimeClassInitialize(LC::IUserSettingManager* userSettingManager, LC::LogonUIRequestReason reason)
{
	m_userSettingManager = userSettingManager;
	m_reason = reason;

	ComPtr<Windows::Foundation::Collections::Internal::AgileVector<GUID>> preferredProviders;
	RETURN_IF_FAILED(Windows::Foundation::Collections::Internal::AgileVector<GUID>::Make(&preferredProviders)); // 29
	RETURN_IF_FAILED(preferredProviders->Vector::Append(CLSID_PasswordCredentialProvider)); // 30
	RETURN_IF_FAILED(preferredProviders->Vector::Append(CLSID_CWLIDCredentialProvider)); // 31
	RETURN_IF_FAILED(preferredProviders->Vector::GetView(&m_preferredProviders)); // 32

	ComPtr<Windows::Foundation::Collections::Internal::AgileVector<GUID>> excludedProviders;
	RETURN_IF_FAILED(Windows::Foundation::Collections::Internal::AgileVector<GUID>::Make(&excludedProviders)); // 35
	RETURN_IF_FAILED(excludedProviders->Vector::Append(CLSID_PicturePasswordCredentialProvider)); // 36
	RETURN_IF_FAILED(excludedProviders->Vector::GetView(&m_excludedProviders)); // 37

	return S_OK;
}

HRESULT DefaultSelector::get_UseLastLoggedOnProvider(BOOLEAN* value)
{
	*value = TRUE;
	return S_OK;
}

HRESULT DefaultSelector::get_PreferredProviders(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_preferredProviders.CopyTo(ppValue)); // 51
	return S_OK;
}

#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
HRESULT DefaultSelector::get_OtherUserPreferredProviders(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_preferredProviders.CopyTo(ppValue)); // 51
	return S_OK;
}
#endif

HRESULT DefaultSelector::get_ExcludedProviders(WFC::IVectorView<GUID>** ppValue)
{
	RETURN_IF_FAILED(m_excludedProviders.CopyTo(ppValue)); // 58
	return S_OK;
}

HRESULT DefaultSelector::get_DefaultUserSid(HSTRING* value)
{
	RETURN_IF_FAILED(m_userSettingManager->get_UserSid(value)); // 64
	return S_OK;
}

HRESULT SHRegGetBOOLWithREGSAM(HKEY key, LPCWSTR subKey, LPCWSTR value, REGSAM regSam, BOOL* data)
{
	DWORD dwType = REG_NONE;
	DWORD dwData;
	DWORD cbData = sizeof(dwData);
	LSTATUS lRes = RegGetValueW(
		key,
		subKey,
		value,
		((regSam & 0x100) << 8) | RRF_RT_REG_DWORD | RRF_RT_REG_SZ | RRF_NOEXPAND,
		&dwType,
		&dwData,
		&cbData
	);
	if (lRes != ERROR_SUCCESS)
	{
		if (lRes == ERROR_MORE_DATA)
			return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		if (lRes > 0)
			return HRESULT_FROM_WIN32(lRes);
		return lRes;
	}

	if (dwType == REG_DWORD)
	{
		if (dwData > 1)
			return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		*data = dwData == 1;
	}
	else
	{
		if (cbData != 4 || (WCHAR)dwData != L'0' && (WCHAR)dwData != L'1')
			return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		*data = (WCHAR)dwData == L'1';
	}

	return S_OK;
}

HRESULT DefaultSelector::AllowAutoSubmitOnSelection(LCPD::IUser* user, BOOLEAN* value)
{
	*value = TRUE;

	BOOL forceAutoLogon = FALSE;
	bool isForceAutoLogon = SUCCEEDED(SHRegGetBOOLWithREGSAM(
		HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
		L"ForceAutoLogon", 0, &forceAutoLogon)) && forceAutoLogon;

	bool isChangePassword = m_reason == LC::LogonUIRequestReason_LogonUIChange;

	BOOLEAN isFirstLogonAfterSignOutOrSwitchUser = FALSE;
	m_userSettingManager->get_IsFirstLogonAfterSignOutOrSwitchUser(&isFirstLogonAfterSignOutOrSwitchUser);

	BOOLEAN isUserLoggedOn = 0;
	RETURN_IF_FAILED(user->get_IsLoggedIn(&isUserLoggedOn)); // 77

	bool isLoggedOnUserPresentUnlockOrLogon =
		(m_reason == LC::LogonUIRequestReason_LogonUIUnlock || m_reason == LC::LogonUIRequestReason_LogonUILogon) && isUserLoggedOn;
	bool isRemoteSessionUserNotLoggedOn = GetSystemMetrics(SM_REMOTESESSION) && !isUserLoggedOn;

	*value = isForceAutoLogon
		|| isChangePassword
		|| (!isFirstLogonAfterSignOutOrSwitchUser && !isLoggedOnUserPresentUnlockOrLogon)
		|| isRemoteSessionUserNotLoggedOn;
	return S_OK;
}
