#include "pch.h"

#include "editcontrol.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

EditControl::EditControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_IsVisible(false)
	, m_HasFocus(false)
	, m_EditStartIndex(0)
{
}

EditControl::~EditControl()
{
}

HRESULT EditControl::RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource)
{
	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 23

	RETURN_IF_FAILED(Advise(dataSource)); // 25
	auto scopeExit = wil::scope_exit([this]() -> void
	{
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 31

	RETURN_IF_FAILED(Repaint(view)); // 33

	scopeExit.release();
	return S_OK;
}

HRESULT EditControl::v_OnFocusChange(BOOL hasFocus)
{
	m_HasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 122

	return S_OK;
}

HRESULT EditControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;

	if (keyEvent->wVirtualKeyCode == VK_TAB
		|| keyEvent->wVirtualKeyCode == VK_RETURN
		|| keyEvent->wVirtualKeyCode == VK_ESCAPE
		|| (unsigned int)(keyEvent->wVirtualKeyCode - 0x25) <= 3) // this checks if the keys are arrow keys
	{
		return S_OK;
	}

	Wrappers::HString oldContent;
	RETURN_IF_FAILED(m_dataSource->get_Content(oldContent.ReleaseAndGetAddressOf())); // 150

	Wrappers::HString newContent;

	// backspace, so deletion of a character
	if (keyEvent->wVirtualKeyCode == VK_BACK)
	{
		UINT oldLength = 0;
		auto rawOldContent = oldContent.GetRawBuffer(&oldLength);
		RETURN_IF_FAILED(newContent.Set(rawOldContent, oldLength - 1)); // 158
	}
	else if (keyEvent->uChar.UnicodeChar)
	{
		WCHAR characterToAdd[] = {
			keyEvent->uChar.UnicodeChar,
			'\0'
		};
		RETURN_IF_FAILED(oldContent.Concat(Wrappers::HString::MakeReference(characterToAdd), newContent)); // 166
	}
	else
	{
		return S_OK;
	}
	RETURN_IF_FAILED(m_dataSource->put_Content(newContent.Get())); // 172
	*wasHandled = true;

	return S_OK;
}

HRESULT EditControl::v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind)
{
	if (changeKind == LCPD::CredentialFieldChangeKind_State)
	{
		bool isVisible = false;
		RETURN_IF_FAILED(GetVisibility(&isVisible)); // 186

		if (isVisible != m_IsVisible)
		{
			m_IsVisible = isVisible;
			RETURN_IF_FAILED(Repaint(m_view.Get())); // 190
		}
	}
	else if (changeKind == LCPD::CredentialFieldChangeKind_SetString)
	{
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 194
	}

	return S_OK;
}

//compiled version has it optimised so it shares the same function as CheckboxControl
bool EditControl::v_HasFocus()
{
	return m_HasFocus;
}

HRESULT EditControl::Repaint(IConsoleUIView* view)
{
	Wrappers::HString content;
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 43

	UINT contentLength;
	const WCHAR* rawContent = content.GetRawBuffer(&contentLength);

	ComPtr<LCPD::ICredentialField> fieldInfo;
	RETURN_IF_FAILED(m_dataSource.As(&fieldInfo)); // 49

	Wrappers::HString label;
	RETURN_IF_FAILED(fieldInfo->get_Label(label.ReleaseAndGetAddressOf())); // 52

	BOOLEAN isPasswordField;
	RETURN_IF_FAILED(m_dataSource->get_IsPasswordField(&isPasswordField)); // 55

	int maxPasswordLength;
	RETURN_IF_FAILED(m_dataSource->get_MaxPasswordLength(&maxPasswordLength)); // 58

	CoTaskMemNativeString labelAndContent;
	RETURN_IF_FAILED(labelAndContent.InitializeFormat(L"%s : ", label.GetRawBuffer(nullptr))); // 62

	if (contentLength)
	{
		if (isPasswordField)
		{
			for (UINT i = 0; i < contentLength; i++)
			{
				RETURN_IF_FAILED(labelAndContent.Concat(L'*')); // 71
			}
		}
		else
		{
			RETURN_IF_FAILED(labelAndContent.Concat(rawContent)); // 76
		}
	}

	RETURN_IF_FAILED(labelAndContent.Concat(L' ')); // 81

	UINT length = (UINT)labelAndContent.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 86

	UINT controlSize = m_IsVisible ? length / consoleWidth + 1 : 0;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(1, controlSize, view)); // 92
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 98
		m_VisibleControlSize = controlSize;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(labelAndContent.Get(), length, ColorScheme::Normal, consoleWidth, m_VisibleControlSize)); // 104

		if (m_HasFocus)
		{
			COORD cursorPosition = {
				(SHORT)((length - 1) % consoleWidth),
				(SHORT)((length - 1) / consoleWidth)
			};
			RETURN_IF_FAILED(m_view->SetCursorPos(m_outputHandle.Get(), cursorPosition, true)); // 112
		}
	}

	return S_OK;
}
