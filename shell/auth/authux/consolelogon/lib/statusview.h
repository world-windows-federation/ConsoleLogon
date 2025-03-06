#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "logoninterfaces.h"

class StatusView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	StatusView();
	~StatusView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(HSTRING status, LCPD::IUser* selectedUser);
	
protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
