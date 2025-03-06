#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "logoninterfaces.h"

class LockedView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	LockedView();
	~LockedView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();
	
protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
