#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

class SelectedCredentialView final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, ConsoleUIView
		, IComboBoxSelectCallback
	>
{
public:
	SelectedCredentialView();
	~SelectedCredentialView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LC::LogonUIRequestReason reason, LCPD::ICredential* credential, HSTRING userName);

	//~ Begin IComboBoxSelectCallback Interface
	STDMETHODIMP OnComboboxSelected(LCPD::IComboBoxField* comboBox) override;
	//~ End IComboBoxSelectCallback Interface

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

private:
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredential> m_credential;
	Microsoft::WRL::ComPtr<IQueryFocus> m_controlWithSubmitButton;
};
