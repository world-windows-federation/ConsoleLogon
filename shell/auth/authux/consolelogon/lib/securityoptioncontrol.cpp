#include "pch.h"

#include "securityoptioncontrol.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

SecurityOptionControl::SecurityOptionControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_hasFocus(false)
	, m_option(LC::LogonUISecurityOptions_Cancel)
{
}

SecurityOptionControl::~SecurityOptionControl()
{
}

HRESULT SecurityOptionControl::RuntimeClassInitialize(IConsoleUIView* view, LC::LogonUISecurityOptions option,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion)
{
	m_option = option;
	m_completion = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>>(completion);

	RETURN_HR_IF(E_OUTOFMEMORY, !m_completion);

	UINT resourceId;
	switch (option)
	{
		case LC::LogonUISecurityOptions_Cancel:
			resourceId = IDS_CANCEL;
			break;

		case LC::LogonUISecurityOptions_Lock:
			resourceId = IDS_LOCK;
			break;

		case LC::LogonUISecurityOptions_LogOff:
			resourceId = IDS_SIGNOUT;
			break;

		case LC::LogonUISecurityOptions_ChangePassword:
			resourceId = IDS_CHANGEPASSWD;
			break;

		case LC::LogonUISecurityOptions_TaskManager:
			resourceId = IDS_TASKMGR;
			break;

		case LC::LogonUISecurityOptions_SwitchUser:
			resourceId = IDS_SWITCHUSER;
			break;

		default:
			RETURN_HR(E_INVALIDARG); // 51
	}

	RETURN_IF_FAILED(m_label.Initialize(HINST_THISCOMPONENT, resourceId)); // 54

	RETURN_IF_FAILED(Repaint(view)); // 56

	return S_OK;
}

HRESULT SecurityOptionControl::v_OnFocusChange(BOOL hasFocus)
{
	m_hasFocus = hasFocus != 0;

	RETURN_IF_FAILED(Repaint(m_view.Get()));

	return S_OK;
}

HRESULT SecurityOptionControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	if (keyEvent->wVirtualKeyCode != VK_RETURN)
		return S_OK;

	if (!m_completion)
		return S_OK;

	ComPtr<LC::ILogonUISecurityOptionsResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(
		Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_LogonUISecurityOptionsResult).Get(), &factory)); // 101

	ComPtr<LC::ILogonUISecurityOptionsResult> optionResult;
	RETURN_IF_FAILED(factory->CreateSecurityOptionsResult(m_option, LC::LogonUIShutdownChoice_None, &optionResult)); // 104

	RETURN_IF_FAILED(m_completion->GetResult().Set(optionResult.Get())); // 106

	m_completion->Complete(S_OK);
	m_completion.reset();

	*wasHandled = TRUE;

	return S_OK;
}

HRESULT SecurityOptionControl::Repaint(IConsoleUIView* view)
{
	UINT contentLength = (UINT)m_label.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 66

	UINT controlSize = contentLength / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(true, controlSize, view)); // 72
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 78
		m_VisibleControlSize = controlSize;
	}

	RETURN_IF_FAILED(PaintArea(m_label.Get(), contentLength, FocusToColorScheme(m_hasFocus), consoleWidth, m_VisibleControlSize)); // 82

	return S_OK;
}

