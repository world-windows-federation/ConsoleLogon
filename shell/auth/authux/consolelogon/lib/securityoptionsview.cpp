#include "pch.h"

#include "securityoptionsview.h"

#include "securityoptioncontrol.h"

using namespace Microsoft::WRL;

SecurityOptionsView::SecurityOptionsView()
{
}

SecurityOptionsView::~SecurityOptionsView()
{
}

HRESULT SecurityOptionsView::RuntimeClassInitialize(LC::LogonUISecurityOptions options,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion)
{
	RETURN_IF_FAILED(Initialize()); // 23

	if ((options & LC::LogonUISecurityOptions_ChangePassword) != 0)
	{
		ComPtr<SecurityOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_ChangePassword, &completion)); // 28
	}

	if ((options & LC::LogonUISecurityOptions_Lock) != 0)
	{
		ComPtr<SecurityOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_Lock, &completion)); // 34
	}

	if ((options & LC::LogonUISecurityOptions_LogOff) != 0)
	{
		ComPtr<SecurityOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_LogOff, &completion)); // 40
	}

	if ((options & LC::LogonUISecurityOptions_SwitchUser) != 0)
	{
		ComPtr<SecurityOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_SwitchUser, &completion)); // 46
	}

	if ((options & LC::LogonUISecurityOptions_TaskManager) != 0)
	{
		ComPtr<SecurityOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_TaskManager, &completion)); // 52
	}

	ComPtr<SecurityOptionControl> optionControl;
	RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionControl>(&optionControl, this, LC::LogonUISecurityOptions_Cancel, &completion)); // 56

	m_completion = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>>(completion);
	RETURN_HR_IF_NULL(E_OUTOFMEMORY, m_completion.get()); // 59

	return S_OK;
}

HRESULT SecurityOptionsView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	if (keyEvent->wVirtualKeyCode != VK_ESCAPE)
		return S_OK;

	ComPtr<LC::ILogonUISecurityOptionsResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(
		Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_LogonUISecurityOptionsResult).Get(), &factory)); // 70

	ComPtr<LC::ILogonUISecurityOptionsResult> optionResult;
	RETURN_IF_FAILED(factory->CreateSecurityOptionsResult(LC::LogonUISecurityOptions_Cancel, LC::LogonUIShutdownChoice_None, &optionResult)); // 73

	// more bad code from them, not fixing it yet just in case the badness is needed
	WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult> result = m_completion->GetResult();
	RETURN_IF_FAILED(result.Set(optionResult.Get())); // 76

	m_completion->Complete(S_OK);
	m_completion.reset();

	*wasHandled = TRUE;
	return S_OK;
}
