#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "logoninterfaces.h"

class CredProvSelectionView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	CredProvSelectionView();
	~CredProvSelectionView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LCPD::ICredentialGroup* credentialGroup, HSTRING* userName);
	
protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	
private:
	Microsoft::WRL::ComPtr<LCPD::ICredentialGroup> m_credentialGroup;
};
