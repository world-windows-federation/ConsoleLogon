#include "pch.h"

#include "optionaldependencyprovider.h"

#include "DefaultSelector.h"

using namespace Microsoft::WRL;

HRESULT OptionalDependencyProvider::RuntimeClassInitialize(
	LC::LogonUIRequestReason reason, IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
	LCPD::IDisplayStateProvider* displayStateProvider)
{
	m_autoLogonManager = autoLogonManager;
	m_displayStateProvider = displayStateProvider;
	RETURN_IF_FAILED(MakeAndInitialize<DefaultSelector>(&m_defaultSelector, userSettingManager, reason)); // 16
	RETURN_IF_FAILED(userSettingManager->get_TelemetryDataProvider(&m_telemetryProvider)); // 17
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
