#include "pch.h"

#include "userselectionview.h"

#include "basictextcontrol.h"
#include "selectableuserorcredentialcontrol.h"

using namespace Microsoft::WRL;

UserSelectionView::UserSelectionView()
{
}

UserSelectionView::~UserSelectionView()
{
}

HRESULT UserSelectionView::RuntimeClassInitialize(LCPD::ICredProvDataModel* dataSource)
{
	RETURN_IF_FAILED(Initialize()); // 21

	m_dataModel = dataSource;

	ComPtr<BasicTextControl> navigationMessage;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&navigationMessage, this, IDS_SELECTUSER)); // 27

	ComPtr<WFC::IObservableVector<IInspectable*>> usersAndV1;
	RETURN_IF_FAILED(dataSource->get_UsersAndV1Credentials(&usersAndV1)); // 30

	ComPtr<WFC::IVector<IInspectable*>> usersAndV1Vector;
	RETURN_IF_FAILED(usersAndV1.As(&usersAndV1Vector)); // 33

	UINT numUsers;
	RETURN_IF_FAILED(usersAndV1Vector->get_Size(&numUsers)); // 36

	for (UINT i = 0; i < numUsers; ++i)
	{
		ComPtr<IInspectable> userOrV1;
		RETURN_IF_FAILED(usersAndV1Vector->GetAt(i, &usersAndV1Vector)); // 41

		ComPtr<SelectableUserOrCredentialControl> control;
		RETURN_IF_FAILED(MakeAndInitialize<SelectableUserOrCredentialControl>(&control, this, userOrV1.Get())); // 44
	}

	return S_OK;
}

HRESULT UserSelectionView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	if (keyEvent->wVirtualKeyCode == VK_RETURN)
	{
		UINT userIndex = GetFocusIndex() - 1;

		ComPtr<WFC::IObservableVector<IInspectable*>> usersAndV1;
		RETURN_IF_FAILED(m_dataModel->get_UsersAndV1Credentials(&usersAndV1)); // 63

		ComPtr<WFC::IVector<IInspectable*>> usersAndV1Vector;
		RETURN_IF_FAILED(usersAndV1.As(&usersAndV1Vector)); // 66

		ComPtr<IInspectable> userOrV1;
		RETURN_IF_FAILED(usersAndV1Vector->GetAt(userIndex, &userOrV1)); // 69

		RETURN_IF_FAILED(m_dataModel->put_SelectedUserOrV1Credential(userOrV1.Get())); // 71
		*wasHandled = TRUE;
	}

	return S_OK;
}
