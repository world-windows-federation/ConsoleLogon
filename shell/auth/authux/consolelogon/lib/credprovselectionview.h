#pragma once

#include "pch.h"

#include "consoleuiview.h"

class CredProvSelectionView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, ConsoleUIView>
{
public:
	CredProvSelectionView();
	~CredProvSelectionView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LCPD::ICredentialGroup* credentialGroup, HSTRING userName);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

private:
	Microsoft::WRL::ComPtr<LCPD::ICredentialGroup> m_credentialGroup;
};
