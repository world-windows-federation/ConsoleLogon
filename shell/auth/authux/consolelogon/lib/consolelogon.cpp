#include "pch.h"
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>
#include "logoninterfaces.h"

class DECLSPEC_UUID("00000000-0000-0000-0000-000000000000")
ConsoleLogon 
	: public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>, 
		Windows::Internal::UI::Logon::Controller::ILogonUX, 
		Microsoft::WRL::FtmBase>
{
public:
	WCHAR* InternalGetRuntimeClassName();
	TrustLevel InternalGetTrustLevel();
	HRESULT GetRuntimeClassName(HSTRING*);
	HRESULT GetTrustLevel(TrustLevel*);
	HRESULT GetIids(DWORD*, GUID**);

	HRESULT QueryInterface(GUID&, void**);
	unsigned long Release();
	unsigned long AddRef();

	ConsoleLogon();
	HRESULT RuntimeClassInitialize();
	HRESULT Start(IInspectable*, Windows::Internal::UI::Logon::Controller::IRedirectionManager*, Windows::Internal::UI::Logon::Controller::IUserSettingManager*, Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT DelayLock(bool, Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT HardLock(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, BYTE, Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentialsAsync(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, Windows::Internal::UI::Logon::Controller::LogonUIFlags, ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::RequestCredentialsData*>**);
	HRESULT ReportCredentialsAsync(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, HSTRING, HSTRING, HSTRING, ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::ReportCredentialsData*>**);
	HRESULT DisplayMessageAsync(Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, HSTRING, HSTRING, ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	HRESULT DisplayCredentialErrorAsync(long, long, unsigned int, HSTRING, HSTRING, ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	HRESULT DisplayStatusAsync(Windows::Internal::UI::Logon::Controller::LogonUIState, HSTRING, ABI::Windows::Foundation::IAsyncAction**);
	HRESULT TriggerLogonAnimationAsync(ABI::Windows::Foundation::IAsyncAction**);
	HRESULT ResetCredentials();
	HRESULT RestoreFromFirstSignInAnimation();
	HRESULT ClearUIState(HSTRING);
	HRESULT ShowSecurityOptionsAsync(Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::LogonUISecurityOptionsResult*>**);
	HRESULT get_WindowContainer(IInspectable**);
	HRESULT Hide();
	HRESULT Stop();

private:
	~ConsoleLogon();
	HRESULT CheckUIStarted();
	HRESULT Lock(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, bool, Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	Microsoft::WRL::Wrappers::SRWLock m_Lock;
	//class Microsoft::WRL::ComPtr<LogonViewManager> m_consoleUIManager;
};