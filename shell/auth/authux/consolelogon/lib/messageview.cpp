#include "pch.h"

#include "messageview.h"

#include "basictextcontrol.h"
#include "messageoptioncontrol.h"

MessageView::MessageView()
{
}

MessageView::~MessageView()
{
}

HRESULT MessageView::RuntimeClassInitialize(HSTRING caption, HSTRING message, UINT messageBoxFlags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion, LCPD::IUser* selectedUser)
{
	RETURN_IF_FAILED(ConsoleUIView::Initialize()); // 22
	
	if (selectedUser)
	{
		Microsoft::WRL::Wrappers::HString userName;
		RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 27

		Microsoft::WRL::ComPtr<BasicTextControl> userNameControl;
		RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<BasicTextControl>(&userNameControl, this, userName.GetRawBuffer(nullptr), false)); // 30
	}

	Microsoft::WRL::ComPtr<BasicTextControl> captionControl;
	RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<BasicTextControl>(&captionControl, this, WindowsGetStringRawBuffer(caption, nullptr), false)); // 34

	Microsoft::WRL::ComPtr<BasicTextControl> messageControl;
	RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<BasicTextControl>(&messageControl, this, WindowsGetStringRawBuffer(message, nullptr), false)); // 37

	char translatedFlag;
	switch (messageBoxFlags & 0xF)
	{
	case 1:
		translatedFlag = 3; // Ok and Cancel
		break;
	case 3:
		translatedFlag = 14; // Ok and Cancel and Yes and No
		break;
	case 4:
		translatedFlag = 12; // Yes and No
		break;
	case 6:
		translatedFlag = 2; // Cancel
		break;
	default:
		translatedFlag = 1; // Ok
	}
	
	if ((translatedFlag & 1) != 0) //Ok
	{
		Microsoft::WRL::ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<MessageOptionControl>(&optionControl,this,MessageOptionFlag::Ok,completion)); // 63
	}
	
	if ( (translatedFlag & 2) != 0 ) //Cancel
	{
		Microsoft::WRL::ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<MessageOptionControl>(&optionControl,this,MessageOptionFlag::Cancel,completion)); // 69
	}

	if ( (translatedFlag & 4) != 0 ) //Yes
	{
		Microsoft::WRL::ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<MessageOptionControl>(&optionControl,this,MessageOptionFlag::Yes,completion)); // 75
	}

	if ( (translatedFlag & 8) != 0 ) //No
	{
		Microsoft::WRL::ComPtr<MessageOptionControl> optionControl;
		RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<MessageOptionControl>(&optionControl,this,MessageOptionFlag::No,completion)); // 81
	}

	return S_OK;
}

HRESULT MessageView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return S_OK;
}
