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

	HRESULT SetContext(
		IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
		LC::IRedirectionManager* redirectionManager, LCPD::IDisplayStateProvider* displayStateProvider,
		LC::IBioFeedbackListener* bioFeedbackListener);
	HRESULT Lock(LC::LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, LC::IUnlockTrigger* unlockTrigger);
	HRESULT RequestCredentials(
		LC::LogonUIRequestReason reason, LC::LogonUIFlags flags,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> completion);
	HRESULT ReportResult(
		LC::LogonUIRequestReason reason, NTSTATUS ntStatus, NTSTATUS ntSubStatus, HSTRING samCompatibleUserName,
		HSTRING displayName, HSTRING userSid,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completion);
	HRESULT ClearCredentialState();
	HRESULT DisplayStatus(LC::LogonUIState state, HSTRING status, WI::AsyncDeferral<WI::CNoResult> completion);
	HRESULT DisplayMessage(
		LC::LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);
	HRESULT DisplayCredentialError(
		NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);
	HRESULT ShowSecurityOptions(
		LC::LogonUISecurityOptions options,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);
	HRESULT Cleanup(WI::AsyncDeferral<WI::CNoResult> completion);

private:
	HRESULT SetContextUIThread(
		IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
		LC::IRedirectionManager* redirectionManager, LCPD::IDisplayStateProvider* displayStateProvider,
		LC::IBioFeedbackListener* bioFeedbackListener);
	HRESULT LockUIThread(
		LC::LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, LC::IUnlockTrigger* unlockTrigger);
	HRESULT RequestCredentialsUIThread(
		LC::LogonUIRequestReason reason, LC::LogonUIFlags flags,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> completion);
	HRESULT ReportResultUIThread(
		LC::LogonUIRequestReason reason, NTSTATUS ntStatus, NTSTATUS ntSubStatus, HSTRING samCompatibleUserName,
		HSTRING displayName, HSTRING userSid,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completion);
	HRESULT ShowSecurityOptionsUIThread(
		LC::LogonUISecurityOptions options,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion);
	HRESULT DisplayStatusUIThread(LC::LogonUIState state, HSTRING status, WI::AsyncDeferral<WI::CNoResult> completion);
	HRESULT DisplayMessageUIThread(
		LC::LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);
	HRESULT DisplayCredentialErrorUIThread(
		NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);
	HRESULT ClearCredentialStateUIThread();
	HRESULT CleanupUIThread(WI::AsyncDeferral<WI::CNoResult> completion);
	HRESULT ShowCredentialView();
	HRESULT ShowUserSelection();
	HRESULT ShowCredProvSelection(LCPD::ICredentialGroup*, HSTRING);
	HRESULT ShowSelectedCredentialView(LCPD::ICredential*, HSTRING);
	HRESULT ShowStatusView(HSTRING);
	HRESULT ShowMessageView(
		HSTRING caption, HSTRING message, UINT messageBoxFlags,
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion);
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
