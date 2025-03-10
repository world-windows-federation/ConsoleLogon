#include "pch.h"

#include "checkboxcontrol.h"

#include "basictextcontrol.h"
#include "resource.h"

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
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 23

	RETURN_IF_FAILED(Advise(dataSource)); // 25

	auto scopeExit = wil::scope_exit([this]() -> void
	{
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 32

	RETURN_IF_FAILED(Repaint(view)); // 34

	scopeExit.release();
	return S_OK;
}

HRESULT CheckboxControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus != 0;

	RETURN_IF_FAILED(Repaint(m_view.Get())); // 93
	return S_OK;
}

HRESULT CheckboxControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;

	if (keyEvent->wVirtualKeyCode == VK_RETURN || keyEvent->wVirtualKeyCode == VK_SPACE)
	{
		RETURN_IF_FAILED(Toggle()); // 104
		*wasHandled = true;
	}

	return S_OK;
}

HRESULT CheckboxControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible;
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 117
		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 121
		}
	}
	else if (changeKind == LCPD::CredentialFieldChangeKind_SetString || changeKind == LCPD::CredentialFieldChangeKind_SetCheckbox)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 126
	}

	return S_OK;
}

bool CheckboxControl::v_HasFocus()
{
	return m_HasFocus;
}

HRESULT CheckboxControl::Repaint(IConsoleUIView* view)
{
	Microsoft::WRL::Wrappers::HString boxLabel;
	RETURN_IF_FAILED(m_dataSource->get_BoxLabel(boxLabel.ReleaseAndGetAddressOf())); // 44

	bool isChecked;
	RETURN_IF_FAILED(m_dataSource->get_Checked(&isChecked)); // 47

	CoTaskMemNativeString checkMark;
	RETURN_IF_FAILED(checkMark.Initialize(HINST_THISCOMPONENT, isChecked ? IDS_X : IDS_UNDERSCORE)); // 50

	CoTaskMemNativeString checkBoxAndLabel;
	RETURN_IF_FAILED(checkBoxAndLabel.InitializeResMessage(HINST_THISCOMPONENT, IDS_COMBOBOXFORMAT, checkMark.Get(), boxLabel.GetRawBuffer(nullptr))); // 53

	UINT count = (UINT)checkBoxAndLabel.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 58

	UINT controlSize = m_IsVisible ? count / consoleWidth + 1 : 0;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(TRUE, controlSize, view)); // 64
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 70
		m_VisibleControlSize = controlSize;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(checkBoxAndLabel.Get(), count, m_HasFocus ? ColorScheme::Inverted : ColorScheme::Normal, consoleWidth, m_VisibleControlSize)); // 76
	}

	return S_OK;
}

HRESULT CheckboxControl::Toggle()
{
	bool isChecked;
	RETURN_IF_FAILED(m_dataSource->get_Checked(&isChecked)); // 85
	RETURN_IF_FAILED(m_dataSource->put_Checked(!isChecked)); // 86

	return S_OK;
}
