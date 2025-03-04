#include "../dll/pch.h"
#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

class ConsoleLogon 
	: public Microsoft::WRL::RuntimeClass<
		Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>, 
		Windows::Internal::UI::Logon::Controller::ILogonUX, 
		Microsoft::WRL::FtmBase, Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil, 
		Microsoft::WRL::Details::Nil>
{
	public wchar_t* InternalGetRuntimeClassName();
	public enum TrustLevel InternalGetTrustLevel();
	public HRESULT GetRuntimeClassName(struct HSTRING__**);
	public HRESULT GetTrustLevel(enum TrustLevel*);
	public HRESULT GetIids(unsigned long*, struct _GUID**);
	public HRESULT QueryInterface(struct _GUID&, void**);
	public unsigned long Release();
	public unsigned long AddRef();
	public void ConsoleLogon();
	public HRESULT RuntimeClassInitialize();
	public HRESULT Start(struct IInspectable*, struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*, struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*, struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	public HRESULT DelayLock(unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	public HRESULT HardLock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	public HRESULT RequestCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, enum Windows::Internal::UI::Logon::Controller::LogonUIFlags, struct Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::RequestCredentialsData*>**);
	public HRESULT ReportCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, struct HSTRING__*, struct HSTRING__*, struct HSTRING__*, struct Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::ReportCredentialsData*>**);
	public HRESULT DisplayMessageAsync(enum Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, struct HSTRING__*, struct HSTRING__*, struct Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	public HRESULT DisplayCredentialErrorAsync(long, long, unsigned int, struct HSTRING__*, struct HSTRING__*, struct Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**);
	public HRESULT DisplayStatusAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIState, struct HSTRING__*, struct Windows::Foundation::IAsyncAction**);
	public HRESULT TriggerLogonAnimationAsync(struct Windows::Foundation::IAsyncAction**);
	public HRESULT ResetCredentials();
	public HRESULT RestoreFromFirstSignInAnimation();
	public HRESULT ClearUIState(struct HSTRING__*);
	public HRESULT ShowSecurityOptionsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, struct Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::LogonUISecurityOptionsResult*>**);
	public HRESULT get_WindowContainer(struct IInspectable**);
	public HRESULT Hide();
	public HRESULT Stop();
	private void ~ConsoleLogon();
	private HRESULT CheckUIStarted();
	private HRESULT Lock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	private class Microsoft::WRL::Wrappers::SRWLock m_Lock;
	private class Microsoft::WRL::ComPtr<LogonViewManager> m_consoleUIManager;
	public void __local_vftable_ctor_closure();
	private void* __vecDelDtor(unsigned int);
};