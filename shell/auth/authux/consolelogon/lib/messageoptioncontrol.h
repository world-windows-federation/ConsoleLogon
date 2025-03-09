#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

enum MessageOptionFlag
{
	None=0,
	Ok=1,
	Cancel=2,
	Yes=4,
	No=8
};

class MessageOptionControl : public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	MessageOptionControl();
	~MessageOptionControl() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, MessageOptionFlag option, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);

private:
	HRESULT v_OnFocusChange(int hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT Repaint(IConsoleUIView* view);
	
	unsigned int m_VisibleControlSize;
	CoTaskMemNativeString m_label;
	bool m_isInitialized;
	bool m_hasFocus;
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>> m_completion;
	MessageOptionFlag m_option;
};
