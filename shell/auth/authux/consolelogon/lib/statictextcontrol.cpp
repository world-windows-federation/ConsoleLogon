#include "statictextcontrol.h"
using namespace Windows::Internal::UI::Logon::CredProvData;

StaticTextControl::StaticTextControl()
	: m_VisibleControlSize(0)
	, m_IsVisible(false)
	, m_isInitialized(false)
{
}

StaticTextControl::~StaticTextControl()
{
}

// TODO: fix
HRESULT StaticTextControl::RuntimeClassInitialize(IConsoleUIView* view, ICredentialField* dataSource)
{
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 20

	RETURN_IF_FAILED(CredentialFieldControlBase::Advise(dataSource)); // 22

	HRESULT Visibility = CredentialFieldControlBase::GetVisibility(&m_IsVisible);
	if (FAILED(Visibility))
	{
		LOG_HR(Visibility); // 28
	}
	else
	{
		RETURN_IF_FAILED(Repaint(view)); // 30
		return S_OK;
	}
	ControlBase::Unadvise();

	return Visibility;
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
	Microsoft::WRL::Wrappers::HString content;
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 40

	UINT32 StringLen = WindowsGetStringLen(content);
	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 45

	UINT controlSize;
	if (m_IsVisible)
	{
		controlSize = StringLen / consoleWidth + 1;
	}
	else
	{
		controlSize = 0;
	}

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(FALSE, controlSize, view)); // 51
		m_isInitialized = true;
	}

	if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 57
	}
	m_VisibleControlSize = controlSize;

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(content.GetRawBuffer(NULL), StringLen, ColorScheme::Normal, consoleWidth, m_VisibleControlSize)); // 63
	}

	WindowsDeleteString(content);
	return S_OK;
}

HRESULT StaticTextControl::v_OnFieldChange(CredentialFieldChangeKind changeKind)
{
	if (changeKind == CredentialFieldChangeKind_State)
	{
		bool isVisible;
		RETURN_IF_FAILED(CredentialFieldControlBase::GetVisibility(&isVisible)); // 88

		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 92
		}
	}
	else if (changeKind == CredentialFieldChangeKind_SetString)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 96
	}
	return S_OK;
}
