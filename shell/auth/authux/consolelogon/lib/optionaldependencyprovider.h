#pragma once

#include "pch.h"

class OptionalDependencyProvider final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, LCPD::IOptionalDependencyProvider
		, Microsoft::WRL::FtmBase
	>
{
public:
	HRESULT RuntimeClassInitialize(
		LC::LogonUIRequestReason reason, IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
		LCPD::IDisplayStateProvider* displayStateProvider);

	//~ Begin IOptionalDependencyProvider Interface
	STDMETHODIMP GetOptionalDependency(LCPD::OptionalDependencyKind kind, IInspectable** value) override;
	//~ End IOptionalDependencyProvider Interface

private:
	Microsoft::WRL::ComPtr<LCPD::ICredProvDefaultSelector> m_defaultSelector;
	Microsoft::WRL::ComPtr<LCPD::ITelemetryDataProvider> m_telemetryProvider;
	Microsoft::WRL::ComPtr<IInspectable> m_autoLogonManager;
	Microsoft::WRL::ComPtr<LCPD::IDisplayStateProvider> m_displayStateProvider;
};
