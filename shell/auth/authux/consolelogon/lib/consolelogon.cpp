#include "pch.h"

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include "logoninterfaces.h"
#include "logonviewmanager.h"

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
}

HRESULT ConsoleLogon::ReportCredentialsAsync(
	LogonUIRequestReason reason, NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, HSTRING samCompatibleUserName,
	HSTRING displayName, HSTRING userSid, IAsyncOperation<ReportCredentialsData*>** ppOperation)
{
}

HRESULT ConsoleLogon::DisplayMessageAsync(
	LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	IAsyncOperation<MessageDisplayResult*>** ppOperation)
{
}

HRESULT ConsoleLogon::DisplayCredentialErrorAsync(
	NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	IAsyncOperation<MessageDisplayResult*>** ppOperation)
{
}

HRESULT ConsoleLogon::DisplayStatusAsync(LogonUIState state, HSTRING status, IAsyncAction** ppAction)
{
}

HRESULT ConsoleLogon::TriggerLogonAnimationAsync(IAsyncAction** ppAction)
{
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
