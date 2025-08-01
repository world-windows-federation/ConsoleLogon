﻿#pragma once

#include "pch.h"

#include "consoleuiview.h"

class CredProvSelectionView final
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	CredProvSelectionView();
	~CredProvSelectionView() override;

	HRESULT RuntimeClassInitialize(LCPD::ICredentialGroup* credentialGroup, HSTRING userName, bool userSwitchingAllowed);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

private:
	Microsoft::WRL::ComPtr<LCPD::ICredentialGroup> m_credentialGroup;
};
