#pragma once

#include "pch.h"

#include "controlbase.h"

class SerializationFailedOptionControl final : public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	SerializationFailedOptionControl();

	HRESULT RuntimeClassInitialize(IConsoleUIView* view);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

	HRESULT Repaint(IConsoleUIView* view);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_label;
	bool m_isInitialized;
	bool m_hasFocus;
};
