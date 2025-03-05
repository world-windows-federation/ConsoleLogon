#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

// f4c33f16-f5a6-4c7e-afe7-29c3df6ad04b
struct IControlHandle : public IUnknown
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
