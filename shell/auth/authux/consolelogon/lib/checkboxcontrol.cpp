#include "pch.h"

#include "checkboxcontrol.h"

#include "basictextcontrol.h"

CheckboxControl::CheckboxControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_IsVisible(false)
	, m_HasFocus(false)
{
}

CheckboxControl::~CheckboxControl()
{
}

HRESULT CheckboxControl::RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource)
{
	if (m_dataSource.Get())
	{
		m_dataSource->Release();
	}

	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 23

	RETURN_IF_FAILED(Advise(dataSource)); // 25

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 32

	RETURN_IF_FAILED(Repaint(view)); // 34

	return S_OK;
}

HRESULT CheckboxControl::v_OnFocusChange(int hasFocus)
{
	m_HasFocus = hasFocus != 0;

	RETURN_IF_FAILED(Repaint(m_view.Get())); // 93
	return S_OK;
}

HRESULT CheckboxControl::v_HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;

	WORD wVirtualKeyCode = keyEvent->wVirtualKeyCode;
	if (wVirtualKeyCode == VK_RETURN || wVirtualKeyCode == VK_SPACE)
	{
		RETURN_IF_FAILED(Toggle()); // 104
		*wasHandled = true;
	}
	
	return S_OK;
}

HRESULT CheckboxControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	//if (changeKind && (changeKind-2) <= 1)
	if (changeKind == LCPD::CredentialFieldChangeKind_InteractiveState) //note: pseudocode has it done through maths first then a cmp, but with its logic, it SHOULD always only be true on this value
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 126
	}
	else
	{
		bool isVisible = false;
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 117
		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 121
		}
	}

	return S_OK;
}

bool CheckboxControl::v_HasFocus()
{
	return m_HasFocus;
}

//TODO: have someone verify this, since its dealing with big inlined functions
HRESULT CheckboxControl::Repaint(IConsoleUIView* view)
{
	Microsoft::WRL::Wrappers::HString boxLabel;
	
	RETURN_IF_FAILED(m_dataSource->get_BoxLabel(boxLabel.ReleaseAndGetAddressOf())); // 44

	bool isChecked = false;
	RETURN_IF_FAILED(m_dataSource->get_Checked(&isChecked)); // 47

	CoTaskMemNativeString checkMark;
	RETURN_IF_FAILED(checkMark.Initialize(HINST_THISCOMPONENT,((isChecked == 0) + 126))); // 50

	CoTaskMemNativeString checkBoxAndLabel;
	RETURN_IF_FAILED(checkBoxAndLabel.InitializeResMessage(HINST_THISCOMPONENT,125,checkMark.Get(),boxLabel.GetRawBuffer(0))); //53
	
	size_t count = checkBoxAndLabel.GetCount();
	UINT consoleWidth = 0;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 58

	UINT controlSize = count / consoleWidth + 1;
	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(true, controlSize, view)); // 64
		m_isInitialized = true;
	}

	if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 70
	}
	m_VisibleControlSize = controlSize;

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(checkBoxAndLabel.Get(),count,FocusToColorScheme(m_HasFocus),consoleWidth,m_VisibleControlSize)); // 76
	}

	return S_OK;
}

HRESULT CheckboxControl::Toggle()
{
	bool isChecked = false;
	RETURN_IF_FAILED(m_dataSource->get_Checked(&isChecked)); // 85
	RETURN_IF_FAILED(m_dataSource->put_Checked(isChecked == false)); // 86
	
	return S_OK;
}
