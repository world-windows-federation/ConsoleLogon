#pragma once
#include "InternalAsync.h"
#include "logoninterfaces.h"
#include "consoleuimanager.h"

class LogonViewManager final
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, ConsoleUIManager
		, WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::CredentialSerialization*>
		, WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::BioFeedbackState>
		, WF::ITypedEventHandler<IInspectable*, IInspectable*>
		, WFC::VectorChangedEventHandler<IInspectable*>
		, WFC::VectorChangedEventHandler<LCPD::Credential*>
		, WF::ITypedEventHandler<LCPD::Credential*, IInspectable*>
		, INavigationCallback
		, Microsoft::WRL::FtmBase
	>
{
public:
	LogonViewManager();

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize();

	//~ Begin WF::ITypedEventHandler<LCPD::Credential*, IInspectable*> Interface
	STDMETHODIMP Invoke(LCPD::ICredential* sender, IInspectable* args) override;
	//~ End WF::ITypedEventHandler<LCPD::Credential*, IInspectable*> Interface

	//~ Begin WFC::VectorChangedEventHandler<LCPD::Credential*> Interface
	STDMETHODIMP Invoke(WFC::IObservableVector<LCPD::Credential*>* sender, WFC::IVectorChangedEventArgs* args) override;
	//~ End WFC::VectorChangedEventHandler<LCPD::Credential*> Interface

	//~ Begin WFC::VectorChangedEventHandler<IInspectable*> Interface
	STDMETHODIMP Invoke(WFC::IObservableVector<IInspectable*>* sender, WFC::IVectorChangedEventArgs* args) override;
	//~ End WFC::VectorChangedEventHandler<IInspectable*> Interface

	//~ Begin WF::ITypedEventHandler<IInspectable*, IInspectable*> Interface
	STDMETHODIMP Invoke(IInspectable* sender, IInspectable* args) override;
	//~ End WF::ITypedEventHandler<IInspectable*, IInspectable*> Interface

	//~ Begin WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::BioFeedbackState> Interface
	STDMETHODIMP Invoke(LCPD::ICredProvDataModel* sender, LCPD::BioFeedbackState args) override;
	//~ End WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::BioFeedbackState> Interface

	//~ Begin WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::CredentialSerialization*> Interface
	STDMETHODIMP Invoke(LCPD::ICredProvDataModel* sender, LCPD::ICredentialSerialization* args) override;
	//~ End WF::ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::CredentialSerialization*> Interface

	//~ Begin INavigationCallback Interface
	STDMETHODIMP OnNavigation() override;
	STDMETHODIMP ShowComboBox(LCPD::IComboBoxField* dataSource) override;
	//~ End INavigationCallback Interface

	HRESULT SetContext(IInspectable*, LC::IUserSettingManager*, LC::IRedirectionManager*, LCPD::IDisplayStateProvider*, LC::IBioFeedbackListener*);
	HRESULT Lock(LC::LogonUIRequestReason, BOOLEAN, LC::IUnlockTrigger*);
	HRESULT RequestCredentials(LC::LogonUIRequestReason, LC::LogonUIFlags, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>>);
	HRESULT ReportResult(LC::LogonUIRequestReason, NTSTATUS, NTSTATUS, HSTRING, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>>);
	HRESULT ClearCredentialState();
	HRESULT DisplayStatus(LC::LogonUIState, HSTRING, WI::AsyncDeferral<WI::CNoResult>);
	HRESULT DisplayMessage(LC::LogonMessageMode, UINT, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>);
	HRESULT DisplayCredentialError(NTSTATUS, NTSTATUS, UINT, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>);
	HRESULT ShowSecurityOptions(LC::LogonUISecurityOptions, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>);
	HRESULT Cleanup(WI::AsyncDeferral<WI::CNoResult>);

private:
	HRESULT SetContextUIThread(
		IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
		LC::IRedirectionManager* redirectionManager, LCPD::IDisplayStateProvider* displayStateProvider,
		LC::IBioFeedbackListener* bioFeedbackListener);
	HRESULT LockUIThread(LC::LogonUIRequestReason, BOOLEAN, LC::IUnlockTrigger*);
	HRESULT RequestCredentialsUIThread(LC::LogonUIRequestReason, LC::LogonUIFlags, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>>);
	HRESULT ReportResultUIThread(LC::LogonUIRequestReason, NTSTATUS, NTSTATUS, HSTRING, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>>);
	HRESULT ShowSecurityOptionsUIThread(LC::LogonUISecurityOptions, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>>);
	HRESULT DisplayStatusUIThread(LC::LogonUIState, HSTRING, WI::AsyncDeferral<WI::CNoResult>);
	HRESULT DisplayMessageUIThread(LC::LogonMessageMode, UINT, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>);
	HRESULT DisplayCredentialErrorUIThread(NTSTATUS, NTSTATUS, UINT, HSTRING, HSTRING, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>);
	HRESULT ClearCredentialStateUIThread();
	HRESULT CleanupUIThread(WI::AsyncDeferral<WI::CNoResult>);
	HRESULT ShowCredentialView();
	HRESULT ShowUserSelection();
	HRESULT ShowCredProvSelection(LCPD::ICredentialGroup*, HSTRING);
	HRESULT ShowSelectedCredentialView(LCPD::ICredential*, HSTRING);
	HRESULT ShowStatusView(HSTRING);
	HRESULT ShowMessageView(HSTRING, HSTRING, UINT, WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>>);
	HRESULT ShowSerializationFailedView(HSTRING, HSTRING);
	HRESULT DestroyCurrentView();
	HRESULT StartCredProvsIfNecessary(LC::LogonUIRequestReason, BOOLEAN);
	HRESULT OnCredProvInitComplete();

	enum class LogonView
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
		SerializationFailed = 9,
	};

	LogonView m_currentViewType;
	Microsoft::WRL::ComPtr<IConsoleUIView> m_currentView;
	Microsoft::WRL::ComPtr<IInspectable> m_autoLogonManager;
	Microsoft::WRL::ComPtr<LC::IUserSettingManager> m_userSettingManager;
	Microsoft::WRL::ComPtr<LC::IRedirectionManager> m_redirectionManager;
	Microsoft::WRL::ComPtr<LCPD::IDisplayStateProvider> m_displayStateProvider;
	Microsoft::WRL::ComPtr<LC::IBioFeedbackListener> m_bioFeedbackListener;
	Microsoft::WRL::ComPtr<LCPD::ICredProvDataModel> m_credProvDataModel;
	Microsoft::WRL::ComPtr<LCPD::ICredentialGroup> m_selectedGroup;
	EventRegistrationToken m_serializationCompleteToken;
	EventRegistrationToken m_bioFeedbackStateChangeToken;
	EventRegistrationToken m_usersChangedToken;
	EventRegistrationToken m_selectedUserChangeToken;
	EventRegistrationToken m_credentialsChangedToken;
	EventRegistrationToken m_selectedCredentialChangedToken;
	bool m_isCredentialResetRequired;
	bool m_credProvInitialized;
	bool m_showCredentialViewOnInitComplete;
	LC::LogonUIRequestReason m_currentReason;
	Microsoft::WRL::ComPtr<LC::IUnlockTrigger> m_unlockTrigger;
	wistd::unique_ptr<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>>> m_requestCredentialsComplete;
	Microsoft::WRL::ComPtr<LCPD::IReportResultInfo> m_lastReportResultInfo;
	Microsoft::WRL::ComPtr<LCPD::ICredentialSerialization> m_cachedSerialization;
	Microsoft::WRL::ComPtr<IInputSwitchControl> m_inputSwitchControl;
};
