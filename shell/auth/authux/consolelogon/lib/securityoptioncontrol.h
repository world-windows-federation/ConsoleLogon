#pragma once

#include "pch.h"

#include "controlbase.h"

class SecurityOptionControl
	: public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	SecurityOptionControl();
	~SecurityOptionControl() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LC::LogonUISecurityOptions option, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);

private:
	HRESULT v_OnFocusChange(int hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT Repaint(IConsoleUIView* view);
	
	unsigned int m_VisibleControlSize;
	CoTaskMemNativeString m_label;
	bool m_isInitialized;
	bool m_hasFocus;
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>> m_completion;
	LC::LogonUISecurityOptions m_option;
};
