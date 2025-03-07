#include "pch.h"

#include "EventDispatcher.h"

using namespace Microsoft::WRL;

HRESULT EventDispatcher::RuntimeClassInitialize(INotificationDispatcher* dispatcher)
{
	m_dispatcher = dispatcher;
	return S_OK;
}

HRESULT EventDispatcher::DispatchEvent(LCPD::IDispatchEvent* dispatchEvent)
{
	ComPtr<Windows::Internal::UI::Logon::CredProvData::IDispatchEvent> dispatchEventRef = dispatchEvent;
	HRESULT hr = BeginInvoke(this->m_dispatcher.Get(), [dispatchEventRef]() -> void
	{
		dispatchEventRef->Dispatch();
	});
	RETURN_IF_FAILED(hr); // 28
	return S_OK;
}
