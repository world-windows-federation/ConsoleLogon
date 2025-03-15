#pragma once

#include "pch.h"

extern const __declspec(selectany) _Null_terminated_ WCHAR ConsoleLockAsyncAction[];

class ConsoleLockAction
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		,LC::IUnlockTrigger
		,LC::ILockInfo
		,WF::IAsyncAction
		,WI::AsyncBaseFTM<WF::IAsyncActionCompletedHandler
			, Microsoft::WRL::SingleResult
			, Microsoft::WRL::AsyncCausalityOptions<ConsoleLockAsyncAction,GUID_CAUSALITY_WINDOWS_PLATFORM_ID, WF::Diagnostics::CausalitySource_System>>
		>
{
public:
	ConsoleLockAction();
	~ConsoleLockAction() override;
	
	HRESULT RuntimeClassInitialize(HSTRING domainName, HSTRING userName, HSTRING friendlyName);
    HRESULT TriggerUnlock() override;
    HRESULT SyncBackstop() override;
    HRESULT CheckCompletion() override;
    HRESULT get_VisualOwner(LC::LockDisplayOwner* value) override;
    HRESULT get_DomainName(HSTRING* value) override;
    HRESULT get_UserName(HSTRING* value) override;
    HRESULT get_FriendlyName(HSTRING* value) override;
    HRESULT get_RequireSecureGesture(bool* value) override;
    HRESULT get_ShowSpeedBump(bool* value) override;
    HRESULT get_RequireSecureGestureString(HSTRING* value) override;
    HRESULT get_SpeedBumpString(HSTRING* value) override;
    HRESULT add_UserActivity(WF::ITypedEventHandler<LC::ILockInfo *, LC::LockActivity>* handler, EventRegistrationToken* token) override;
    HRESULT remove_UserActivity(EventRegistrationToken token) override;
    HRESULT put_Completed(WF::IAsyncActionCompletedHandler* pRequestHandler) override;
    HRESULT get_Completed(WF::IAsyncActionCompletedHandler** ppRequestHandler) override;
    HRESULT GetResults() override;
    HRESULT OnStart() override;
    void OnClose() override;
    void OnCancel() override;
private:
	Microsoft::WRL::Wrappers::HString m_domainName;
    Microsoft::WRL::Wrappers::HString m_userName;
    Microsoft::WRL::Wrappers::HString m_friendlyName;
    Microsoft::WRL::EventSource<WF::ITypedEventHandler<LC::ILockInfo*, LC::LockActivity>> m_userActivityEvent;
};
