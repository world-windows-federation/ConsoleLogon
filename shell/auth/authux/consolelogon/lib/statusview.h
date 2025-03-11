#pragma once

#include "pch.h"

#include "consoleuiview.h"

class StatusView final
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	StatusView();
	~StatusView() override;

	HRESULT RuntimeClassInitialize(HSTRING status, LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
