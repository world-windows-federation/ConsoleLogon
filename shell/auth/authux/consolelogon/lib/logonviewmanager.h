#pragma once
#include "logoninterfaces.h"
#include "consoleuimanager.h"

class LogonViewManager 
	: public 
		Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>, 
		ConsoleUIManager, 
		ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::CredProvDataModel*, 
		Windows::Internal::UI::Logon::CredProvData::CredentialSerialization*>, 
		ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::CredProvDataModel*, 
		Windows::Internal::UI::Logon::CredProvData::BioFeedbackState>, 
		ABI::Windows::Foundation::ITypedEventHandler<IInspectable*, IInspectable*>, ABI::Windows::Foundation::Collections::VectorChangedEventHandler<IInspectable*>,
		ABI::Windows::Foundation::Collections::VectorChangedEventHandler<Windows::Internal::UI::Logon::CredProvData::Credential*>,
		ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::Credential*, IInspectable*>,
		INavigationCallback, 
		Microsoft::WRL::FtmBase>
{
public:
	LogonViewManager();
	HRESULT RuntimeClassInitialize();
	HRESULT Invoke(Windows::Internal::UI::Logon::CredProvData::ICredential*, IInspectable*);
	HRESULT Invoke(ABI::Windows::Foundation::Collections::IObservableVector<Windows::Internal::UI::Logon::CredProvData::Credential*>*, ABI::Windows::Foundation::Collections::IVectorChangedEventArgs*);
	HRESULT Invoke(ABI::Windows::Foundation::Collections::IObservableVector<IInspectable*>*, ABI::Windows::Foundation::Collections::IVectorChangedEventArgs*);
	HRESULT Invoke(IInspectable*, IInspectable*);
	HRESULT Invoke(Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel*, Windows::Internal::UI::Logon::CredProvData::BioFeedbackState);
	HRESULT Invoke(Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel*, Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization*);
	HRESULT OnNavigation();
	HRESULT ShowComboBox(Windows::Internal::UI::Logon::CredProvData::IComboBoxField*);
	HRESULT SetContext(IInspectable*, Windows::Internal::UI::Logon::Controller::IUserSettingManager*, Windows::Internal::UI::Logon::Controller::IRedirectionManager*, Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT Lock(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentials(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, Windows::Internal::UI::Logon::Controller::LogonUIFlags, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >);
	HRESULT ReportResult(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, LONG, LONG, HSTRING, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IReportCredentialsData> >);
	HRESULT ClearCredentialState();
	HRESULT DisplayStatus(Windows::Internal::UI::Logon::Controller::LogonUIState, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT DisplayMessage(Windows::Internal::UI::Logon::Controller::LogonMessageMode, UINT, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT DisplayCredentialError(LONG, LONG, UINT, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ShowSecurityOptions(Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult> >);
	HRESULT Cleanup(Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
private:
	HRESULT SetContextUIThread(IInspectable*, Windows::Internal::UI::Logon::Controller::IUserSettingManager*, Windows::Internal::UI::Logon::Controller::IRedirectionManager*, Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT LockUIThread(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentialsUIThread(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, Windows::Internal::UI::Logon::Controller::LogonUIFlags, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >);
	HRESULT ReportResultUIThread(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, LONG, LONG, HSTRING, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IReportCredentialsData> >);
	HRESULT ShowSecurityOptionsUIThread(Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult> >);
	HRESULT DisplayStatusUIThread(Windows::Internal::UI::Logon::Controller::LogonUIState, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT DisplayMessageUIThread(Windows::Internal::UI::Logon::Controller::LogonMessageMode, UINT, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT DisplayCredentialErrorUIThread(LONG, LONG, UINT, HSTRING, HSTRING, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ClearCredentialStateUIThread();
	HRESULT CleanupUIThread(Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT ShowCredentialView();
	HRESULT ShowUserSelection();
	HRESULT ShowCredProvSelection(Windows::Internal::UI::Logon::CredProvData::ICredentialGroup*, HSTRING);
	HRESULT ShowSelectedCredentialView(Windows::Internal::UI::Logon::CredProvData::ICredential*, HSTRING);
	HRESULT ShowStatusView(HSTRING);
	HRESULT ShowMessageView(HSTRING, HSTRING, UINT, Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ShowSerializationFailedView(HSTRING, HSTRING);
	HRESULT DestroyCurrentView();
	HRESULT StartCredProvsIfNecessary(Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char);
	HRESULT OnCredProvInitComplete();
	enum LogonView
	{
		None = 0,
		UserSelection = 1,
		CredProvSelection = 2,
		SelectedCredential = 3,
		Status = 4,
		Message = 5,
		ComboBox = 6,
		Locked = 7,
		SecurityOptions = 8,
		SerializationFailed = 9
	};

	LogonViewManager::LogonView m_currentViewType;
	Microsoft::WRL::ComPtr<IConsoleUIView> m_currentView;
	Microsoft::WRL::ComPtr<IInspectable> m_autoLogonManager;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IUserSettingManager> m_userSettingManager;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IRedirectionManager> m_redirectionManager;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider> m_displayStateProvider;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IBioFeedbackListener> m_bioFeedbackListener;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel> m_credProvDataModel;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredentialGroup> m_selectedGroup;
	EventRegistrationToken m_serializationCompleteToken;
	EventRegistrationToken m_bioFeedbackStateChangeToken;
	EventRegistrationToken m_usersChangedToken;
	EventRegistrationToken m_selectedUserChangeToken;
	EventRegistrationToken m_credentialsChangedToken;
	EventRegistrationToken m_selectedCredentialChangedToken;
	bool m_isCredentialResetRequired;
	bool m_credProvInitialized;
	bool m_showCredentialViewOnInitComplete;
	Windows::Internal::UI::Logon::Controller::LogonUIRequestReason m_currentReason;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IUnlockTrigger> m_unlockTrigger;
	wistd::unique_ptr<Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >, wistd::default_delete<Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> > > > m_requestCredentialsComplete;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::IReportResultInfo> m_lastReportResultInfo;
	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization> m_cachedSerialization;
	Microsoft::WRL::ComPtr<IInputSwitchControl> m_inputSwitchControl;
};