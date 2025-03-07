#pragma once

#include "pch.h"

#include "consoleuiview.h"

class SerializationFailedView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	SerializationFailedView();
	~SerializationFailedView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(HSTRING caption, HSTRING message, LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
