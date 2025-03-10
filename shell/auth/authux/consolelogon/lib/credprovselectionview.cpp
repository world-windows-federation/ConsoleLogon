#include "pch.h"

#include "credprovselectionview.h"

#include "basictextcontrol.h"
#include "messageoptioncontrol.h"
#include "selectableuserorcredentialcontrol.h"

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
	RETURN_IF_FAILED(navigationString.InitializeResFormat(HINST_THISCOMPONENT, IDS_SELECTSIGNINOPT, WindowsGetStringRawBuffer(userName,nullptr))); // 27

	Microsoft::WRL::ComPtr<BasicTextControl> navigationMessage;
	RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<BasicTextControl>(&navigationMessage,this,navigationString.Get(),false)); // 31

	Microsoft::WRL::ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
	RETURN_IF_FAILED(credentialGroup->get_Credentials(&credentials)); // 34

	Microsoft::WRL::ComPtr<WFC::IVector<LCPD::Credential*>> credentialVector;
	RETURN_IF_FAILED(credentials.As(&credentialVector)); // 37

	UINT numCredentials = 0;
	RETURN_IF_FAILED(credentialVector->get_Size(&numCredentials)); // 40

	/*if (numCredentials > 0)
	{
		for (unsigned int i = 0; i < numCredentials; i++)
		{
			Microsoft::WRL::ComPtr<LCPD::Credential> dataSource;
			RETURN_IF_FAILED(credentialVector->GetAt(i,&dataSource)); // 45

			Microsoft::WRL::ComPtr<SelectableUserOrCredentialControl> control;
			RETURN_IF_FAILED(Microsoft::WRL::Details::MakeAndInitialize<SelectableUserOrCredentialControl>(&control,this,dataSource.Get())); // 48
		}
	}*/
	
	//@MOD, REMOVED > 0 CHECK, REDUNDANT, THO IDK IF SOURCE ORIGINALLY HAD IT OR IF ITS A COMPILATION/DECOMPILATION QUIRK
	for (unsigned int i = 0; i < numCredentials; i++)
	{
		Microsoft::WRL::ComPtr<LCPD::ICredential> dataSource;
		RETURN_IF_FAILED(credentialVector->GetAt(i,&dataSource)); // 45

		Microsoft::WRL::ComPtr<SelectableUserOrCredentialControl> control;
		RETURN_IF_FAILED(Microsoft::WRL::Details::MakeAndInitialize<SelectableUserOrCredentialControl>(&control,this,dataSource.Get())); // 48
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
			RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 62
			*wasHandled = TRUE;
			break;

		case VK_RETURN:
			Microsoft::WRL::ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
			RETURN_IF_FAILED(m_credentialGroup->get_Credentials(&credentials)); // 72

			Microsoft::WRL::ComPtr<WFC::IVector<LCPD::Credential*>> credentialVector;
			RETURN_IF_FAILED(credentials.As(&credentialVector)); // 75

			Microsoft::WRL::ComPtr<LCPD::ICredential> credential;
			RETURN_IF_FAILED(credentialVector->GetAt(GetFocusIndex() - 1,&credential)); // 78

			RETURN_IF_FAILED(m_credentialGroup->put_SelectedCredential(credential.Get())); // 80
			*wasHandled = TRUE;
			return S_OK;
	}

	return S_OK;
}
