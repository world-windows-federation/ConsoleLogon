#pragma once

#include "pch.h"

#include "NotificationDispatcher.h"

class EventDispatcher final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, LCPD::IUIThreadEventDispatcher
		, Microsoft::WRL::FtmBase
	>
{
public:
	HRESULT RuntimeClassInitialize(INotificationDispatcher* dispatcher);

	//~ Begin LCPD::IUIThreadEventDispatcher Interface
	STDMETHODIMP DispatchEvent(LCPD::IDispatchEvent* dispatchEvent) override;
	//~ End LCPD::IUIThreadEventDispatcher Interface

private:
	Microsoft::WRL::ComPtr<INotificationDispatcher> m_dispatcher;
};
