#pragma once

#include "pch.h"

#include "InternalAsync.h"

class ConsoleLockAction
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, LC::IUnlockTrigger
		, LC::ILockInfo
		, WF::IAsyncOperation<HSTRING>
		, WI::AsyncBaseFTM<
			  WF::IAsyncOperationCompletedHandler<HSTRING>
			, Microsoft::WRL::SingleResult
			, Microsoft::WRL::AsyncOptions<>
		>
	>
{
public:
	ConsoleLockAction();
	~ConsoleLockAction() override;

	HRESULT RuntimeClassInitialize(HSTRING domainName, HSTRING userName, HSTRING friendlyName);

	//~ Begin LC::IUnlockTrigger Interface
    STDMETHODIMP TriggerUnlock() override;
    STDMETHODIMP SyncBackstop() override;
    STDMETHODIMP CheckCompletion() override;
	//~ End LC::IUnlockTrigger Interface

	//~ Begin LC::ILockInfo Interface
    STDMETHODIMP get_VisualOwner(LC::LockDisplayOwner* value) override;
    STDMETHODIMP get_DomainName(HSTRING* value) override;
    STDMETHODIMP get_UserName(HSTRING* value) override;
    STDMETHODIMP get_FriendlyName(HSTRING* value) override;
    STDMETHODIMP get_RequireSecureGesture(BOOLEAN* value) override;
    STDMETHODIMP get_ShowSpeedBump(BOOLEAN* value) override;
    STDMETHODIMP get_RequireSecureGestureString(HSTRING* value) override;
    STDMETHODIMP get_SpeedBumpString(HSTRING* value) override;
    STDMETHODIMP get_IsLostMode(BOOLEAN* value) override;
    STDMETHODIMP get_LostModeMessage(HSTRING* value) override;
    STDMETHODIMP add_UserActivity(WF::ITypedEventHandler<LC::ILockInfo *, LC::LockActivity>* handler, EventRegistrationToken* token) override;
    STDMETHODIMP remove_UserActivity(EventRegistrationToken token) override;
	//~ End LC::ILockInfo Interface

	//~ Begin WF::IAsyncOperation<HSTRING> Interface
    STDMETHODIMP put_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>* pRequestHandler) override;
    STDMETHODIMP get_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>** ppRequestHandler) override;
	STDMETHODIMP GetResults(HSTRING* results) override;
	//~ End WF::IAsyncOperation<HSTRING> Interface

    HRESULT OnStart() override;
    void OnClose() override;
    void OnCancel() override;

private:
	Microsoft::WRL::Wrappers::HString m_domainName;
    Microsoft::WRL::Wrappers::HString m_userName;
    Microsoft::WRL::Wrappers::HString m_friendlyName;
    Microsoft::WRL::EventSource<WF::ITypedEventHandler<LC::ILockInfo*, LC::LockActivity>> m_userActivityEvent;
};
