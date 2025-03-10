#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

enum class MessageOptionFlag
{
	None = 0x0,
	Ok = 0x1,
	Cancel = 0x2,
	Yes = 0x4,
	No = 0x8,
};

DEFINE_ENUM_FLAG_OPERATORS(MessageOptionFlag);

class MessageOptionControl final : public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	MessageOptionControl();
	~MessageOptionControl() override;

	HRESULT RuntimeClassInitialize(
		IConsoleUIView* view, MessageOptionFlag option,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

	HRESULT Repaint(IConsoleUIView* view);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_label;
	bool m_isInitialized;
	bool m_hasFocus;
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>> m_completion;
	MessageOptionFlag m_option;
};
