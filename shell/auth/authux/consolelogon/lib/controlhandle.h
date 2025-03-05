#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include <consoleuiview.h>

// f4c33f16-f5a6-4c7e-afe7-29c3df6ad04b
MIDL_INTERFACE("f4c33f16-f5a6-4c7e-afe7-29c3df6ad04b")
IControlHandle : public IUnknown
{
	virtual UINT STDMETHODCALLTYPE GetOffsetFromRoot() PURE;
	virtual HRESULT STDMETHODCALLTYPE UpdateOffsetFromRoot(UINT) PURE;
	virtual UINT STDMETHODCALLTYPE GetSize() PURE;
	virtual HRESULT STDMETHODCALLTYPE SetSize(UINT) PURE;
	virtual INT STDMETHODCALLTYPE IsFocusable() PURE;
	virtual HRESULT STDMETHODCALLTYPE SetFocus(int) PURE;
	virtual UINT STDMETHODCALLTYPE GetIndexInTable() PURE;
	virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(PKEY_EVENT_RECORD, int*) PURE;
	virtual HRESULT STDMETHODCALLTYPE Destroy() PURE;
};


class ControlHandle
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
	, IControlHandle
	>
{
public:
	ControlHandle();
	~ControlHandle();

	//~ Begin IControlHandle Interface
	STDMETHODIMP RuntimeClassInitialize(UINT, UINT, UINT, IConsoleUIControl*);
	STDMETHODIMP_(UINT) GetOffsetFromRoot();
	STDMETHODIMP UpdateOffsetFromRoot(UINT);
	STDMETHODIMP_(UINT) GetSize();
	STDMETHODIMP SetSize(UINT);
	STDMETHODIMP_(INT) IsFocusable();
	STDMETHODIMP SetFocus(INT);
	STDMETHODIMP_(UINT) GetIndexInTable();
	STDMETHODIMP HandleKeyInput(PKEY_EVENT_RECORD, PINT);
	STDMETHODIMP Destroy();
	//~ End IControlHandle Interface

private:
	UINT m_offsetFromRoot;
	UINT m_size;
	UINT m_index;
	Microsoft::WRL::ComPtr<IConsoleUIControl> m_controlCallback;
};
