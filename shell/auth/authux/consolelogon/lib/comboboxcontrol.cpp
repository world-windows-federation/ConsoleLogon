#include "pch.h"

#include "comboboxcontrol.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

ComboboxControl::ComboboxControl()
	: m_itemsChangedToken()
	, m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_IsVisible(false)
	, m_HasFocus(false)
{
}

ComboboxControl::~ComboboxControl()
{
}

HRESULT ComboboxControl::RuntimeClassInitialize(
	IConsoleUIView* view, LCPD::ICredentialField* dataSource, IComboBoxSelectCallback* callback)
{
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 24
	m_callback = callback;

	RETURN_IF_FAILED(Advise(dataSource)); // 27
	auto scopeExit = wil::scope_exit([this]() -> void
	{
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(m_dataSource->get_Items(&m_items)); // 33
	RETURN_IF_FAILED(m_items->add_VectorChanged(this, &m_itemsChangedToken)); // 34

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 37

	RETURN_IF_FAILED(ComboboxControl::Repaint(view)); // 39

	scopeExit.release();
	return S_OK;
}

HRESULT ComboboxControl::Invoke(WFC::IObservableVector<HSTRING>* sender, WFC::IVectorChangedEventArgs* args)
{
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 62
	return S_OK;
}

HRESULT ComboboxControl::v_Unadvise()
{
	if (m_itemsChangedToken.value)
	{
		LOG_IF_FAILED(m_items->remove_VectorChanged(m_itemsChangedToken)); // 50
	}
	m_items.Reset();

	m_callback.Reset();
	RETURN_IF_FAILED(CredentialFieldControlBase::v_Unadvise()); // 55

	return S_OK;
}

HRESULT ComboboxControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 114
	return S_OK;
}

HRESULT ComboboxControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	if (keyEvent->wVirtualKeyCode == VK_RETURN || keyEvent->wVirtualKeyCode == VK_SPACE)
	{
		RETURN_IF_FAILED(m_callback->OnComboboxSelected(m_dataSource.Get())); // 125
		*wasHandled = TRUE;
	}

	return S_OK;
}

HRESULT ComboboxControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible = false;

		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 138

		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 142
		}
	}
	else if (changeKind == LCPD::CredentialFieldChangeKind_SetString
		|| changeKind == LCPD::CredentialFieldChangeKind_SetComboBoxSelected)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 147
	}

	return S_OK;
}

bool ComboboxControl::v_HasFocus()
{
	return m_HasFocus;
}

HRESULT ComboboxControl::Repaint(IConsoleUIView* view)
{
	ComPtr<WFC::IVector<HSTRING>> items;
	RETURN_IF_FAILED(m_items.As(&items)); // 69

	int selectedIndex;
	RETURN_IF_FAILED(m_dataSource->get_SelectedIndex(&selectedIndex)); // 72

	Wrappers::HString content;
	if (selectedIndex >= 0)
	{
		RETURN_IF_FAILED(items->GetAt(selectedIndex, content.ReleaseAndGetAddressOf())); // 77
	}
	else
	{
		RETURN_IF_FAILED(content.Set(L"   ")); // 81
	}
	UINT32 contentLength = content.Length();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 87

	UINT controlSize = m_IsVisible ? contentLength / consoleWidth + 1 : 0;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(true, controlSize, view)); // 93
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 99
		m_VisibleControlSize = controlSize;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(ControlBase::PaintArea(content.GetRawBuffer(nullptr), contentLength, FocusToColorScheme(m_HasFocus), consoleWidth, m_VisibleControlSize)); // 105
	}

	items.Reset();

	return S_OK;
}
