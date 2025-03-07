#include "statusview.h"
#include "controlhandle.h"
#include "SimpleArray.h"
#include <wil\resource.h>

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

StatusView::StatusView()
{
}

StatusView::~StatusView()
{
}

HRESULT StatusView::RuntimeClassInitialize(HSTRING status,
	Windows::Internal::UI::Logon::CredProvData::IUser* selectedUser)
{
	RETURN_IF_FAILED(this->Initialize()); // 19

	if (selectedUser)
	{
		Wrappers::HString userName;
		RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 24

		ComPtr<BasicTextControl> userNameControl;
		RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&userNameControl, this, userName.GetRawBuffer(0), false)); // 27
	}

	ComPtr<BasicTextControl> statusControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&statusControl, WindowsGetStringRawBuffer(status, nullptr), false)); // 31

	return S_OK;
}

HRESULT StatusView::v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;
	return S_OK;
}
