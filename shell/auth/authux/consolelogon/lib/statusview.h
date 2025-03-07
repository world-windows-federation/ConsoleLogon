#pragma once

#include "pch.h"

#include "consoleuiview.h"

class StatusView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	StatusView();
	~StatusView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(HSTRING status, LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
