#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "credentialfieldcontrolbase.h"

class ComboboxControl
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, CredentialFieldControlBase
		, WF::Collections::VectorChangedEventHandler<HSTRING>
	>
{
public:
	ComboboxControl();
	~ComboboxControl() override;

	//ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource, IComboBoxSelectCallback* callback);

	//~ Begin WF::Collections::VectorChangedEventHandler<HSTRING> Interface
	HRESULT Invoke(WF::Collections::IObservableVector<HSTRING>* sender, WF::Collections::IVectorChangedEventArgs* args);
	//~ End WF::Collections::VectorChangedEventHandler<HSTRING> Interface
	
private:
	HRESULT v_Unadvise() override;
	HRESULT v_OnFocusChange(int hasFocus) override;
	HRESULT v_HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled);
	HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind) override;
	bool v_HasFocus() override;
	HRESULT Repaint(IConsoleUIView* view);
	
	Microsoft::WRL::ComPtr<LCPD::IComboBoxField> m_dataSource;
	Microsoft::WRL::ComPtr<WF::Collections::IObservableVector<HSTRING>> m_items;
	EventRegistrationToken m_itemsChangedToken;
	Microsoft::WRL::ComPtr<IComboBoxSelectCallback> m_callback;
	unsigned int m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
	bool m_HasFocus;
};
