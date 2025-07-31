#include "pch.h"

#include "logonviewmanager.h"

#include "WaitForCompletion.h"

using namespace Microsoft::WRL;

using namespace ABI::Windows::Foundation;
using namespace Windows::Internal::UI::Logon::Controller;
using namespace Windows::Internal::UI::Logon::CredProvData;

extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLogonUX[] = L"Windows.Internal.UI.Logon.Controller.ConsoleLogonUX";

class ConsoleLogon final
	: public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>
		, ILogonUX
		, FtmBase
	>
{
	InspectableClass(RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLogonUX, FullTrust);

public:
	ConsoleLogon();

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

	//~ Begin Windows::Internal::UI::Logon::Controller::ILogonUX Interface
	STDMETHODIMP Start(
		IInspectable* autoLogonManager, IRedirectionManager* redirectionManager,
		IUserSettingManager* userSettingManager, IDisplayStateProvider* displayStateProvider,
		IBioFeedbackListener* bioFeedbackListener) override;
#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
	STDMETHODIMP DelayLock(BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, HSTRING unk3, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, HSTRING unk3, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP RequestCredentialsAsync(
		LogonUIRequestReason reason, LogonUIFlags flags, HSTRING unk, IAsyncOperation<RequestCredentialsData*>** ppOperation) override;
#else
	STDMETHODIMP DelayLock(BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP RequestCredentialsAsync(
		LogonUIRequestReason reason, LogonUIFlags flags, IAsyncOperation<RequestCredentialsData*>** ppOperation) override;
#endif
	STDMETHODIMP ReportCredentialsAsync(
		LogonUIRequestReason reason, NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, HSTRING samCompatibleUserName,
		HSTRING displayName, HSTRING userSid, IAsyncOperation<ReportCredentialsData*>** ppOperation) override;
	STDMETHODIMP DisplayMessageAsync(
		LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		IAsyncOperation<MessageDisplayResult*>** ppOperation) override;
	STDMETHODIMP DisplayCredentialErrorAsync(
		NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		IAsyncOperation<MessageDisplayResult*>** ppOperation) override;
	STDMETHODIMP DisplayStatusAsync(LogonUIState state, HSTRING status, IAsyncAction** ppAction) override;
	STDMETHODIMP DisplayStatusAndForceCredentialPageAsync(
		LogonUIRequestReason reason, LogonUIFlags flags, HSTRING unk1, LogonUIState state, HSTRING hstrStatus,
		IAsyncAction** ppAction) override;
	STDMETHODIMP TriggerLogonAnimationAsync(IAsyncAction** ppAction) override;
	STDMETHODIMP ResetCredentials() override;
	STDMETHODIMP RestoreFromFirstSignInAnimation() override;
	STDMETHODIMP ClearUIState(HSTRING statusMessage) override;
	STDMETHODIMP ShowSecurityOptionsAsync(
		LogonUISecurityOptions options, IAsyncOperation<LogonUISecurityOptionsResult*>** ppOperation) override;
	STDMETHODIMP WebDialogDisplayed(void*) override;
	STDMETHODIMP get_WindowContainer(IInspectable** value) override;
	STDMETHODIMP Hide() override;
	STDMETHODIMP Stop() override;
	//~ End Windows::Internal::UI::Logon::Controller::ILogonUX Interface

private:
	~ConsoleLogon() override;

	HRESULT CheckUIStarted();
	HRESULT Lock(LogonUIRequestReason reason, bool allowDirectUserSwitching, IUnlockTrigger* unlockTrigger);

	template <typename TResult, typename TLambda>
	HRESULT CancellableAsyncOperationThreadProc(WI::AsyncStage stage, HRESULT hr, TResult& result, const TLambda& lambda)
	{
		if (SUCCEEDED(hr) && stage == WI::AsyncStage::Execute)
		{
			RETURN_IF_FAILED(lambda(result)); // 442
		}
		else if (stage == WI::AsyncStage::Cancel)
		{
			WI::AsyncDeferral<TResult> deferral = result.GetDeferral(result);
			deferral.Complete(S_OK);
		}
		return S_OK;
	}

	template <
		typename TResult, // WI::CMarshaledInterfaceResult<IRequestCredentialsData>,
		typename TResultRaw, // RequestCredentialsData*
		typename THandler, // WI::ComTaskPoolHandler
		typename TLambda // <lambda_3785fc05d0c47cf9a9ab843267e7d6bb>
	>
	HRESULT MakeCancellableAsyncOperation(THandler&& handler, IAsyncOperation<TResultRaw>** ppOperation, const TLambda& lambda)
	{
		*ppOperation = nullptr;
		ComPtr<ConsoleLogon> thisRef = this;
		HRESULT hr = WI::MakeAsyncOperationHelper(
			wistd::forward<THandler>(handler),
			ppOperation,
			BaseTrust,
			WI::MakeOperationStagedLambda<TResult>([this, thisRef, lambda](WI::AsyncStage stage, HRESULT hr, TResult& result) -> HRESULT
			{
				UNREFERENCED_PARAMETER(thisRef);
				return CancellableAsyncOperationThreadProc<TResult, TLambda>(stage, hr, result, lambda);
			})
		);
		RETURN_IF_FAILED(hr); // 462
		return S_OK;
	}

	template <
		typename TOptions, // AsyncCausalityOptions<&unsigned short const * const DisplayStatusAction,&struct _GUID const GUID_CAUSALITY_WINDOWS_PLATFORM_ID,2>
		typename THandler, // WI::ComTaskPoolHandler
		typename TLambda // _lambda_fe7b513293bb3593300cd8367b69ca74_
	>
	HRESULT MakeCancellableAsyncAction(THandler&& handler, IAsyncAction** ppAction, const TLambda& lambda)
	{
		*ppAction = nullptr;
		ComPtr<ConsoleLogon> thisRef = this;
		HRESULT hr = WI::MakeAsyncActionHelper<THandler, TOptions>(
			wistd::forward<THandler>(handler),
			ppAction,
			BaseTrust,
			WI::MakeOperationStagedLambda<WI::CNoResult>([this, thisRef, lambda](WI::AsyncStage stage, HRESULT hr, WI::CNoResult& result) -> HRESULT
			{
				UNREFERENCED_PARAMETER(thisRef);
				return CancellableAsyncOperationThreadProc<WI::CNoResult, TLambda>(stage, hr, result, lambda);
			})
		);
		RETURN_IF_FAILED(hr); // 475
		return S_OK;
	}

	Wrappers::SRWLock m_Lock;
	ComPtr<LogonViewManager> m_consoleUIManager;
};

ConsoleLogon::ConsoleLogon()
{
}

HRESULT ConsoleLogon::RuntimeClassInitialize()
{
	return S_OK;
}

HRESULT ConsoleLogon::Start(
	IInspectable* autoLogonManager, IRedirectionManager* redirectionManager,
	IUserSettingManager* userSettingManager, IDisplayStateProvider* displayStateProvider,
	IBioFeedbackListener* bioFeedbackListener)
{
	Wrappers::SRWLock::SyncLockExclusive lock = m_Lock.LockExclusive();
	RETURN_IF_FAILED(MakeAndInitialize<LogonViewManager>(&m_consoleUIManager)); // 102

	RETURN_IF_FAILED(m_consoleUIManager->StartUI()); // 104

	RETURN_IF_FAILED(m_consoleUIManager->SetContext(autoLogonManager, userSettingManager, redirectionManager, displayStateProvider, bioFeedbackListener)); // 106
	return S_OK;
}
#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
HRESULT ConsoleLogon::DelayLock(BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, HSTRING unk3, IUnlockTrigger* unlockTrigger)
#else
HRESULT ConsoleLogon::DelayLock(BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, IUnlockTrigger* unlockTrigger)
#endif
{
	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 118

	RETURN_IF_FAILED(Lock(LogonUIRequestReason_LogonUIUnlock, allowDirectUserSwitching, unlockTrigger)); // 120
	return S_OK;
}
#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
HRESULT ConsoleLogon::HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, HSTRING unk3, IUnlockTrigger* unlockTrigger)
#else
HRESULT ConsoleLogon::HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, UCHAR unk1, UCHAR unk2, IUnlockTrigger* unlockTrigger)
#endif
{
	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 131

	RETURN_IF_FAILED(Lock(reason, allowDirectUserSwitching, unlockTrigger)); // 133
	return S_OK;
}
#if CONSOLELOGON_FOR >= CONSOLELOGON_FOR_19h1
HRESULT ConsoleLogon::RequestCredentialsAsync(
	LogonUIRequestReason reason, LogonUIFlags flags, HSTRING unk, IAsyncOperation<RequestCredentialsData*>** ppOperation)
#else
HRESULT ConsoleLogon::RequestCredentialsAsync(
	LogonUIRequestReason reason, LogonUIFlags flags, IAsyncOperation<RequestCredentialsData*>** ppOperation)
#endif
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 145

	//MessageBox(0,WindowsGetStringRawBuffer(unk,0),WindowsGetStringRawBuffer(unk,0),0);

	ComPtr<ConsoleLogon> asyncReference = this;
	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<WI::CMarshaledInterfaceResult<IRequestCredentialsData>, RequestCredentialsData*>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppOperation,
		[asyncReference, this, viewManager, reason, flags](WI::CMarshaledInterfaceResult<IRequestCredentialsData>& result) -> HRESULT // @Mod: pass in ref of unk param
		{
			UNREFERENCED_PARAMETER(asyncReference);
			WI::AsyncDeferral<WI::CMarshaledInterfaceResult<IRequestCredentialsData>> deferral = result.GetDeferral(result);
			RETURN_IF_FAILED(viewManager->RequestCredentials(reason, flags, nullptr, deferral)); // 156
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 156
	return S_OK;
}

HRESULT ConsoleLogon::ReportCredentialsAsync(
	LogonUIRequestReason reason, NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, HSTRING samCompatibleUserName,
	HSTRING displayName, HSTRING userSid, IAsyncOperation<ReportCredentialsData*>** ppOperation)
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 169

	ComPtr<CRefCountedObject<Wrappers::HString>> samCompatibleUserNameRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(samCompatibleUserNameRef->Set(samCompatibleUserName)); // 172

	ComPtr<CRefCountedObject<Wrappers::HString>> displayNameRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(displayNameRef->Set(displayName)); // 175

	ComPtr<CRefCountedObject<Wrappers::HString>> userSidRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(userSidRef->Set(userSid)); // 178

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<WI::CMarshaledInterfaceResult<IReportCredentialsData>, ReportCredentialsData*>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppOperation,
		[=](WI::CMarshaledInterfaceResult<IReportCredentialsData>& result) -> HRESULT
		{
			WI::AsyncDeferral<WI::CMarshaledInterfaceResult<IReportCredentialsData>> deferral = result.GetDeferral(result);
			HRESULT hrInner = viewManager->ReportResult(
				reason, ntsStatus, ntsSubStatus, samCompatibleUserNameRef->Get(), displayNameRef->Get(),
				userSidRef->Get(), deferral);
			RETURN_IF_FAILED(hrInner); // 189
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 189
	return S_OK;
}

HRESULT ConsoleLogon::DisplayMessageAsync(
	LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	IAsyncOperation<MessageDisplayResult*>** ppOperation)
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 201

	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef = CreateRefCountedObj<Wrappers::HString>();
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 206
	}

	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef = CreateRefCountedObj<Wrappers::HString>();
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 212
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<WI::CMarshaledInterfaceResult<IMessageDisplayResult>, MessageDisplayResult*>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppOperation,
		[=](WI::CMarshaledInterfaceResult<IMessageDisplayResult>& result) -> HRESULT
		{
			WI::AsyncDeferral<WI::CMarshaledInterfaceResult<IMessageDisplayResult>> deferral = result.GetDeferral(result);
			RETURN_IF_FAILED(viewManager->DisplayMessage(
				messageMode, messageBoxFlags, captionRef->Get(), messageRef->Get(), deferral)); // 224
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 224
	return S_OK;
}

HRESULT ConsoleLogon::DisplayCredentialErrorAsync(
	NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	IAsyncOperation<MessageDisplayResult*>** ppOperation)
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 236

	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef = CreateRefCountedObj<Wrappers::HString>();
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 241
	}

	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef = CreateRefCountedObj<Wrappers::HString>();
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 247
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<WI::CMarshaledInterfaceResult<IMessageDisplayResult>, MessageDisplayResult*>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppOperation,
		[=](WI::CMarshaledInterfaceResult<IMessageDisplayResult>& result) -> HRESULT
		{
			RETURN_IF_FAILED(viewManager->DisplayCredentialError(
				ntsStatus, ntsSubstatus, messageBoxFlags, captionRef->Get(), messageRef->Get(),
				result.GetDeferral(result))); // 259
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 259
	return S_OK;
}

static const WCHAR DisplayStatusAction[] = L"Windows.Foundation.IAsyncAction ConsoleLogon.DisplayStatus";

HRESULT ConsoleLogon::DisplayStatusAsync(LogonUIState state, HSTRING status, IAsyncAction** ppAction)
{
	*ppAction = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 271

	ComPtr<CRefCountedObject<Wrappers::HString>> statusRef = CreateRefCountedObj<Wrappers::HString>();
	if (status)
	{
		RETURN_IF_FAILED(statusRef->Set(status)); // 276
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncAction<AsyncCausalityOptions<DisplayStatusAction>>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppAction,
		[=](WI::CNoResult& result) -> HRESULT
		{
			WI::AsyncDeferral<WI::CNoResult> deferral = result.GetDeferral(result);
			RETURN_IF_FAILED(viewManager->DisplayStatus(state, statusRef->Get(), deferral)); // 288
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 288
	return S_OK;
}

HRESULT ConsoleLogon::DisplayStatusAndForceCredentialPageAsync(LogonUIRequestReason reason, LogonUIFlags flags,
	HSTRING unk1, LogonUIState state, HSTRING hstrStatus, IAsyncAction** ppAction)
{
	return ConsoleLogon::DisplayStatusAsync(state, hstrStatus, ppAction);
}

static const WCHAR LogonAnimationAction[] = L"Windows.Foundation.IAsyncAction ConsoleLogon.LogonAnimation";

HRESULT ConsoleLogon::TriggerLogonAnimationAsync(IAsyncAction** ppAction)
{
	*ppAction = nullptr;

	HRESULT hr = MakeCancellableAsyncAction<AsyncCausalityOptions<LogonAnimationAction>>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppAction,
		[=](WI::CNoResult& result) -> HRESULT
	{
		return S_OK;
	});
	RETURN_IF_FAILED(hr); // 302
	return S_OK;
}

HRESULT ConsoleLogon::ResetCredentials()
{
	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 311

	RETURN_IF_FAILED(m_consoleUIManager->ClearCredentialState()); // 313
	return S_OK;
}

HRESULT ConsoleLogon::RestoreFromFirstSignInAnimation()
{
	return S_OK;
}

static const WCHAR StopAction[] = L"Windows.Foundation.IAsyncAction ConsoleLogon.Stop";

HRESULT ConsoleLogon::ClearUIState(HSTRING statusMessage)
{
	Wrappers::SRWLock::SyncLockExclusive lock = m_Lock.LockExclusive();
	if (SUCCEEDED(CheckUIStarted()))
	{
		ComPtr<ConsoleLogon> asyncReference = this;
		ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
		ComPtr<IAsyncAction> cleanupAction;
		HRESULT hr = WI::MakeAsyncActionHelper<WI::ComTaskPoolHandler, AsyncCausalityOptions<StopAction>>(
			WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
			&cleanupAction,
			BaseTrust,
			WI::MakeOperationLambda<WI::CNoResult>([asyncReference, this, viewManager](WI::CNoResult& result) -> HRESULT
			{
				UNREFERENCED_PARAMETER(asyncReference);
				WI::AsyncDeferral<WI::CNoResult> deferral = result.GetDeferral(result);
				RETURN_IF_FAILED(viewManager->Cleanup(deferral)); // 341
				return S_OK;
			})
		);
		RETURN_IF_FAILED(hr); // 341

		RETURN_IF_FAILED(WaitForCompletion<IAsyncActionCompletedHandler>(cleanupAction.Get())); // 343

		ComPtr<CRefCountedObject<Wrappers::HString>> statusRef = CreateRefCountedObj<Wrappers::HString>();
		if (statusMessage)
		{
			RETURN_IF_FAILED(statusRef->Set(statusMessage)); // 348
		}

		ComPtr<IAsyncAction> displayStatusAction;
		hr = MakeCancellableAsyncAction<AsyncCausalityOptions<DisplayStatusAction>>(
			WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
			&displayStatusAction,
			[=](WI::CNoResult& result) -> HRESULT
			{
				WI::AsyncDeferral<WI::CNoResult> deferral = result.GetDeferral(result);
				RETURN_IF_FAILED(viewManager->DisplayStatus(LogonUIState_Start, statusRef->Get(), deferral)); // 360
				return S_OK;
			}
		);
		RETURN_IF_FAILED(hr); // 360

		RETURN_IF_FAILED(WaitForCompletion<IAsyncActionCompletedHandler>(displayStatusAction.Get())); // 362
	}

	return S_OK;
}

HRESULT ConsoleLogon::ShowSecurityOptionsAsync(LogonUISecurityOptions options, IAsyncOperation<LogonUISecurityOptionsResult*>** ppOperation)
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 375

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<WI::CMarshaledInterfaceResult<ILogonUISecurityOptionsResult>, LogonUISecurityOptionsResult*>(
		WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
		ppOperation,
		[=](WI::CMarshaledInterfaceResult<ILogonUISecurityOptionsResult>& result) -> HRESULT
		{
			WI::AsyncDeferral<WI::CMarshaledInterfaceResult<ILogonUISecurityOptionsResult>> deferral = result.GetDeferral(result);
			RETURN_IF_FAILED(viewManager->ShowSecurityOptions(options, deferral)); // 386
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 386
	return S_OK;
}

HRESULT ConsoleLogon::WebDialogDisplayed(void*)
{
	MessageBoxW(nullptr, L"WebDialogDisplayed not Implemented", L"WebDialogDisplayed not Implemented", 0);
	return S_OK;
}

HRESULT ConsoleLogon::get_WindowContainer(IInspectable** value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT ConsoleLogon::Hide()
{
	return S_OK;
}

HRESULT ConsoleLogon::Stop()
{
	Wrappers::SRWLock::SyncLockExclusive lock = m_Lock.LockExclusive();
	if (SUCCEEDED(CheckUIStarted()))
	{
		auto scopeExit = wil::scope_exit([this]() -> void { m_consoleUIManager->StopUI(); });
		ComPtr<ConsoleLogon> asyncReference = this;
		ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
		ComPtr<IAsyncAction> cleanupAction;
		HRESULT hr = WI::MakeAsyncActionHelper<WI::ComTaskPoolHandler, AsyncCausalityOptions<StopAction>>(
			WI::ComTaskPoolHandler(WI::TaskApartment::Any, WI::TaskOptions::SyncNesting),
			&cleanupAction,
			BaseTrust,
			WI::MakeOperationLambda<WI::CNoResult>([asyncReference, this, viewManager](WI::CNoResult& result) -> HRESULT
			{
				UNREFERENCED_PARAMETER(asyncReference);
				RETURN_IF_FAILED(viewManager->Cleanup(result.GetDeferral(result))); // 341
				return S_OK;
			})
		);
		RETURN_IF_FAILED(hr); // 426

		RETURN_IF_FAILED(WaitForCompletion<IAsyncActionCompletedHandler>(cleanupAction.Get())); // 428
	}

	RETURN_IF_WIN32_BOOL_FALSE(FreeConsole()); // 431
	return S_OK;
}

ConsoleLogon::~ConsoleLogon()
{
}

HRESULT ConsoleLogon::CheckUIStarted()
{
	return m_consoleUIManager.Get() ? S_OK : E_APPLICATION_EXITING;
}

HRESULT ConsoleLogon::Lock(LogonUIRequestReason reason, bool allowDirectUserSwitching, IUnlockTrigger* unlockTrigger)
{
	RETURN_IF_FAILED(CheckUIStarted()); // 486

	RETURN_IF_FAILED(m_consoleUIManager->Lock(reason, allowDirectUserSwitching, unlockTrigger)); // 488
	return S_OK;
}

ActivatableClass(ConsoleLogon);
