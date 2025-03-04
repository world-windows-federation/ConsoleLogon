#include "../dll/pch.h"
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>
#include "../../../../../sdk/inc/logoninterfaces.h"

class DECLSPEC_UUID("00000000-0000-0000-0000-000000000000")
ConsoleLogon 
	: public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>, 
		Windows::Internal::UI::Logon::Controller::ILogonUX, 
		Microsoft::WRL::FtmBase>
{
public:
	wchar_t* InternalGetRuntimeClassName();
	enum TrustLevel InternalGetTrustLevel();
	HRESULT GetRuntimeClassName(struct HSTRING__**);
	HRESULT GetTrustLevel(enum TrustLevel*);
	HRESULT GetIids(unsigned long*, struct _GUID**);
	HRESULT QueryInterface(struct _GUID&, void**);
	unsigned long Release();
	unsigned long AddRef();
	ConsoleLogon();
	HRESULT RuntimeClassInitialize();
	HRESULT Start(struct IInspectable*, struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*, struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*, struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT DelayLock(unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT HardLock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, enum Windows::Internal::UI::Logon::Controller::LogonUIFlags, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::RequestCredentialsData*>**);
	HRESULT ReportCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, struct HSTRING__*, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::ReportCredentialsData*>**);
	HRESULT DisplayMessageAsync(enum Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	HRESULT DisplayCredentialErrorAsync(long, long, unsigned int, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	HRESULT DisplayStatusAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIState, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncAction**);
	HRESULT TriggerLogonAnimationAsync(struct ABI::Windows::Foundation::IAsyncAction**);
	HRESULT ResetCredentials();
	HRESULT RestoreFromFirstSignInAnimation();
	HRESULT ClearUIState(struct HSTRING__*);
	HRESULT ShowSecurityOptionsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::LogonUISecurityOptionsResult*>**);
	HRESULT get_WindowContainer(struct IInspectable**);
	HRESULT Hide();
	HRESULT Stop();
private:
	~ConsoleLogon();
	HRESULT CheckUIStarted();
	HRESULT Lock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	class Microsoft::WRL::Wrappers::SRWLock m_Lock;
	//class Microsoft::WRL::ComPtr<LogonViewManager> m_consoleUIManager;
};