#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "controlbase.h"
#include "InternalAsync.h"
#include "logoninterfaces.h"

class SerializationFailedView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	SerializationFailedView();
	~SerializationFailedView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(HSTRING caption, HSTRING message, LCPD::IUser* selectedUser);

protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
