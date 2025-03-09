#include "pch.h"

#include "messageoptioncontrol.h"

#include "basictextcontrol.h"

#include "resource.h"

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

	RETURN_HR_IF(E_OUTOFMEMORY,!m_completion); // 26

	UINT resourceId;
	switch (option)
	{
	case Ok:
		resourceId = IDS_OK;
		break;
	case Cancel:
		resourceId = IDS_CANCEL;
		break;
	case Yes:
		resourceId = IDS_YES;
		break;
	case No:
		resourceId = IDS_NO;
		break;
	default:
		RETURN_HR(E_INVALIDARG); // 45
	}

	RETURN_IF_FAILED(m_label.Initialize(HINST_THISCOMPONENT,resourceId)); // 48

	RETURN_IF_FAILED(Repaint(view)); // 50

	return S_OK;
}

HRESULT MessageOptionControl::v_OnFocusChange(int hasFocus)
{
	m_hasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 84

	return S_OK;
}

HRESULT MessageOptionControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	if (keyEvent->wVirtualKeyCode != VK_RETURN || !m_completion)
		return 0;

	UINT optflag;
	
	switch (m_option)
	{
		case Ok:
			optflag = 1;
			break;
		case Cancel:
			optflag = 2;
			break;
		case Yes:
			optflag = 6;
			break;
		case No:
			optflag = 7;
			break;
		default:
			RETURN_HR(E_INVALIDARG); //111
	}

	Microsoft::WRL::ComPtr<LC::IMessageDisplayResultFactory> factory;
	RETURN_IF_FAILED(WF::GetActivationFactory(Microsoft::WRL::Wrappers::HStringReference(L"Windows.Internal.UI.Logon.Controller.MessageDisplayResult").Get(),&factory)); // 115

	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> messageResult;
	RETURN_IF_FAILED(factory->CreateMessageDisplayResult(optflag, &messageResult)); // 118

	RETURN_IF_FAILED(m_completion->GetResult().Set(messageResult.Get())); // 120

	m_completion->Complete(S_OK);

	//TODO: VERIFY THIS IS CORRECT
	m_completion.reset();
	*wasHandled = TRUE;

	return S_OK;
}

HRESULT MessageOptionControl::Repaint(IConsoleUIView* view)
{
	UINT length = (UINT)m_label.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 60

	UINT consoleHeight = length / consoleWidth + 1;
	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(TRUE,consoleHeight,view)); // 66
		m_isInitialized = true;
		//note: theres a goto to go to setting this, which would avoid the resizecontrol call, no clue why they do this, but it is done, unless ida pseudocode is wrong
		m_VisibleControlSize = consoleHeight;
	}
	
	if ( consoleHeight != this->m_VisibleControlSize )
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(),consoleHeight)); // 72
		m_VisibleControlSize = consoleHeight;
	}

	RETURN_IF_FAILED(PaintArea(m_label.Get(),length,FocusToColorScheme(m_hasFocus),consoleWidth,m_VisibleControlSize)); // 76

	return S_OK;
}
