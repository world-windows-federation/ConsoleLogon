#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include <consoleuiview.h>

MIDL_INTERFACE("f4c33f16-f5a6-4c7e-afe7-29c3df6ad04b")
IControlHandle : IUnknown
{
	virtual UINT STDMETHODCALLTYPE GetOffsetFromRoot() PURE;
	virtual HRESULT STDMETHODCALLTYPE UpdateOffsetFromRoot(UINT) PURE;
	virtual UINT STDMETHODCALLTYPE GetSize() PURE;
	virtual HRESULT STDMETHODCALLTYPE SetSize(UINT) PURE;
	virtual INT STDMETHODCALLTYPE IsFocusable() PURE;
	virtual HRESULT STDMETHODCALLTYPE SetFocus(BOOL) PURE;
	virtual UINT STDMETHODCALLTYPE GetIndexInTable() PURE;
	virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(const KEY_EVENT_RECORD*, BOOL*) PURE;
	virtual HRESULT STDMETHODCALLTYPE Destroy() PURE;
};

class ControlHandle
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, IControlHandle
	>
{
public:
	ControlHandle();
	~ControlHandle() override;

	HRESULT RuntimeClassInitialize(UINT offset, UINT size, UINT index, IConsoleUIControl* controlCallback);

	//~ Begin IControlHandle Interface
	STDMETHODIMP_(UINT) GetOffsetFromRoot() override;
	STDMETHODIMP UpdateOffsetFromRoot(UINT offset) override;
	STDMETHODIMP_(UINT) GetSize() override;
	STDMETHODIMP SetSize(UINT size) override;
	STDMETHODIMP_(BOOL) IsFocusable() override;
	STDMETHODIMP SetFocus(BOOL hasFocus) override;
	STDMETHODIMP_(UINT) GetIndexInTable() override;
	STDMETHODIMP HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	STDMETHODIMP Destroy() override;
	//~ End IControlHandle Interface

private:
	UINT m_offsetFromRoot;
	UINT m_size;
	UINT m_index;
	Microsoft::WRL::ComPtr<IConsoleUIControl> m_controlCallback;
};
