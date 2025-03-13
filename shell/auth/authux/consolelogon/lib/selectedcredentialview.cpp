#include "pch.h"

#include "selectedcredentialview.h"

#include "basictextcontrol.h"
#include "checkboxcontrol.h"
#include "comboboxcontrol.h"
#include "commandlinkcontrol.h"
#include "editcontrol.h"
#include "statictextcontrol.h"

using namespace Microsoft::WRL;

SelectedCredentialView::SelectedCredentialView()
{
}

SelectedCredentialView::~SelectedCredentialView()
{
}

HRESULT SelectedCredentialView::RuntimeClassInitialize(LC::LogonUIRequestReason reason, LCPD::ICredential* credential, HSTRING userName)
{
	RETURN_IF_FAILED(Initialize()); // 22

	m_credential = credential;
	CoTaskMemNativeString navigationString;
	RETURN_IF_FAILED(navigationString.InitializeResFormat(HINST_THISCOMPONENT, reason == LC::LogonUIRequestReason_LogonUIChange ? IDS_ENTERNEWCREDS : IDS_ENTERCREDS, WindowsGetStringRawBuffer(userName, nullptr))); // 27

	ComPtr<BasicTextControl> navigationMessage;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&navigationMessage, this, navigationString.Get(), false)); // 31

	ComPtr<WFC::IVectorView<LCPD::ICredentialField*>> fields;
	RETURN_IF_FAILED(credential->get_Fields(&fields)); // 34

	UINT numFields;
	RETURN_IF_FAILED(fields->get_Size(&numFields)); // 37

	int submitButtonIndex = -1;
	//@Mod, this adds support for newer versions
	LCPD::ICredentialField* submitButtonField;
	RETURN_IF_FAILED(credential->get_SubmitButton(&submitButtonField)); // 40
	if (submitButtonField)
	{
		bool isVisibleInDeselectedTile = false;
		RETURN_IF_FAILED(submitButtonField->get_IsVisibleInDeselectedTile(&isVisibleInDeselectedTile));

		if (!isVisibleInDeselectedTile)
		{
			ComPtr<LCPD::ICredentialSubmitButtonField> submitButtonField2;
			RETURN_IF_FAILED(submitButtonField->QueryInterface(IID_PPV_ARGS(&submitButtonField2)));

			RETURN_IF_FAILED(submitButtonField2->get_AdjacentID((UINT*)&submitButtonIndex)); // ye this really shouldn't be done, but they do treat the unsigned int as an int, so idgaf
		}
		else
		{
			//not sure if we need to handle anything here
		}

	}

	LOG_HR_MSG(E_FAIL,"Numfields %i\n",numFields);

	for (int i = 0; i < static_cast<int>(numFields); ++i)
	{
		ComPtr<LCPD::ICredentialField> dataSource;
		RETURN_IF_FAILED(fields->GetAt(i, &dataSource)); // 45

		LCPD::CredentialFieldKind fieldKind;
		RETURN_IF_FAILED(dataSource->get_Kind(&fieldKind)); // 48

		LOG_HR_MSG(E_FAIL,"fieldKind %i\n", (int)fieldKind);

		ComPtr<IQueryFocus> control;
		switch (fieldKind)
		{
			case LCPD::CredentialFieldKind_CheckBox:
			{
				RETURN_IF_FAILED(MakeAndInitialize<CheckboxControl>(&control, this, dataSource.Get())); // 55
				break;
			}
			case LCPD::CredentialFieldKind_ComboBox:
			{
				RETURN_IF_FAILED(MakeAndInitialize<ComboboxControl>(&control, this, dataSource.Get(), this)); // 60
				break;
			}
			case LCPD::CredentialFieldKind_CommandLink:
			{
				RETURN_IF_FAILED(MakeAndInitialize<CommandLinkControl>(&control, this, dataSource.Get())); // 65
				break;
			}
			case LCPD::CredentialFieldKind_EditText:
			{
				RETURN_IF_FAILED(MakeAndInitialize<EditControl>(&control, this, dataSource.Get())); // 70
				break;
			}
			case LCPD::CredentialFieldKind_StaticText:
			{
				RETURN_IF_FAILED(MakeAndInitialize<StaticTextControl>(&control, this, dataSource.Get())); // 75
				break;
			}
			case LCPD::CredentialFieldKind_SubmitButton:
			{
				CoTaskMemNativeString submitButtonText;
				RETURN_IF_FAILED(submitButtonText.Initialize(HINST_THISCOMPONENT, IDS_SIGNIN)); // 84
				RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&control, this, submitButtonText.Get(), true)); // 85
				m_controlWithSubmitButton = control;
				break;
			}
		}

		if (i == submitButtonIndex)
		{
			m_controlWithSubmitButton = control;
		}
	}

	return S_OK;
}

HRESULT SelectedCredentialView::OnComboboxSelected(LCPD::IComboBoxField* comboBox)
{
	RETURN_IF_FAILED(m_navigationCallback->ShowComboBox(comboBox)); // 104
	return S_OK;
}

HRESULT SelectedCredentialView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	switch (keyEvent->wVirtualKeyCode)
	{
		case VK_BACK:
		case VK_ESCAPE:
			RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 116
			*wasHandled = TRUE;
			break;
		case VK_RETURN:
			if (m_controlWithSubmitButton && m_controlWithSubmitButton->HasFocus())
			{
				RETURN_IF_FAILED(m_credential->Submit()); // 122
			}
			*wasHandled = TRUE;
			break;
	}

	return S_OK;
}
