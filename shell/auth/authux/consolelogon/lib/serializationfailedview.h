#pragma once

#include "pch.h"

#include "consoleuiview.h"

class SerializationFailedView final
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	SerializationFailedView();
	~SerializationFailedView() override;

	HRESULT RuntimeClassInitialize(HSTRING caption, HSTRING message, LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
