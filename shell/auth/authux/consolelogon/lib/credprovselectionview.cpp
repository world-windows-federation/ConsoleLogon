#include "pch.h"

#include "credprovselectionview.h"

#include "basictextcontrol.h"
#include "selectableuserorcredentialcontrol.h"

using namespace Microsoft::WRL;

CredProvSelectionView::CredProvSelectionView()
{
}

CredProvSelectionView::~CredProvSelectionView()
{
}

HRESULT CredProvSelectionView::RuntimeClassInitialize(LCPD::ICredentialGroup* credentialGroup, HSTRING userName)
{
	RETURN_IF_FAILED(Initialize()); // 22

	m_credentialGroup = credentialGroup;

	CoTaskMemNativeString navigationString;
	RETURN_IF_FAILED(navigationString.InitializeResFormat(HINST_THISCOMPONENT, IDS_SELECTSIGNINOPT, WindowsGetStringRawBuffer(userName, nullptr))); // 27

	ComPtr<BasicTextControl> navigationMessage;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&navigationMessage, this, navigationString.Get(), false)); // 31

	ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
	RETURN_IF_FAILED(credentialGroup->get_Credentials(&credentials)); // 34

	ComPtr<WFC::IVector<LCPD::Credential*>> credentialVector;
	RETURN_IF_FAILED(credentials.As(&credentialVector)); // 37

	UINT numCredentials = 0;
	RETURN_IF_FAILED(credentialVector->get_Size(&numCredentials)); // 40

	for (UINT i = 0; i < numCredentials; i++)
	{
		ComPtr<LCPD::ICredential> dataSource;
		RETURN_IF_FAILED(credentialVector->GetAt(i, &dataSource)); // 45

		ComPtr<SelectableUserOrCredentialControl> control;
		RETURN_IF_FAILED(Details::MakeAndInitialize<SelectableUserOrCredentialControl>(&control, this, dataSource.Get())); // 48
	}

	return S_OK;
}

HRESULT CredProvSelectionView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	switch (keyEvent->wVirtualKeyCode)
	{
		case VK_ESCAPE:
		case VK_BACK:
		{
			RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 62

			*wasHandled = TRUE;
			break;
		}

		case VK_RETURN:
		{
			ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
			RETURN_IF_FAILED(m_credentialGroup->get_Credentials(&credentials)); // 72

			ComPtr<WFC::IVector<LCPD::Credential*>> credentialVector;
			RETURN_IF_FAILED(credentials.As(&credentialVector)); // 75

			ComPtr<LCPD::ICredential> credential;
			RETURN_IF_FAILED(credentialVector->GetAt(GetFocusIndex() - 1, &credential)); // 78

			RETURN_IF_FAILED(m_credentialGroup->put_SelectedCredential(credential.Get())); // 80

			*wasHandled = TRUE;
			break;
		}
	}

	return S_OK;
}
