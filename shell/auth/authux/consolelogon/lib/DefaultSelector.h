#pragma once

#include "pch.h"

class DefaultSelector final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, LCPD::ICredProvDefaultSelector
		, Microsoft::WRL::FtmBase
	>
{
public:
	HRESULT RuntimeClassInitialize(LC::IUserSettingManager* userSettingManager, LC::LogonUIRequestReason reason);

	//~ Begin LCPD::ICredProvDefaultSelector Interface
	STDMETHODIMP get_UseLastLoggedOnProvider(BOOLEAN* value) override;
#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
	STDMETHODIMP get_PreferredProvidersDup(WFC::IVectorView<GUID>** ppValue) override;
#endif
	STDMETHODIMP get_PreferredProviders(WFC::IVectorView<GUID>** ppValue) override;
	STDMETHODIMP get_ExcludedProviders(WFC::IVectorView<GUID>** ppValue) override;
	STDMETHODIMP get_DefaultUserSid(HSTRING* value) override;
	STDMETHODIMP AllowAutoSubmitOnSelection(LCPD::IUser* user, BOOLEAN* value) override;
	//~ End LCPD::ICredProvDefaultSelector Interface

private:
	Microsoft::WRL::ComPtr<WFC::IVectorView<GUID>> m_preferredProviders;
	Microsoft::WRL::ComPtr<WFC::IVectorView<GUID>> m_excludedProviders;
	Microsoft::WRL::ComPtr<LC::IUserSettingManager> m_userSettingManager;
	LC::LogonUIRequestReason m_reason;
};
