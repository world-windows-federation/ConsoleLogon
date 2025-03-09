#include "pch.h"

#include "selectableuserorcredentialcontrol.h"

#include "resource.h"

using namespace Microsoft::WRL;

SelectableUserOrCredentialControl::SelectableUserOrCredentialControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_HasFocus(false)
{
}

HRESULT SelectableUserOrCredentialControl::RuntimeClassInitialize(IConsoleUIView* view, IInspectable* dataSource)
{
	if (FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSourceUser))))
	{
		RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSourceCredential))); // 24
	}

	RETURN_IF_FAILED(Repaint(view)); // 27
	return S_OK;
}

HRESULT SelectableUserOrCredentialControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus != 0;

	RETURN_IF_FAILED(Repaint(m_view.Get())); // 80
	return S_OK;
}

HRESULT SelectableUserOrCredentialControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return S_OK;
}

HRESULT SelectableUserOrCredentialControl::Repaint(IConsoleUIView* view)
{
	Wrappers::HString content;
	if (m_dataSourceUser.Get())
	{
		RETURN_IF_FAILED(m_dataSourceUser->get_DisplayName(content.ReleaseAndGetAddressOf())); // 37
	}
	else
	{
		RETURN_IF_FAILED(m_dataSourceCredential->get_LogoLabel(content.ReleaseAndGetAddressOf())); // 43

		if (!content.Get())
		{
			CoTaskMemNativeString defaultContent;
			RETURN_IF_FAILED(defaultContent.Initialize(HINST_THISCOMPONENT, IDS_USER)); // 48
			RETURN_IF_FAILED(defaultContent.Get() ? content.Set(defaultContent.Get()) : E_POINTER); // 49
		}
	}

	UINT contentLength = content.Length();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 56

	UINT controlSize = contentLength / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(TRUE, controlSize, view)); // 62
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 68
		m_VisibleControlSize = controlSize;
	}

	RETURN_IF_FAILED(PaintArea(content.GetRawBuffer(nullptr), contentLength, m_HasFocus ? ColorScheme::Inverted : ColorScheme::Normal, consoleWidth, m_VisibleControlSize)); // 72
	return S_OK;
}
