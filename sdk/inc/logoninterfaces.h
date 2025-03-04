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
			virtual HRESULT STDMETHODCALLTYPE GetUserAccountKind(struct HSTRING__*, enum Windows::Internal::UI::Logon::CredProvData::UserAccountKind*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CurrentLogonUIRequestReason(unsigned long*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_DisplayState(enum Windows::Internal::UI::Logon::CredProvData::DisplayStateFlags*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_DisplayStateChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, enum Windows::Internal::UI::Logon::CredProvData::DisplayStateFlags>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_DisplayStateChanged(struct EventRegistrationToken) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_CredentialId(unsigned int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationResponse(enum Windows::Internal::UI::Logon::CredProvData::SerializationResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationIcon(enum Windows::Internal::UI::Logon::CredProvData::CredentialProviderStatusIcon*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusMessage(struct HSTRING__**) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE OnBeginPainting() PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectStatus(struct HSTRING__*, Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectMessage(struct HSTRING__*, struct HSTRING__*, unsigned int, unsigned int*, enum Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectLogonError(long, long, struct HSTRING__*, struct HSTRING__*, unsigned int, unsigned int*, enum Windows::Internal::UI::Logon::Controller::LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsRedirectMode(unsigned char*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE IsUserSwitchingAllowed(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE CancelUserSwitch() PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLoggedOnUserSidPresent(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LangID(unsigned short*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UserSid(struct HSTRING__**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_UserSid(struct HSTRING__*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CaretWidth(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LastLoggedOnUserSid(struct HSTRING__**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TelemetryDataProvider(struct Windows::Internal::UI::Logon::CredProvData::ITelemetryDataProvider**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLockScreenAllowed(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_IsLockScreenAllowed(unsigned char) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UserTheme(enum Windows::Internal::UI::Logon::Controller::UserTheme*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLowMemoryDevice(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsFirstLogonAfterSignOutOrSwitchUser(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_EmbeddedSKUPolicy(enum Windows::Internal::UI::Logon::Controller::EmbeddedSKUPolicyFlags*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShowAccentColorInsteadOfLogonBackgroundImage(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsFirstBoot(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldLaunchFirstLogonAnimation(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsOobeZDPRebootRequired(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldLaunchFirstSignInAnimationInUserSession(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_ShouldLaunchFirstSignInAnimationInUserSession(unsigned char) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsAudioHIDEnabled(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SessionDisconnected(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SessionDisconnected(struct EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_ColorSetChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_ColorSetChanged(struct EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_HighContrastChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_HighContrastChanged(struct EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_CaretWidthChanged(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_CaretWidthChanged(struct EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SlideToShutdownDetected(struct ABI::Windows::Foundation::ITypedEventHandler<Windows::Internal::UI::Logon::Controller::IUserSettingManager*, IInspectable*>*, struct EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SlideToShutdownDetected(struct EventRegistrationToken) PURE;
		};

		struct IUnlockTrigger : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE TriggerUnlock() PURE;
			virtual HRESULT STDMETHODCALLTYPE SyncBackstop() PURE;
			virtual HRESULT STDMETHODCALLTYPE CheckCompletion() PURE;
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
			virtual HRESULT STDMETHODCALLTYPE OnBioFeedbackUpdate(enum Windows::Internal::UI::Logon::CredProvData::BioFeedbackState, struct HSTRING__*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_Credential(struct Windows::Internal::UI::Logon::CredProvData::ICredentialSerialization**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SaveCredential(unsigned char*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_Message(HSTRING__**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Response(Windows::Internal::UI::Logon::Controller::LogonUICredProvResponse*) PURE;
		};


		class ReportCredentialsData : public IReportCredentialsData
		{
		};

		struct IMessageDisplayResult : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_ResultCode(unsigned int*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_SecurityOptionChoice(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShutdownChoice(enum Windows::Internal::UI::Logon::Controller::LogonUIShutdownChoice*) PURE;
		};

		class LogonUISecurityOptionsResult : ILogonUISecurityOptionsResult
		{
		};

		MIDL_INTERFACE("fbc9fd2c-9b7e-4ed1-9b60-61cf17f4ec4c")
		ILogonUX : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE Start(struct IInspectable*, struct Windows::Internal::UI::Logon::Controller::IRedirectionManager*, struct Windows::Internal::UI::Logon::Controller::IUserSettingManager*, struct Windows::Internal::UI::Logon::CredProvData::IDisplayStateProvider*, struct Windows::Internal::UI::Logon::Controller::IBioFeedbackListener*) PURE;
			virtual HRESULT STDMETHODCALLTYPE DelayLock(unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*) PURE;
			virtual HRESULT STDMETHODCALLTYPE HardLock(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, unsigned char, struct Windows::Internal::UI::Logon::Controller::IUnlockTrigger*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RequestCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, enum Windows::Internal::UI::Logon::Controller::LogonUIFlags, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::RequestCredentialsData*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ReportCredentialsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIRequestReason, long, long, struct HSTRING__*, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::ReportCredentialsData*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayMessageAsync(enum Windows::Internal::UI::Logon::Controller::LogonMessageMode, unsigned int, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayCredentialErrorAsync(long, long, unsigned int, struct HSTRING__*, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::MessageDisplayResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayStatusAsync(enum Windows::Internal::UI::Logon::Controller::LogonUIState, struct HSTRING__*, struct ABI::Windows::Foundation::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE TriggerLogonAnimationAsync(struct ABI::Windows::Foundation::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ResetCredentials() PURE;
			virtual HRESULT STDMETHODCALLTYPE ClearUIState(struct HSTRING__*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RestoreFromFirstSignInAnimation() PURE;
			virtual HRESULT STDMETHODCALLTYPE ShowSecurityOptionsAsync(enum Windows::Internal::UI::Logon::Controller::LogonUISecurityOptions, struct ABI::Windows::Foundation::IAsyncOperation<Windows::Internal::UI::Logon::Controller::LogonUISecurityOptionsResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_WindowContainer(struct IInspectable**) PURE;
			virtual HRESULT STDMETHODCALLTYPE Hide() PURE;
			virtual HRESULT STDMETHODCALLTYPE Stop() PURE;
		};
	}
	
}


