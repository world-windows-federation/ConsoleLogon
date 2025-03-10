#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "credentialfieldcontrolbase.h"

class ComboboxControl final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, CredentialFieldControlBase
		, WFC::VectorChangedEventHandler<HSTRING>
	>
{
public:
	ComboboxControl();
	~ComboboxControl() override;

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource, IComboBoxSelectCallback* callback);

	//~ Begin WFC::VectorChangedEventHandler<HSTRING> Interface
	STDMETHODIMP Invoke(WFC::IObservableVector<HSTRING>* sender, WFC::IVectorChangedEventArgs* args) override;
	//~ End WFC::VectorChangedEventHandler<HSTRING> Interface

private:
	HRESULT v_Unadvise() override;
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind) override;
	bool v_HasFocus() override;

	HRESULT Repaint(IConsoleUIView* view);

	Microsoft::WRL::ComPtr<LCPD::IComboBoxField> m_dataSource;
	Microsoft::WRL::ComPtr<WFC::IObservableVector<HSTRING>> m_items;
	EventRegistrationToken m_itemsChangedToken;
	Microsoft::WRL::ComPtr<IComboBoxSelectCallback> m_callback;
	UINT m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
	bool m_HasFocus;
};
