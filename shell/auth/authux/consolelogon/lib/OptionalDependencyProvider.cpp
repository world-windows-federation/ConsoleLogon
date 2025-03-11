#include "pch.h"

#include "optionaldependencyprovider.h"

#include "DefaultSelector.h"

using namespace Microsoft::WRL;

class CredUXTelemetryProvider final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, LCPD::ITelemetryDataProvider
		, Microsoft::WRL::FtmBase
	>
{
public:
	CredUXTelemetryProvider();
	~CredUXTelemetryProvider();

	HRESULT RuntimeClassInitialize(LC::LogonUIRequestReason reason);

	virtual HRESULT STDMETHODCALLTYPE GetUserAccountKind(HSTRING str, LCPD::UserAccountKind* kind) override;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentLogonUIRequestReason(DWORD* reason) override;

	LC::LogonUIRequestReason m_reason;
};

CredUXTelemetryProvider::CredUXTelemetryProvider()
	: m_reason(LC::LogonUIRequestReason_LogonUILogon)
{
}

CredUXTelemetryProvider::~CredUXTelemetryProvider()
{
}

HRESULT CredUXTelemetryProvider::RuntimeClassInitialize(LC::LogonUIRequestReason reason)
{
	m_reason = reason;
	return S_OK;
}

HRESULT CredUXTelemetryProvider::GetUserAccountKind(HSTRING str, LCPD::UserAccountKind* kind)
{
	if (!str)
		return S_OK;

	//TODO: add proper thing
	*kind = LCPD::UserAccountKind_Local;

	return S_OK;
}

HRESULT CredUXTelemetryProvider::get_CurrentLogonUIRequestReason(DWORD* reason)
{
	*reason = m_reason;
	return S_OK;
}

HRESULT OptionalDependencyProvider::RuntimeClassInitialize(
	LC::LogonUIRequestReason reason, IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
	LCPD::IDisplayStateProvider* displayStateProvider)
{
	m_autoLogonManager = autoLogonManager;
	m_displayStateProvider = displayStateProvider;
	RETURN_IF_FAILED(MakeAndInitialize<DefaultSelector>(&m_defaultSelector, userSettingManager, reason)); // 16
	//@Mod
	RETURN_IF_FAILED(MakeAndInitialize<CredUXTelemetryProvider>(&m_telemetryProvider, reason));
	//RETURN_IF_FAILED(userSettingManager->get_TelemetryDataProvider(&m_telemetryProvider)); // 17
	return S_OK;
}

HRESULT OptionalDependencyProvider::GetOptionalDependency(LCPD::OptionalDependencyKind kind, IInspectable** value)
{
	*value = nullptr;

	switch (kind)
	{
		case LCPD::OptionalDependencyKind_AutoLogonManager:
			RETURN_IF_FAILED(m_autoLogonManager.CopyTo(value)); // 27
			break;
		case LCPD::OptionalDependencyKind_DefaultSelector:
			RETURN_IF_FAILED(m_defaultSelector.CopyTo(value)); // 30
			break;
		case LCPD::OptionalDependencyKind_DisplayState:
			RETURN_IF_FAILED(m_displayStateProvider.CopyTo(value)); // 33
			break;
		case LCPD::OptionalDependencyKind_TelemetryData:
			RETURN_IF_FAILED(m_telemetryProvider.CopyTo(value)); // 36
			break;
	}

	return S_OK;
}
