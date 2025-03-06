#pragma once
#include <windows.h>
#include <Windows.Foundation.h>

#include "consoleuiview.h"
#include "NativeString.h"
#include "controlbase.h"

class BasicTextControl
	: public  Microsoft::WRL::RuntimeClass < Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
	, ControlBase
	, IQueryFocus
	>
{
public:
	BasicTextControl();
	~BasicTextControl();

	STDMETHODIMP RuntimeClassInitialize(IConsoleUIView* view, PWCHAR dataSource, bool acceptFocus);
	STDMETHODIMP RuntimeClassInitialize(IConsoleUIView* view, UINT stringResourceId);

	//~ Begin IQueryFocus
	STDMETHODIMP_(BOOL) HasFocus();
	//~ End IQueryFocus

private:
	virtual HRESULT v_OnFocusChange(BOOL) PURE;
	virtual HRESULT v_HandleKeyInput(PKEY_EVENT_RECORD, PBOOL) PURE;

	STDMETHODIMP Repaint(IConsoleUIView*);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_dataSource;
	bool m_isInitialized;
	bool m_acceptFocus;
	bool m_hasFocus;
};

