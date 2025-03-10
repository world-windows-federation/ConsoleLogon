#include "pch.h"

#include "commandlinkcontrol.h"

#include <string_view>

#include "basictextcontrol.h"

CommandLinkControl::CommandLinkControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_IsVisible(false)
	, m_HasFocus(false)
{
}

CommandLinkControl::~CommandLinkControl()
{
}

HRESULT CommandLinkControl::RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource)
{
	//if (m_dataSource)
	//{
	//	m_dataSource.Reset();
	//}

	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 21

	RETURN_IF_FAILED(CredentialFieldControlBase::Advise(dataSource)); // 23

	auto scopeExit = wil::scope_exit([this]() -> void {
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(CredentialFieldControlBase::GetVisibility(&m_IsVisible)); // 30

	RETURN_IF_FAILED(Repaint(view)); // 32

	scopeExit.release();
	return S_OK;
}

HRESULT CommandLinkControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 75

	return S_OK;
}

HRESULT CommandLinkControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	if (keyEvent->wVirtualKeyCode == VK_RETURN)
	{
		RETURN_IF_FAILED(m_dataSource->Invoke()); // 85
		*wasHandled = TRUE;
	}
	return S_OK;
}

HRESULT CommandLinkControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible = false;
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 98

		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 102
		}
	}
	else if (changeKind == LCPD::CredentialFieldChangeKind_SetString)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 106
	}

	return S_OK;
}

//optimized to be CheckboxControl::v_HasFocus, since they do the same thing and share same shifted ptr delta
bool CommandLinkControl::v_HasFocus()
{
	return m_HasFocus;
}

HRESULT CommandLinkControl::Repaint(IConsoleUIView* view)
{
	Microsoft::WRL::Wrappers::HString content;
	
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 43

	UINT length = WindowsGetStringLen(content.Get());

	UINT consoleWidth = 0;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 48

	UINT consoleHeight = m_IsVisible ? (length / consoleWidth + 1) : (0); //turned into ternary operator so line numbers line up

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(true,consoleHeight,view)); // 54
		m_VisibleControlSize = consoleHeight;
		m_isInitialized = true;
	}
	else if ( consoleHeight != this->m_VisibleControlSize )
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(),consoleHeight)); // 60
		m_VisibleControlSize = consoleHeight;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(ControlBase::PaintArea(content.GetRawBuffer(nullptr),length,FocusToColorScheme(m_HasFocus), consoleWidth,m_VisibleControlSize)); // 105
	}

	return S_OK;
}
