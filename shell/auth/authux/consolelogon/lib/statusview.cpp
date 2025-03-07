#include "pch.h"

#include "statusview.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

StatusView::StatusView()
{
}

StatusView::~StatusView()
{
}

HRESULT StatusView::RuntimeClassInitialize(HSTRING status, LCPD::IUser* selectedUser)
{
	RETURN_IF_FAILED(Initialize()); // 19

	if (selectedUser)
	{
		Wrappers::HString userName;
		RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 24

		ComPtr<BasicTextControl> userNameControl;
		RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&userNameControl, this, userName.GetRawBuffer(nullptr), false)); // 27
	}

	ComPtr<BasicTextControl> statusControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&statusControl, this, WindowsGetStringRawBuffer(status, nullptr), false)); // 31

	return S_OK;
}

HRESULT StatusView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = false;
	return S_OK;
}
