#include "pch.h"

#include "comboboxcontrol.h"

#include "basictextcontrol.h"

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

HRESULT ComboboxControl::RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource,
	IComboBoxSelectCallback* callback)
{
	if (m_dataSource)
	{
		m_dataSource.Reset();
	}

	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 24

	//TODO: verify if this is operator= or std::move
	m_callback = callback;

	RETURN_IF_FAILED(Advise(dataSource)); // 27

	m_items.Reset();
	RETURN_IF_FAILED(m_dataSource->get_Items(&m_items)); // 33

	RETURN_IF_FAILED(m_items->add_VectorChanged(this,&m_itemsChangedToken)); // 34

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 37

	RETURN_IF_FAILED(ComboboxControl::Repaint(view)); // 39

	return S_OK;
}

HRESULT ComboboxControl::Invoke(WF::Collections::IObservableVector<HSTRING>* sender,
	WF::Collections::IVectorChangedEventArgs* args)
{
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 62
	return S_OK;
}

HRESULT ComboboxControl::v_Unadvise()
{
	if (m_itemsChangedToken.value)
		RETURN_IF_FAILED(m_items->remove_VectorChanged(m_itemsChangedToken)); // 50
	m_items.Reset();

	if (m_callback.Get())
		m_callback->Release();
	RETURN_IF_FAILED(CredentialFieldControlBase::v_Unadvise()); // 55

	return S_OK;
}

HRESULT ComboboxControl::v_OnFocusChange(int hasFocus)
{
	m_HasFocus = hasFocus;
	RETURN_IF_FAILED(Repaint(m_view.Get())); //114
	return S_OK;
}

HRESULT ComboboxControl::v_HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;
	
	WORD wVirtualKeyCode = keyEvent->wVirtualKeyCode;
	if (wVirtualKeyCode == VK_RETURN || wVirtualKeyCode == VK_SPACE)
	{
		RETURN_IF_FAILED(m_callback->OnComboboxSelected(m_dataSource.Get())); // 125
		*wasHandled = true;
	}
	
	return S_OK;
}

HRESULT ComboboxControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible = false;
		
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 138
		
		if ( isVisible != m_IsVisible )
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 142
		}
	}
	else if ( changeKind == LCPD::CredentialFieldChangeKind_SetString
		  || changeKind == LCPD::CredentialFieldChangeKind_SetComboBoxSelected )
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
	Microsoft::WRL::ComPtr<WF::Collections::IVector<HSTRING> > items;
	RETURN_IF_FAILED(m_items.As(&items)); // 69
	
	int selectedIndex = 0;
	RETURN_IF_FAILED(m_dataSource->get_SelectedIndex(&selectedIndex)); // 72

	Microsoft::WRL::Wrappers::HString content;
	if (selectedIndex >= 0)
	{
		RETURN_IF_FAILED(items->GetAt(selectedIndex,content.ReleaseAndGetAddressOf())); // 77
	}
	else
	{
		RETURN_IF_FAILED(WindowsCreateString(L"   ",3,content.ReleaseAndGetAddressOf())); // 81
	}
	UINT32 length = content.Length();

	UINT consoleWidth = 0;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 87

	UINT consoleHeight = 0;
	if (m_IsVisible)
		consoleHeight = length / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(ControlBase::Initialize(true,consoleHeight,view)); // 93
		m_VisibleControlSize = consoleHeight;
		m_isInitialized = true;
	}
	else if ( consoleHeight != this->m_VisibleControlSize )
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(),consoleHeight)); // 99
		m_VisibleControlSize = consoleHeight;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(ControlBase::PaintArea(content.GetRawBuffer(nullptr),length,FocusToColorScheme(m_HasFocus), consoleWidth,m_VisibleControlSize)); // 105
	}
	
	items.Reset();

	return S_OK;
}
