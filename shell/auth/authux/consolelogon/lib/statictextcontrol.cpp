#include "pch.h"

#include "statictextcontrol.h"

using namespace Microsoft::WRL;

StaticTextControl::StaticTextControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_IsVisible(false)
{
}

StaticTextControl::~StaticTextControl()
{
}

HRESULT StaticTextControl::RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource)
{
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 20

	RETURN_IF_FAILED(Advise(dataSource)); // 22
	auto scopeExit = wil::scope_exit([this]() -> void
	{
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 28

	RETURN_IF_FAILED(Repaint(view)); // 30

	scopeExit.release();
	return S_OK;
}

HRESULT StaticTextControl::v_OnFocusChange(BOOL hasFocus)
{
	return E_NOTIMPL;
}

HRESULT StaticTextControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return E_NOTIMPL;
}

HRESULT StaticTextControl::Repaint(IConsoleUIView* view)
{
	Wrappers::HString content;
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 40

	UINT32 contentLength = WindowsGetStringLen(content);

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 45

	UINT controlSize = m_IsVisible ? contentLength / consoleWidth + 1 : 0;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(FALSE, controlSize, view)); // 51
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 57
		m_VisibleControlSize = controlSize;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(content.GetRawBuffer(nullptr), contentLength, ColorScheme::Normal, consoleWidth, m_VisibleControlSize)); // 63
	}

	WindowsDeleteString(content);
	return S_OK;
}

HRESULT StaticTextControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible;
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 88
		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 92
		}
	}
	else if (changeKind == LCPD::CredentialFieldChangeKind_SetString)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 96
	}
	return S_OK;
}

bool StaticTextControl::v_HasFocus()
{
	return false;
}
