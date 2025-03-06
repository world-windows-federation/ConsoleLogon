#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "InternalAsync.h"
#include "logoninterfaces.h"

class SecurityOptionsView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	SecurityOptionsView();
	~SecurityOptionsView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LC::LogonUISecurityOptions options, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);

protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	
private:
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>> m_completion;
};
