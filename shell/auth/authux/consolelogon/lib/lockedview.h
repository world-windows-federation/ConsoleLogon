#pragma once

#include "pch.h"

#include "consoleuiview.h"

class LockedView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	LockedView();
	~LockedView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
