#include "pch.h"

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include "InternalAsync.h"
#include "logoninterfaces.h"
#include "logonviewmanager.h"
#include "RefCountedObject.h"

using namespace Microsoft::WRL;

using namespace ABI::Windows::Foundation;
using namespace Windows::Internal::UI::Logon::Controller;
using namespace Windows::Internal::UI::Logon::CredProvData;

class DECLSPEC_UUID("00000000-0000-0000-0000-000000000000")
ConsoleLogon final
	: public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>
		, ILogonUX
		, FtmBase
	>
{
public:
	static const WCHAR* InternalGetRuntimeClassName();
	static TrustLevel InternalGetTrustLevel();

	//~ Begin IInspectable Interface
	STDMETHODIMP GetRuntimeClassName(HSTRING* runtimeName) override;
	STDMETHODIMP GetTrustLevel(TrustLevel* trustLevel) override;
	STDMETHODIMP GetIids(ULONG* iidCount, GUID** iids) override;
	//~ End IInspectable Interface

	//~ Begin IUnknown Interface
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override;
	STDMETHODIMP_(ULONG) Release() override;
	STDMETHODIMP_(ULONG) AddRef() override;
	//~ End IUnknown Interface

	ConsoleLogon();

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

	//~ Begin Windows::Internal::UI::Logon::Controller::ILogonUX Interface
	STDMETHODIMP Start(
		IInspectable* autoLogonManager, IRedirectionManager* redirectionManager,
		IUserSettingManager* userSettingManager, IDisplayStateProvider* displayStateProvider,
		IBioFeedbackListener* bioFeedbackListener) override;
	STDMETHODIMP DelayLock(BOOLEAN allowDirectUserSwitching, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, IUnlockTrigger* unlockTrigger) override;
	STDMETHODIMP RequestCredentialsAsync(
		LogonUIRequestReason reason, LogonUIFlags flags, IAsyncOperation<RequestCredentialsData*>** ppOperation) override;
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
	STDMETHODIMP TriggerLogonAnimationAsync(IAsyncAction** ppAction) override;
	STDMETHODIMP ResetCredentials() override;
	STDMETHODIMP RestoreFromFirstSignInAnimation() override;
	STDMETHODIMP ClearUIState(HSTRING statusMessage) override;
	STDMETHODIMP ShowSecurityOptionsAsync(
		LogonUISecurityOptions options, IAsyncOperation<LogonUISecurityOptionsResult*>** ppOperation) override;
	STDMETHODIMP get_WindowContainer(IInspectable** value) override;
	STDMETHODIMP Hide() override;
	STDMETHODIMP Stop() override;
	//~ End Windows::Internal::UI::Logon::Controller::ILogonUX Interface

private:
	~ConsoleLogon() override;

	HRESULT CheckUIStarted();
	HRESULT Lock(LogonUIRequestReason reason, bool allowDirectUserSwitching, IUnlockTrigger* unlockTrigger);

	template <typename TResult, typename TLambda>
	HRESULT ConsoleLogon::CancellableAsyncOperationThreadProc(Windows::Internal::AsyncStage stage, HRESULT hr, TResult& result, const TLambda& lambda)
	{
		if (SUCCEEDED(hr) && stage == Windows::Internal::AsyncStage::Execute)
		{
			Windows::Internal::AsyncDeferral<TResult> deferral = result.GetDeferral(result);
			RETURN_IF_FAILED(lambda(deferral.GetResult())); // 442
		}
		else if (stage == Windows::Internal::AsyncStage::Cancel)
		{
			Windows::Internal::AsyncDeferral<TResult> deferral = result.GetDeferral(result);
			deferral.Complete(S_OK);
		}
		return S_OK;
	}

	template <
		typename TResult, // Windows::Internal::CMarshaledInterfaceResult<IRequestCredentialsData>,
		typename TResultRaw, // RequestCredentialsData*
		typename THandler, // Windows::Internal::ComTaskPoolHandler
		typename TLambda // <lambda_3785fc05d0c47cf9a9ab843267e7d6bb>
	>
	HRESULT ConsoleLogon::MakeCancellableAsyncOperation(THandler&& handler, IAsyncOperation<TResultRaw>** ppOperation, const TLambda& lambda)
	{
		*ppOperation = nullptr;
		ComPtr<ConsoleLogon> thisRef = this;
		HRESULT hr = Windows::Internal::MakeAsyncHelper<
			IAsyncOperation<TResultRaw>,
			IAsyncOperationCompletedHandler<TResultRaw>,
			Windows::Internal::INilDelegate,
			TResult,
			THandler&,
			AsyncOptions<>
		>(
			ppOperation,
			handler,
			L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.Controller.RequestCredentialsData>", // TODO Don't hardcode this
			BaseTrust,
			Windows::Internal::MakeOperationStagedLambda<TResult>([this, thisRef, lambda](Windows::Internal::AsyncStage stage, HRESULT hr, TResult& result) -> HRESULT
			{
				return CancellableAsyncOperationThreadProc<TResult, TLambda>(stage, hr, result, lambda);
			})
		);
		RETURN_IF_FAILED(hr); // 462
		return S_OK;
	}

	template <
		typename TOptions, // AsyncCausalityOptions<&unsigned short const * const DisplayStatusAction,&struct _GUID const GUID_CAUSALITY_WINDOWS_PLATFORM_ID,2>
		typename THandler, // Windows::Internal::ComTaskPoolHandler
		typename TLambda // _lambda_fe7b513293bb3593300cd8367b69ca74_
	>
	HRESULT ConsoleLogon::MakeCancellableAsyncAction(THandler&& handler, IAsyncAction** ppAction, const TLambda& lambda)
	{
		*ppAction = nullptr;
		ComPtr<ConsoleLogon> thisRef = this;
		HRESULT hr = Windows::Internal::MakeAsyncHelper<
			IAsyncAction,
			IAsyncActionCompletedHandler,
			Windows::Internal::INilDelegate,
			Windows::Internal::CNoResult,
			THandler&,
			TOptions
		>(
			ppAction,
			handler,
			L"Windows.Foundation.IAsyncAction",
			BaseTrust,
			Windows::Internal::MakeOperationStagedLambda<Windows::Internal::CNoResult>([this, thisRef, lambda](Windows::Internal::AsyncStage stage, HRESULT hr, Windows::Internal::CNoResult& result) -> HRESULT
			{
				return CancellableAsyncOperationThreadProc<Windows::Internal::CNoResult, TLambda>(stage, hr, result, lambda);
			})
		);
		RETURN_IF_FAILED(hr); // 475
		return S_OK;
	}

	Wrappers::SRWLock m_Lock;
	ComPtr<LogonViewManager> m_consoleUIManager;
};

extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Internal_UI_Logon_Controller_ConsoleLogonUX[] = L"Windows.Internal.UI.Logon.Controller.ConsoleLogonUX";

const WCHAR* ConsoleLogon::InternalGetRuntimeClassName()
{
	return RuntimeClass_Internal_UI_Logon_Controller_ConsoleLogonUX;
}

TrustLevel ConsoleLogon::InternalGetTrustLevel()
{
	return FullTrust;
}

HRESULT ConsoleLogon::GetRuntimeClassName(HSTRING* runtimeName)
{
	*runtimeName = nullptr;
	return WindowsCreateString(RuntimeClass_Internal_UI_Logon_Controller_ConsoleLogonUX, wcslen(RuntimeClass_Internal_UI_Logon_Controller_ConsoleLogonUX), runtimeName);
}

HRESULT ConsoleLogon::GetTrustLevel(TrustLevel* trustLevel)
{
	*trustLevel = FullTrust;
	return S_OK;
}

HRESULT ConsoleLogon::GetIids(ULONG* iidCount, GUID** iids)
{
	return RuntimeClass::GetIids(iidCount, iids);
}

HRESULT ConsoleLogon::QueryInterface(const IID& riid, void** ppvObject)
{
	return RuntimeClass::QueryInterface(riid, ppvObject);
}

ULONG ConsoleLogon::Release()
{
	return RuntimeClass::Release();
}

ULONG ConsoleLogon::AddRef()
{
	return RuntimeClass::AddRef();
}

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

HRESULT ConsoleLogon::DelayLock(BOOLEAN allowDirectUserSwitching, IUnlockTrigger* unlockTrigger)
{
	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 118

	RETURN_IF_FAILED(Lock(LogonUIRequestReason_LogonUIUnlock, allowDirectUserSwitching, unlockTrigger)); // 120
	return S_OK;
}

HRESULT ConsoleLogon::HardLock(LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, IUnlockTrigger* unlockTrigger)
{
	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 131

	RETURN_IF_FAILED(Lock(reason, allowDirectUserSwitching, unlockTrigger)); // 133
	return S_OK;
}

HRESULT ConsoleLogon::RequestCredentialsAsync(
	LogonUIRequestReason reason, LogonUIFlags flags, IAsyncOperation<RequestCredentialsData*>** ppOperation)
{
	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 145

	ComPtr<ConsoleLogon> asyncReference = this;
	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<Windows::Internal::CMarshaledInterfaceResult<IRequestCredentialsData>, RequestCredentialsData*>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppOperation,
		[asyncReference, this, =](Windows::Internal::CMarshaledInterfaceResult<IRequestCredentialsData>& result) -> HRESULT
		{
			RETURN_IF_FAILED(viewManager->RequestCredentials(reason, flags, result)); // 156
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
	ComPtr<CRefCountedObject<Wrappers::HString>> samCompatibleUserNameRef;
	ComPtr<CRefCountedObject<Wrappers::HString>> displayNameRef;
	ComPtr<CRefCountedObject<Wrappers::HString>> userSidRef;

	*ppOperation = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 169

	CreateRefCountedObj<Wrappers::HString>(&samCompatibleUserNameRef);
	RETURN_IF_FAILED(samCompatibleUserNameRef->Set(samCompatibleUserName)); // 172

	CreateRefCountedObj<Wrappers::HString>(&displayNameRef);
	RETURN_IF_FAILED(displayNameRef->Set(displayName)); // 175

	CreateRefCountedObj<Wrappers::HString>(&userSidRef);
	RETURN_IF_FAILED(userSidRef->Set(userSid)); // 178

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<Windows::Internal::CMarshaledInterfaceResult<IReportCredentialsData>, ReportCredentialsData*>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppOperation,
		[=](Windows::Internal::CMarshaledInterfaceResult<IReportCredentialsData>& result) -> HRESULT
		{
			HRESULT hrInner = viewManager->ReportResult(
				reason, ntsStatus, ntsSubStatus, samCompatibleUserNameRef->Get(), displayNameRef->Get(),
				userSidRef->Get(), result);
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
	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef;
	CreateRefCountedObj<Wrappers::HString>(&captionRef);
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 206
	}
	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef;
	CreateRefCountedObj<Wrappers::HString>(&messageRef);
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 212
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<Windows::Internal::CMarshaledInterfaceResult<IMessageDisplayResult>, MessageDisplayResult*>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppOperation,
		[=](Windows::Internal::CMarshaledInterfaceResult<IMessageDisplayResult>& result) -> HRESULT
		{
			RETURN_IF_FAILED(viewManager->DisplayMessage(messageMode, messageBoxFlags, captionRef->Get(), messageRef->Get(), result)); // 224
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

	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef;
	CreateRefCountedObj<Wrappers::HString>(&captionRef);
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 241
	}

	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef;
	CreateRefCountedObj<Wrappers::HString>(&messageRef);
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 247
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncOperation<Windows::Internal::CMarshaledInterfaceResult<IMessageDisplayResult>, MessageDisplayResult*>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppOperation,
		[=](Windows::Internal::CMarshaledInterfaceResult<IMessageDisplayResult>& result) -> HRESULT
		{
			RETURN_IF_FAILED(viewManager->DisplayCredentialError(ntsStatus, ntsSubstatus, messageBoxFlags, captionRef->Get(), messageRef->Get(), result)); // 259
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 259
	return S_OK;
}

const WCHAR DisplayStatusAction[] = L"Windows.Foundation.IAsyncAction ConsoleLogon.DisplayStatus";

HRESULT ConsoleLogon::DisplayStatusAsync(LogonUIState state, HSTRING status, IAsyncAction** ppAction)
{
	*ppAction = nullptr;

	Wrappers::SRWLock::SyncLockShared lock = m_Lock.LockShared();
	RETURN_IF_FAILED(CheckUIStarted()); // 271

	ComPtr<CRefCountedObject<Wrappers::HString>> statusRef;
	CreateRefCountedObj<Wrappers::HString>(&statusRef);
	if (status)
	{
		RETURN_IF_FAILED(statusRef->Set(status)); // 276
	}

	ComPtr<LogonViewManager> viewManager = m_consoleUIManager;
	HRESULT hr = MakeCancellableAsyncAction<AsyncCausalityOptions<DisplayStatusAction>>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppAction,
		[=](Windows::Internal::CNoResult& result) -> HRESULT
		{
			RETURN_IF_FAILED(viewManager->DisplayStatus(state, statusRef->Get(), result)); // 288
			return S_OK;
		}
	);
	RETURN_IF_FAILED(hr); // 288
	return S_OK;
}

const WCHAR LogonAnimationAction[] = L"Windows.Foundation.IAsyncAction ConsoleLogon.LogonAnimation";

HRESULT ConsoleLogon::TriggerLogonAnimationAsync(IAsyncAction** ppAction)
{
	*ppAction = nullptr;

	HRESULT hr = MakeCancellableAsyncAction<AsyncCausalityOptions<LogonAnimationAction>>(
		Windows::Internal::ComTaskPoolHandler(Windows::Internal::TaskApartment::Any, Windows::Internal::TaskOptions::SyncNesting),
		ppAction,
		[=](Windows::Internal::CNoResult& result) -> HRESULT
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

HRESULT ConsoleLogon::ClearUIState(HSTRING statusMessage)
{
}

HRESULT ConsoleLogon::ShowSecurityOptionsAsync(LogonUISecurityOptions options, IAsyncOperation<LogonUISecurityOptionsResult*>** ppOperation)
{
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
