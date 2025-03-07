#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

MIDL_INTERFACE("af86e2e0-b12d-4c6a-9c5a-d7aa65101e90")
IComboBoxSelectCallback : IUnknown
{
	virtual HRESULT OnComboboxSelected(LCPD::IComboBoxField* comboBox) = 0;
};

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
