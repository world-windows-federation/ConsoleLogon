#pragma once

#include "pch.h"

#include "consoleuiview.h"

class LockedView final
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	LockedView();
	~LockedView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
};
