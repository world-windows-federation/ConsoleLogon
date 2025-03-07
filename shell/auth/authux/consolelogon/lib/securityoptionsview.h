#pragma once

#include "pch.h"

#include "consoleuiview.h"

class SecurityOptionsView final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	SecurityOptionsView();
	~SecurityOptionsView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(
		LC::LogonUISecurityOptions options,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

private:
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>> m_completion;
};
