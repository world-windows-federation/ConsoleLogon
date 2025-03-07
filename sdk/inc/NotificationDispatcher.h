#pragma once

#include <ShlObj_core.h>

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include "SimpleArray.h"

MIDL_INTERFACE("0dab9293-7fd8-4284-8559-d22bcbb2376e")
IDispatchNotification : IUnknown
{
	virtual void STDMETHODCALLTYPE Dispatch() PURE;
};

MIDL_INTERFACE("961dafb3-fd25-4136-a1cc-523ee58e8ee8")
INotificationDispatcher : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE QueueNotification(IDispatchNotification*) PURE;
	virtual void STDMETHODCALLTYPE StartProcessingNotifications() PURE;
	virtual void STDMETHODCALLTYPE StopProcessingNotifications() PURE;
};

class CNotificationDispatcherBase
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, IObjectWithBackReferences
		, INotificationDispatcher
	>
{
public:
	CNotificationDispatcherBase();

	//~ Begin INotificationDispatcher Interface
	STDMETHODIMP QueueNotification(IDispatchNotification* pNotification) override;
	STDMETHODIMP_(void) StartProcessingNotifications() override;
	STDMETHODIMP_(void) StopProcessingNotifications() override;
	//~ End INotificationDispatcher Interface

	//~ Begin IObjectWithBackReferences Interface
	STDMETHODIMP RemoveBackReferences() override;
	//~ End IObjectWithBackReferences Interface

protected:
	HRESULT Initialize();

	virtual bool CanProcessWork();

	~CNotificationDispatcherBase();

	HWND _hwndMsgWindow;
	Microsoft::WRL::Wrappers::SRWLock _srwLock;
	bool _fProcessNotifications;

private:
	static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CCoSimpleArray<Microsoft::WRL::ComPtr<IDispatchNotification>> _srgPendingNotifications;
};

class CNotificationDispatcher final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, CNotificationDispatcherBase
		, Microsoft::WRL::FtmBase
	>
{
public:
	CNotificationDispatcher();

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

	static HRESULT GetForCurrentThread(INotificationDispatcher** ppDispatcher);
};

template <typename T, typename I>
FORCEINLINE HRESULT MakeNotificationDispatcher(I** ppDispatcher)
{
	*ppDispatcher = nullptr;

	Microsoft::WRL::ComPtr<IUnknown> spUnkDispatcher;
	HRESULT hr = MakeAndInitialize<T>(&spUnkDispatcher);
	if (SUCCEEDED(hr))
	{
		hr = spUnkDispatcher.CopyTo(ppDispatcher);
	}

	return hr;
}

template <typename T, typename I>
FORCEINLINE HRESULT MakeNotificationDispatcher(Microsoft::WRL::Details::ComPtrRef<Microsoft::WRL::ComPtr<I>> ppvObject)
{
	return MakeNotificationDispatcher<T, I>(ppvObject.ReleaseAndGetAddressOf());
}
