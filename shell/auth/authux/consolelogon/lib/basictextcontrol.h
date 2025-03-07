#pragma once
#include <windows.h>
#include <Windows.Foundation.h>

#include "consoleuiview.h"
#include "NativeString.h"
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
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, PWCHAR dataSource, bool acceptFocus);

	//~ Begin IQueryFocus
	STDMETHODIMP_(BOOL) HasFocus() override;
	//~ End IQueryFocus

private:
	HRESULT v_OnFocusChange(BOOL) override PURE;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD*, BOOL*) override PURE;

	STDMETHODIMP Repaint(IConsoleUIView*);

	UINT m_VisibleControlSize;
	CoTaskMemNativeString m_dataSource;
	bool m_isInitialized;
	bool m_acceptFocus;
	bool m_hasFocus;
};
