#pragma once

#include "pch.h"

#include "consoleuiview.h"

class MessageView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	MessageView();
	~MessageView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(
		HSTRING caption, HSTRING message, UINT messageBoxFlags,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion,
		LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
