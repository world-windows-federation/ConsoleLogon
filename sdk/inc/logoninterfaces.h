#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include <inspectable.h>
#include <windows.foundation.collections.h>
#include <windows.foundation.numerics.h>

namespace Windows::Internal::UI::Logon
{
	namespace CredProvData
	{
		enum class UserAccountKind
		{
			Unknown = 0,
			Local = 1,
			Domain = 2,
			Connected = 3,
			CloudDomain = 4
		};

		struct ITelemetryDataProvider : public IInspectable
		{
			HRESULT GetUserAccountKind(struct HSTRING__*, enum Windows::Internal::UI::Logon::CredProvData::UserAccountKind*);
			HRESULT get_CurrentLogonUIRequestReason(unsigned long*);
			ITelemetryDataProvider(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider*);
			ITelemetryDataProvider(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider&);
			ITelemetryDataProvider();
			struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider& operator=(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider*);
			struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider& operator=(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider&);
		};

		enum class DisplayStateFlags
		{
			None = 0,
			DisplayOn = 1,
			DisplayDim = 2,
			LockScreenVisible = 4,
			LockScreenManipulated = 8,
			AppOnLockScreen = 16,
			UserSwitching = 32,
			SessionLocked = 64,
			AcceptingCreds = 128,
			LidClosed = 256,
			DelayLocked = 512,
			LoggedOut = 1024,
			ResumeFromHibernateOrScreenSaver = 2048
		};

		struct IDisplayStateProvider : public IInspectable
		{
			HRESULT get_DisplayState(enum Windows::Internal::UI::Logon::CredProvData::DisplayStateFlags*);
			HRESULT add_DisplayStateChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, enum Windows::Internal::UI::Logon::CredProvData::DisplayStateFlags>*, struct EventRegistrationToken*);
			HRESULT remove_DisplayStateChanged(struct EventRegistrationToken);
			IDisplayStateProvider(struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*);
			IDisplayStateProvider(struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider&);
			IDisplayStateProvider();
			struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider& operator=(struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*);
			struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider& operator=(struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider&);
		};

		enum class BioFeedbackState
		{
			None = 0,
			Error = 1,
			PowerOn = 2,
			Searching = 3,
			Authenticated = 4,
			TooFar = 5,
			TooClose = 6,
			MessageOnly = 7
		};

		enum class SerializationResponse
		{
			NoCredentialIncomplete = 0,
			NoCredentialComplete = 1,
			ReturnCredentialComplete = 2,
			ReturnNoCredentialComplete = 3
		};

		enum class CredentialProviderStatusIcon
		{
			None = 0,
			Error = 1,
			Warning = 2,
			Success = 3
		};

		struct ICredentialSerialization : public IInspectable
		{
			HRESULT get_CredentialId(unsigned int*);
			HRESULT get_SerializationResponse(enum Windows::Internal::UI::Logon::CredProvData::SerializationResponse*);
			HRESULT get_SerializationIcon(enum Windows::Internal::UI::Logon::CredProvData::CredentialProviderStatusIcon*);
			HRESULT get_StatusMessage(struct HSTRING__**);
			ICredentialSerialization(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization*);
			ICredentialSerialization(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization&);
			ICredentialSerialization();
			struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization& operator=(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization*);
			struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization& operator=(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization&);
		};
	}

	namespace Controller
	{
		enum class LogonErrorRedirectorResponse
		{
			Invalid = 0,
			NotHandled = 1,
			HandledShowLocally = 2,
			HandledDoNotShowLocally = 3,
			HandledDoNotShowLocallyStartOver = 4
		};

		enum class LogonUIRequestReason
		{
			LogonUILogon = 0,
			LogonUIUnlock = 1,
			LogonUIChange = 2,
			LogonUISessionRecovery = 3,
			LogonUIAlternate = 4,
			CredUI = 5
		};

		struct IRedirectionManager : public IInspectable
		{
			HRESULT OnBeginPainting();
			HRESULT RedirectStatus(struct HSTRING__*, Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*);
			HRESULT RedirectMessage(struct HSTRING__*, struct HSTRING__*, unsigned int, unsigned int*, enum Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*);
			HRESULT RedirectLogonError(long, long, struct HSTRING__*, struct HSTRING__*, unsigned int, unsigned int*, enum Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*);
			HRESULT get_IsRedirectMode(unsigned char*);
			IRedirectionManager(struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*);
			IRedirectionManager(struct Windows::Internal::UI::Logon::Controller::IRedirectionManager&);
			IRedirectionManager();
			struct Windows::Internal::UI::Logon::Controller::IRedirectionManager& operator=(struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*);
			struct Windows::Internal::UI::Logon::Controller::IRedirectionManager& operator=(struct Windows::Internal::UI::Logon::Controller::IRedirectionManager&);
		};

		enum class UserTheme
		{
			Dark = 0,
			HCDark = 1,
			Light = 2,
			HCLight = 3
		};

		enum class EmbeddedSKUPolicyFlags
		{
			DisableAll = 0,
			EnableShutdownOption = 1,
			EnableLanguageOption = 2,
			EnableEaseOfAccessOption = 4,
			EnableBackButton = 16,
			EnableBSDR = 32,
			EnableAll = 63
		};

		struct IUserSettingManager : public IInspectable
		{
			HRESULT IsUserSwitchingAllowed(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char*);
			HRESULT CancelUserSwitch();
			HRESULT get_IsLoggedOnUserSidPresent(unsigned char*);
			HRESULT get_LangID(unsigned short*);
			HRESULT get_UserSid(struct HSTRING__**);
			HRESULT put_UserSid(struct HSTRING__*);
			HRESULT get_CaretWidth(int*);
			HRESULT get_LastLoggedOnUserSid(struct HSTRING__**);
			HRESULT get_TelemetryDataProvider(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider**);
			HRESULT get_IsLockScreenAllowed(unsigned char*);
			HRESULT put_IsLockScreenAllowed(unsigned char);
			HRESULT get_UserTheme(enum Windows::Internal::UI::Logon::Controller::UserTheme*);
			HRESULT get_IsLowMemoryDevice(unsigned char*);
			HRESULT get_IsFirstLogonAfterSignOutOrSwitchUser(unsigned char*);
			HRESULT get_EmbeddedSKUPolicy(enum Windows::Internal::UI::Logon::Controller::EmbeddedSKUPolicyFlags*);
			HRESULT get_ShowAccentColorInsteadOfLogonBackgroundImage(unsigned char*);
			HRESULT get_IsFirstBoot(unsigned char*);
			HRESULT get_ShouldLaunchFirstLogonAnimation(unsigned char*);
			HRESULT get_IsOobeZDPRebootRequired(unsigned char*);
			HRESULT get_ShouldLaunchFirstSignInAnimationInUserSession(unsigned char*);
			HRESULT put_ShouldLaunchFirstSignInAnimationInUserSession(unsigned char);
			HRESULT get_IsAudioHIDEnabled(unsigned char*);
			HRESULT add_SessionDisconnected(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*);
			HRESULT remove_SessionDisconnected(struct EventRegistrationToken);
			HRESULT add_ColorSetChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*);
			HRESULT remove_ColorSetChanged(struct EventRegistrationToken);
			HRESULT add_HighContrastChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*);
			HRESULT remove_HighContrastChanged(struct EventRegistrationToken);
			HRESULT add_CaretWidthChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*);
			HRESULT remove_CaretWidthChanged(struct EventRegistrationToken);
			HRESULT add_SlideToShutdownDetected(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*);
			HRESULT remove_SlideToShutdownDetected(struct EventRegistrationToken);
			IUserSettingManager(struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*);
			IUserSettingManager(struct Windows::Internal::UI::Logon::Controller::IUserSettingManager&);
			IUserSettingManager();
			struct Windows::Internal::UI::Logon::Controller::IUserSettingManager& operator=(struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*);
			struct Windows::Internal::UI::Logon::Controller::IUserSettingManager& operator=(struct Windows::Internal::UI::Logon::Controller::IUserSettingManager&);
		};

		struct IUnlockTrigger : public IInspectable
		{
			HRESULT TriggerUnlock();
			HRESULT SyncBackstop();
			HRESULT CheckCompletion();
			IUnlockTrigger(struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
			IUnlockTrigger(struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger&);
			IUnlockTrigger();
			struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger& operator=(struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*);
			struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger& operator=(struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger&);
		};

		enum class LogonUIFlags
		{
			None = 0,
			SecureGestureDisabled = 1,
			PasswordExpired = 2,
			AllowDirectUserSwitching = 4
		};

		struct IBioFeedbackListener : public IInspectable
		{
			HRESULT OnBioFeedbackUpdate(enum Windows::Internal::UI::Logon::CredProvData::BioFeedbackState, struct HSTRING__*);
			IBioFeedbackListener(struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
			IBioFeedbackListener(struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener&);
			IBioFeedbackListener();
			struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener& operator=(struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*);
			struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener& operator=(struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener&);
		};

		enum class LogonMessageMode
		{
			None = 0,
			Message = 1,
			LogonError = 2,
			NoCredentialNotFinished = 3,
			NoCredentialFinished = 4,
			EmergencyRestart = 5,
			TerminalServiceDisconnectOptions = 6,
			CredentialConnecting = 7
		};

		enum class LogonUIState
		{
			Start = 0,
			Welcome = 1,
			Logon = 2,
			Authenticate = 3,
			LoggingOn = 4,
			LoggingOff = 5,
			StandingBy = 6,
			ShuttingDown = 7,
			Restarting = 8
		};

		enum class LogonUISecurityOptions
		{
			Cancel = 0,
			Lock = 1,
			LogOff = 2,
			ChangePassword = 4,
			TaskManager = 8,
			SwitchUser = 16
		};

		struct IRequestCredentialsData : public IInspectable
		{
			HRESULT get_Credential(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization**);
			HRESULT get_SaveCredential(unsigned char*);
			//IRequestCredentialsData(struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData*);
			//IRequestCredentialsData(struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData&);
			//IRequestCredentialsData();
			//struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData& operator=(struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData*);
			//struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData& operator=(struct Windows::Internal::UI::Credentials::Controller::IRequestCredentialsData&);
		};

		class RequestCredentialsData : public IRequestCredentialsData
		{

		};

		enum class LogonUICredProvResponse
		{
			LogonUIResponseDefault = 0,
			LogonUIResponseRetry = 1,
			LogonUIResponseAbort = 2
		};

		struct IReportCredentialsData : IInspectable
		{
			HRESULT get_Message(HSTRING__**);
			HRESULT get_Response(Windows::Internal::UI::Logon::Controller::LogonUICredProvResponse*);
		};


		class ReportCredentialsData : public IReportCredentialsData
		{
		};

		struct IMessageDisplayResult : public IInspectable
		{
			HRESULT get_ResultCode(unsigned int*);
		};

		class MessageDisplayResult : public IMessageDisplayResult
		{
		};

		enum class LogonUIShutdownChoice
		{
			None = 0,
			TurnOff = 1,
			Restart = 2,
			StandBy = 4,
			AdvStandBy = 8,
			Hibernate = 16,
			EmergencyRestart = 32,
			ForceCurrentSession = 64,
			ForceOtherSessions = 128,
			InstallUpdates = 256,
			HybridMode = 512,
			BootOptions = 1024,
			Disconnect = 2048
		};

		struct ILogonUISecurityOptionsResult : public IInspectable
		{
			HRESULT get_SecurityOptionChoice(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions*);
			HRESULT get_ShutdownChoice(enum Windows::Internal::UI::Logon::Controller::LogonUIShutdownChoice*);

			ILogonUISecurityOptionsResult(struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult*);
			ILogonUISecurityOptionsResult(struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult&);
			ILogonUISecurityOptionsResult();
			struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult& operator=(struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult*);
			struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult& operator=(struct Windows::Internal::UI::Logon::Controller::ILogonUISecurityOptionsResult&);
		};

		class LogonUISecurityOptionsResult : ILogonUISecurityOptionsResult
		{
		};

		struct ILogonUX : public IInspectable
		{
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
			HRESULT ClearUIState(struct HSTRING__*);
			HRESULT RestoreFromFirstSignInAnimation();
			HRESULT ShowSecurityOptionsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::LogonUISecurityOptionsResult*>**);
			HRESULT get_WindowContainer(struct IInspectable**);
			HRESULT Hide();
			HRESULT Stop();
			ILogonUX(struct Windows::Internal::UI::Logon::Controller::ILogonUX*);
			ILogonUX(struct Windows::Internal::UI::Logon::Controller::ILogonUX&);
			ILogonUX();
			struct Windows::Internal::UI::Logon::Controller::ILogonUX& operator=(struct Windows::Internal::UI::Logon::Controller::ILogonUX*);
			struct Windows::Internal::UI::Logon::Controller::ILogonUX& operator=(struct Windows::Internal::UI::Logon::Controller::ILogonUX&);
		};
	}
	
}


