#include "pch.h"

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

#include "logoninterfaces.h"

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
	WCHAR* InternalGetRuntimeClassName();
	TrustLevel InternalGetTrustLevel();

	//~ Begin IInspectable Interface
	STDMETHODIMP GetRuntimeClassName(HSTRING* className) override;
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
	STDMETHODIMP Start(IInspectable*, IRedirectionManager*, IUserSettingManager*, IDisplayStateProvider*, IBioFeedbackListener*) override;
	STDMETHODIMP DelayLock(BOOLEAN, IUnlockTrigger*) override;
	STDMETHODIMP HardLock(LogonUIRequestReason, BYTE, IUnlockTrigger*) override;
	STDMETHODIMP RequestCredentialsAsync(LogonUIRequestReason, LogonUIFlags, IAsyncOperation<RequestCredentialsData*>**) override;
	STDMETHODIMP ReportCredentialsAsync(LogonUIRequestReason, long, long, HSTRING, HSTRING, HSTRING, IAsyncOperation<ReportCredentialsData*>**) override;
	STDMETHODIMP DisplayMessageAsync(LogonMessageMode, UINT, HSTRING, HSTRING, IAsyncOperation<MessageDisplayResult*>**) override;
	STDMETHODIMP DisplayCredentialErrorAsync(long, long, UINT, HSTRING, HSTRING, IAsyncOperation<MessageDisplayResult*>**) override;
	STDMETHODIMP DisplayStatusAsync(LogonUIState, HSTRING, IAsyncAction**) override;
	STDMETHODIMP TriggerLogonAnimationAsync(IAsyncAction**) override;
	STDMETHODIMP ResetCredentials() override;
	STDMETHODIMP RestoreFromFirstSignInAnimation() override;
	STDMETHODIMP ClearUIState(HSTRING) override;
	STDMETHODIMP ShowSecurityOptionsAsync(LogonUISecurityOptions, IAsyncOperation<LogonUISecurityOptionsResult*>**) override;
	STDMETHODIMP get_WindowContainer(IInspectable**) override;
	STDMETHODIMP Hide() override;
	STDMETHODIMP Stop() override;
	//~ End Windows::Internal::UI::Logon::Controller::ILogonUX Interface

private:
	~ConsoleLogon() override;

	HRESULT CheckUIStarted();
	HRESULT Lock(LogonUIRequestReason, bool, IUnlockTrigger*);

	Wrappers::SRWLock m_Lock;
	//ComPtr<LogonViewManager> m_consoleUIManager;
};