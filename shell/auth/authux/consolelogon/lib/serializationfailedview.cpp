#include "pch.h"

#include "serializationfailedview.h"

#include "basictextcontrol.h"
#include "serializationfailedoptioncontrol.h"

using namespace Microsoft::WRL;

SerializationFailedView::SerializationFailedView()
{
}

SerializationFailedView::~SerializationFailedView()
{
}

HRESULT SerializationFailedView::RuntimeClassInitialize(HSTRING caption, HSTRING message, LCPD::IUser* selectedUser)
{
	RETURN_IF_FAILED(Initialize()); // 18

	if (selectedUser)
	{
		Wrappers::HString userName;
		RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 23

		ComPtr<BasicTextControl> userNameControl;
		RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&userNameControl, this, userName.GetRawBuffer(nullptr), false)); // 26
	}

	ComPtr<BasicTextControl> captionControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&captionControl, this, WindowsGetStringRawBuffer(caption, nullptr), false)); // 30

	ComPtr<BasicTextControl> messageControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&messageControl, this, WindowsGetStringRawBuffer(message, nullptr), false)); // 33

	ComPtr<SerializationFailedOptionControl> optionControl;
	RETURN_IF_FAILED(MakeAndInitialize<SerializationFailedOptionControl>(&optionControl, this)); // 36

	return S_OK;
}

HRESULT SerializationFailedView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	if (keyEvent->wVirtualKeyCode == VK_RETURN)
	{
		m_navigationCallback->OnNavigation();
		*wasHandled = TRUE;
	}

	return S_OK;
}
