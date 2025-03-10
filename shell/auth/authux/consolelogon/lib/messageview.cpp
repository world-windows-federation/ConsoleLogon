#include "pch.h"

#include "messageview.h"

#include "basictextcontrol.h"
#include "messageoptioncontrol.h"

using namespace Microsoft::WRL;

MessageView::MessageView()
{
}

MessageView::~MessageView()
{
}

HRESULT MessageView::RuntimeClassInitialize(
	HSTRING caption, HSTRING message, UINT messageBoxFlags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion, LCPD::IUser* selectedUser)
{
	RETURN_IF_FAILED(ConsoleUIView::Initialize()); // 22

	if (selectedUser)
	{
		Wrappers::HString userName;
		RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 27

		ComPtr<BasicTextControl> userNameControl;
		RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&userNameControl, this, userName.GetRawBuffer(nullptr), false)); // 30
	}

	ComPtr<BasicTextControl> captionControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&captionControl, this, WindowsGetStringRawBuffer(caption, nullptr), false)); // 34

	ComPtr<BasicTextControl> messageControl;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&messageControl, this, WindowsGetStringRawBuffer(message, nullptr), false)); // 37

	MessageOptionFlag optionFlags;
	switch (messageBoxFlags & 0xF)
	{
		case 1:
			optionFlags = MessageOptionFlag::Ok | MessageOptionFlag::Cancel;
			break;

		case 3:
			optionFlags = MessageOptionFlag::Cancel | MessageOptionFlag::Yes | MessageOptionFlag::No;
			break;

		case 4:
			optionFlags = MessageOptionFlag::Yes | MessageOptionFlag::No;
			break;

		case 6:
			optionFlags = MessageOptionFlag::Cancel;
			break;

		default:
			optionFlags = MessageOptionFlag::Ok;
			break;
	}

	if ((optionFlags & MessageOptionFlag::Ok) != MessageOptionFlag::None)
	{
		ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<MessageOptionControl>(&optionControl, this, MessageOptionFlag::Ok, completion)); // 63
	}

	if ((optionFlags & MessageOptionFlag::Cancel) != MessageOptionFlag::None)
	{
		ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<MessageOptionControl>(&optionControl, this, MessageOptionFlag::Cancel, completion)); // 69
	}

	if ((optionFlags & MessageOptionFlag::Yes) != MessageOptionFlag::None)
	{
		ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<MessageOptionControl>(&optionControl, this, MessageOptionFlag::Yes, completion)); // 75
	}

	if ((optionFlags & MessageOptionFlag::No) != MessageOptionFlag::None)
	{
		ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(MakeAndInitialize<MessageOptionControl>(&optionControl, this, MessageOptionFlag::No, completion)); // 81
	}

	return S_OK;
}

HRESULT MessageView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return S_OK;
}
