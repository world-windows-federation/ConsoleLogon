#include "pch.h"

#include "commandlinkcontrol.h"

using namespace Microsoft::WRL;

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
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 21

	RETURN_IF_FAILED(CredentialFieldControlBase::Advise(dataSource)); // 23

	auto scopeExit = wil::scope_exit([this]() -> void
	{
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(CredentialFieldControlBase::GetVisibility(&m_IsVisible)); // 30

	RETURN_IF_FAILED(Repaint(view)); // 32

	scopeExit.release();
	return S_OK;
}

HRESULT CommandLinkControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus != 0;
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
		bool isVisible;
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
	Wrappers::HString content;
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 43

	UINT contentLength = content.Length();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 48

	UINT controlSize = m_IsVisible ? contentLength / consoleWidth + 1 : 0;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(true, controlSize, view)); // 54
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 60
		m_VisibleControlSize = controlSize;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(ControlBase::PaintArea(content.GetRawBuffer(nullptr), contentLength, FocusToColorScheme(m_HasFocus), consoleWidth, m_VisibleControlSize)); // 105
	}

	return S_OK;
}
