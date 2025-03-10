#include "pch.h"

#include "messageoptioncontrol.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

MessageOptionControl::MessageOptionControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_hasFocus(false)
	, m_option(MessageOptionFlag::None)
{
}

MessageOptionControl::~MessageOptionControl()
{
}

HRESULT MessageOptionControl::RuntimeClassInitialize(IConsoleUIView* view, MessageOptionFlag option,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	m_option = option;
	m_completion = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>>(completion);
	RETURN_IF_NULL_ALLOC(m_completion); // 26

	UINT resourceId;
	switch (option)
	{
		case MessageOptionFlag::Ok:
			resourceId = IDS_OK;
			break;
		case MessageOptionFlag::Cancel:
			resourceId = IDS_CANCEL;
			break;
		case MessageOptionFlag::Yes:
			resourceId = IDS_YES;
			break;
		case MessageOptionFlag::No:
			resourceId = IDS_NO;
			break;

		default:
			RETURN_HR(E_INVALIDARG); // 45
	}

	RETURN_IF_FAILED(m_label.Initialize(HINST_THISCOMPONENT, resourceId)); // 48

	RETURN_IF_FAILED(Repaint(view)); // 50

	return S_OK;
}

HRESULT MessageOptionControl::v_OnFocusChange(BOOL hasFocus)
{
	m_hasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 84

	return S_OK;
}

HRESULT MessageOptionControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	if (keyEvent->wVirtualKeyCode != VK_RETURN || !m_completion)
		return S_OK;

	UINT messageCode;
	switch (m_option)
	{
		case MessageOptionFlag::Ok:
			messageCode = 1;
			break;
		case MessageOptionFlag::Cancel:
			messageCode = 2;
			break;
		case MessageOptionFlag::Yes:
			messageCode = 6;
			break;
		case MessageOptionFlag::No:
			messageCode = 7;
			break;

		default:
			RETURN_HR(E_INVALIDARG); // 111
	}

	ComPtr<LC::IMessageDisplayResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(
		Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_MessageDisplayResult).Get(), &factory)); // 115

	ComPtr<LC::IMessageDisplayResult> messageResult;
	RETURN_IF_FAILED(factory->CreateMessageDisplayResult(messageCode, &messageResult)); // 118

	RETURN_IF_FAILED(m_completion->GetResult().Set(messageResult.Get())); // 120

	m_completion->Complete(S_OK);
	m_completion.reset();

	*wasHandled = TRUE;

	return S_OK;
}

HRESULT MessageOptionControl::Repaint(IConsoleUIView* view)
{
	UINT contentLength = (UINT)m_label.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 60

	UINT controlSize = contentLength / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(TRUE, controlSize, view)); // 66
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 72
		m_VisibleControlSize = controlSize;
	}

	RETURN_IF_FAILED(PaintArea(m_label.Get(), contentLength, FocusToColorScheme(m_hasFocus), consoleWidth, m_VisibleControlSize)); // 76

	return S_OK;
}
