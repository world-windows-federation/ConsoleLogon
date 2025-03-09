#include "pch.h"

#include "editcontrol.h"

#include "basictextcontrol.h"

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
	//if (m_dataSource.Get())
	//{
	//	m_dataSource.Reset();
	//}

	RETURN_IF_FAILED(dataSource->QueryInterface(IID_PPV_ARGS(&m_dataSource))); // 23

	RETURN_IF_FAILED(Advise(dataSource)); // 25

	auto scopeExit = wil::scope_exit([this]() -> void {
		ControlBase::Unadvise();
	});

	RETURN_IF_FAILED(GetVisibility(&m_IsVisible)); // 31

	RETURN_IF_FAILED(Repaint(view)); // 33

	scopeExit.release();
	return S_OK;
}

HRESULT EditControl::v_OnFocusChange(int hasFocus)
{
	m_HasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 122

	return S_OK;
}

HRESULT EditControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;

	if ( keyEvent->wVirtualKeyCode == VK_TAB
	  || keyEvent->wVirtualKeyCode == VK_RETURN
	  || keyEvent->wVirtualKeyCode == VK_ESCAPE
	  || (unsigned int)(keyEvent->wVirtualKeyCode - 0x25) <= 3 ) // this checks if the keys are arrow keys
	{
		return S_OK;
	}

	Microsoft::WRL::Wrappers::HString oldContent;
	RETURN_IF_FAILED(m_dataSource->get_Content(oldContent.ReleaseAndGetAddressOf())); // 150

	Microsoft::WRL::Wrappers::HString newContent;
	if (keyEvent->wVirtualKeyCode == VK_BACK) // backspace, so deletion of a character
	{
		//i believe this is what they did, but someone should double check and verify it
		unsigned int oldLength = 0;
		RETURN_IF_FAILED(newContent.Set(oldContent.GetRawBuffer(&oldLength), oldLength - 1)); // 158
		
		RETURN_IF_FAILED(m_dataSource->put_Content(newContent.Get())); // 172

		*wasHandled = true;
	}
	else if ( keyEvent->uChar.UnicodeChar )
	{
		WCHAR characterToAdd[] = { keyEvent->uChar.UnicodeChar, '\0' };
		RETURN_IF_FAILED(oldContent.Concat(Microsoft::WRL::Wrappers::HString::MakeReference(characterToAdd),newContent)); // 166

		//note, this is a copy and paste of a goto label which would go to the one above, they prob had a duplicate in code which got optimized to this
		RETURN_IF_FAILED(m_dataSource->put_Content(newContent.Get())); // 172

		*wasHandled = true;
	}

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

//TODO: have someone else other than me verify this code
HRESULT EditControl::Repaint(IConsoleUIView* view)
{
	Microsoft::WRL::Wrappers::HString content;
	RETURN_IF_FAILED(m_dataSource->get_Content(content.ReleaseAndGetAddressOf())); // 43

	//strange way to get the length, but okay
	//note can't inline this
	unsigned int contentLength = 0;
	PCWSTR StringRawBuffer = content.GetRawBuffer(&contentLength);
	
	Microsoft::WRL::ComPtr<LCPD::ICredentialField> fieldInfo;
	RETURN_IF_FAILED(m_dataSource->QueryInterface(IID_PPV_ARGS(&fieldInfo))); // 49

	Microsoft::WRL::Wrappers::HString label;
	RETURN_IF_FAILED(fieldInfo->get_Label(label.ReleaseAndGetAddressOf())); // 52

	bool isPasswordField = false;
	RETURN_IF_FAILED(m_dataSource->get_IsPasswordField(&isPasswordField)); // 55

	int maxPasswordLength = 0;
	RETURN_IF_FAILED(m_dataSource->get_MaxPasswordLength(&maxPasswordLength)); // 58

	CoTaskMemNativeString labelAndContent;
	RETURN_IF_FAILED(labelAndContent.InitializeFormat(L"%s : ",label.GetRawBuffer(0))); // 62

	if (contentLength > 0)
	{
		if (isPasswordField)
		{
			for (unsigned int i = 0; i < contentLength; i++)
			{
				RETURN_IF_FAILED(labelAndContent.Concat(L'*')); // 71
			}
		}
		else
		{
			RETURN_IF_FAILED(labelAndContent.Concat(StringRawBuffer)); // 76
		}
	}
	RETURN_IF_FAILED(labelAndContent.Concat(L' ')); // 81

	unsigned int count = (unsigned int)labelAndContent.GetCount();

	unsigned int consoleWidth = 0;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 86

	unsigned int consoleheight = 0;
	if (m_IsVisible)
		consoleheight = count / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(true,consoleheight,view)); // 92
		m_isInitialized = true;
	}

	if (consoleheight != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(),consoleheight)); // 98
		m_VisibleControlSize = consoleheight;
	}

	if (m_IsVisible)
	{
		RETURN_IF_FAILED(PaintArea(labelAndContent.Get(),count,ColorScheme::Normal,consoleWidth,m_VisibleControlSize)); // 104

		if (m_HasFocus)
		{
			COORD cursorPosition;
			cursorPosition.Y = (SHORT)((count - 1) / consoleWidth);
			cursorPosition.X = (SHORT)((count - 1) % consoleWidth);
			RETURN_IF_FAILED(m_view->SetCursorPos(m_outputHandle.Get(),cursorPosition,true)); // 112
		}
	}

	return S_OK;
}
