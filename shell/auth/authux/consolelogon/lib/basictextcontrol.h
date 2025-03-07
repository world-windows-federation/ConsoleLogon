#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "controlbase.h"

class BasicTextControl
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, ControlBase
		, IQueryFocus
	>
{
public:
	BasicTextControl();
	~BasicTextControl() override;

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, UINT stringResourceId);
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, const WCHAR* dataSource, bool acceptFocus);

	//~ Begin IQueryFocus
	STDMETHODIMP_(BOOL) HasFocus() override;
	//~ End IQueryFocus

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

	STDMETHODIMP Repaint(IConsoleUIView* view);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_dataSource;
	bool m_isInitialized;
	bool m_acceptFocus;
	bool m_hasFocus;
};
