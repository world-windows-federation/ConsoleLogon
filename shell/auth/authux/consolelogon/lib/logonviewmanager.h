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
		enum Windows::Internal::UI::Logon::CredProvData::BioFeedbackState>, 
		ABI::Windows::Foundation::ITypedEventHandler<IInspectable*, IInspectable*>, Windows::Foundation::Collections::VectorChangedEventHandler<IInspectable*>,
		ABI::Windows::Foundation::Collections::VectorChangedEventHandler<Windows::Internal::UI::Logon::CredProvData::Credential*>,
		ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::Credential*, IInspectable*>,
		INavigationCallback, 
		Microsoft::WRL::FtmBase>
{
public:
	LogonViewManager();
	HRESULT RuntimeClassInitialize();
	HRESULT Invoke(struct Windows::Internal::UI::Logon::CredProvData::ICredential*, struct IInspectable*);
	HRESULT Invoke(struct Windows::Foundation::Collections::IObservableVector<Windows::Internal::UI::Logon::CredProvData::Credential*>*, struct Windows::Foundation::Collections::IVectorChangedEventArgs*);
	HRESULT Invoke(struct Windows::Foundation::Collections::IObservableVector<IInspectable*>*, struct Windows::Foundation::Collections::IVectorChangedEventArgs*);
	HRESULT Invoke(struct IInspectable*, struct IInspectable*);
	HRESULT Invoke(struct Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel*, enum Windows::Internal::UI::Logon::CredProvData::BioFeedbackState);
	HRESULT Invoke(struct Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel*, struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization*);
	HRESULT OnNavigation();
	HRESULT ShowComboBox(struct Windows::Internal::UI::Logon::CredProvData::IComboBoxField*);
	HRESULT SetContext(struct IInspectable*, struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*, struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*, struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT Lock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentials(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, enum Windows::Internal::UI::Logon::Controller::LogonUIFlags, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >);
	HRESULT ReportResult(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, struct HSTRING__*, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IReportCredentialsData> >);
	HRESULT ClearCredentialState();
	HRESULT DisplayStatus(enum Windows::Internal::UI::Logon::Controller::LogonUIState, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT DisplayMessage(enum Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT DisplayCredentialError(long, long, unsigned int, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ShowSecurityOptions(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult> >);
	HRESULT Cleanup(class Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
private:
	HRESULT SetContextUIThread(struct IInspectable*, struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*, struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*, struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
	HRESULT LockUIThread(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
	HRESULT RequestCredentialsUIThread(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, enum Windows::Internal::UI::Logon::Controller::LogonUIFlags, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >);
	HRESULT ReportResultUIThread(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, struct HSTRING__*, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IReportCredentialsData> >);
	HRESULT ShowSecurityOptionsUIThread(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult> >);
	HRESULT DisplayStatusUIThread(enum Windows::Internal::UI::Logon::Controller::LogonUIState, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT DisplayMessageUIThread(enum Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT DisplayCredentialErrorUIThread(long, long, unsigned int, struct HSTRING__*, struct HSTRING__*, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ClearCredentialStateUIThread();
	HRESULT CleanupUIThread(class Windows::Internal::AsyncDeferral<Windows::Internal::CNoResult>);
	HRESULT ShowCredentialView();
	HRESULT ShowUserSelection();
	HRESULT ShowCredProvSelection(struct Windows::Internal::UI::Logon::CredProvData::ICredentialGroup*, struct HSTRING__*);
	HRESULT ShowSelectedCredentialView(struct Windows::Internal::UI::Logon::CredProvData::ICredential*, struct HSTRING__*);
	HRESULT ShowStatusView(struct HSTRING__*);
	HRESULT ShowMessageView(struct HSTRING__*, struct HSTRING__*, unsigned int, class Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IMessageDisplayResult> >);
	HRESULT ShowSerializationFailedView(struct HSTRING__*, struct HSTRING__*);
	HRESULT DestroyCurrentView();
	HRESULT StartCredProvsIfNecessary(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char);
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

	enum LogonViewManager::LogonView m_currentViewType;
	class Microsoft::WRL::ComPtr<IConsoleUIView> m_currentView;
	class Microsoft::WRL::ComPtr<IInspectable> m_autoLogonManager;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IUserSettingManager> m_userSettingManager;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IRedirectionManager> m_redirectionManager;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider> m_displayStateProvider;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IBioFeedbackListener> m_bioFeedbackListener;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredProvDataModel> m_credProvDataModel;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredentialGroup> m_selectedGroup;
	struct EventRegistrationToken m_serializationCompleteToken;
	struct EventRegistrationToken m_bioFeedbackStateChangeToken;
	struct EventRegistrationToken m_usersChangedToken;
	struct EventRegistrationToken m_selectedUserChangeToken;
	struct EventRegistrationToken m_credentialsChangedToken;
	struct EventRegistrationToken m_selectedCredentialChangedToken;
	bool m_isCredentialResetRequired;
	bool m_credProvInitialized;
	bool m_showCredentialViewOnInitComplete;
	enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason m_currentReason;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::Controller::IUnlockTrigger> m_unlockTrigger;
	class wistd::unique_ptr<Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> >, wistd::default_delete<Windows::Internal::AsyncDeferral<Windows::Internal::CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData> > > > m_requestCredentialsComplete;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::IReportResultInfo> m_lastReportResultInfo;
	class Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization> m_cachedSerialization;
	class Microsoft::WRL::ComPtr<IInputSwitchControl> m_inputSwitchControl;
};