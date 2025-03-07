#pragma once

#include "pch.h"

#include "consoleuiview.h"

class ComboboxSelectionView final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, ConsoleUIView
		, WFC::VectorChangedEventHandler<HSTRING>
	>
{
public:
	ComboboxSelectionView();
	~ComboboxSelectionView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LCPD::IComboBoxField* dataSource);

	//~ Begin WFC::VectorChangedEventHandler<HSTRING> Interface
	STDMETHODIMP Invoke(WFC::IObservableVector<HSTRING>* sender, WFC::IVectorChangedEventArgs* args) override;
	//~ End WFC::VectorChangedEventHandler<HSTRING> Interface

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	void v_Unadvise() override;

	HRESULT Reset();

private:
	Microsoft::WRL::ComPtr<LCPD::IComboBoxField> m_dataSource;
	Microsoft::WRL::ComPtr<WFC::IObservableVector<HSTRING>> m_items;
	EventRegistrationToken m_itemsChangedToken;
};
