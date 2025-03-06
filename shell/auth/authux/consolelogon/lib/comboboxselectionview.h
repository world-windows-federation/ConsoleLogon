#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "logoninterfaces.h"

class ComboboxSelectionView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, ConsoleUIView
		, WF::Collections::VectorChangedEventHandler<HSTRING>
	>
{
public:
	ComboboxSelectionView();
	~ComboboxSelectionView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LCPD::IComboBoxField* dataSource);

	//~ Begin WF::Collections::VectorChangedEventHandler<HSTRING> Interface
	STDMETHODIMP Invoke(WF::Collections::IObservableVector<HSTRING>* sender, WF::Collections::IVectorChangedEventArgs* args) override;
	//~ End WF::Collections::VectorChangedEventHandler<HSTRING> Interface
	
protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	void v_Unadvise() override;
	HRESULT Reset();
	
private:
	Microsoft::WRL::ComPtr<LCPD::IComboBoxField> m_dataSource;
	Microsoft::WRL::ComPtr<WF::Collections::IObservableVector<HSTRING> > m_items;
	EventRegistrationToken m_itemsChangedToken;
};
