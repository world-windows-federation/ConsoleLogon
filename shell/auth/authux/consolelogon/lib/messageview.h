#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "InternalAsync.h"
#include "logoninterfaces.h"

class MessageView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	MessageView();
	~MessageView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(
		HSTRING* caption, HSTRING* message, UINT messageBoxFlags,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion,
		LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
