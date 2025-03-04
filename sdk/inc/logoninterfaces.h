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

namespace WF = ABI::Windows::Foundation;

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
			CloudDomain = 4,
		};

		struct ITelemetryDataProvider : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE GetUserAccountKind(HSTRING, UserAccountKind*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CurrentLogonUIRequestReason(DWORD*) PURE;
		};

		enum class DisplayStateFlags
		{
			None = 0,
			DisplayOn = 1,
			DisplayDim = 2,
			LockScreenVisible = 4,
			LockScreenManipulated = 8,
			AppOnLockScreen = 0x10,
			UserSwitching = 0x20,
			SessionLocked = 0x40,
			AcceptingCreds = 0x80,
			LidClosed = 0x100,
			DelayLocked = 0x200,
			LoggedOut = 0x400,
			ResumeFromHibernateOrScreenSaver = 0x800,
		};

		DEFINE_ENUM_FLAG_OPERATORS(DisplayStateFlags);

		struct IDisplayStateProvider : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_DisplayState(DisplayStateFlags*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_DisplayStateChanged(WF::ITypedEventHandler<IDisplayStateProvider*, DisplayStateFlags>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_DisplayStateChanged(EventRegistrationToken) PURE;
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
			MessageOnly = 7,
		};

		enum class SerializationResponse
		{
			NoCredentialIncomplete = 0,
			NoCredentialComplete = 1,
			ReturnCredentialComplete = 2,
			ReturnNoCredentialComplete = 3,
		};

		enum class CredentialProviderStatusIcon
		{
			None = 0,
			Error = 1,
			Warning = 2,
			Success = 3,
		};

		struct ICredentialSerialization : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_CredentialId(UINT*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationResponse(SerializationResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationIcon(CredentialProviderStatusIcon*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusMessage(HSTRING*) PURE;
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
			HandledDoNotShowLocallyStartOver = 4,
		};

		enum class LogonUIRequestReason
		{
			LogonUILogon = 0,
			LogonUIUnlock = 1,
			LogonUIChange = 2,
			LogonUISessionRecovery = 3,
			LogonUIAlternate = 4,
			CredUI = 5,
		};

		struct IRedirectionManager : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE OnBeginPainting() PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectStatus(HSTRING, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectMessage(HSTRING, HSTRING, UINT, UINT*, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectLogonError(long, long, HSTRING, HSTRING, UINT, UINT*, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsRedirectMode(BOOLEAN*) PURE;
		};

		enum class UserTheme
		{
			Dark = 0,
			HCDark = 1,
			Light = 2,
			HCLight = 3,
		};

		enum class EmbeddedSKUPolicyFlags
		{
			DisableAll = 0,
			EnableShutdownOption = 0x1,
			EnableLanguageOption = 0x2,
			EnableEaseOfAccessOption = 0x4,
			EnableBackButton = 0x10,
			EnableBSDR = 0x20,
			EnableAll = 0x3F,
		};

		DEFINE_ENUM_FLAG_OPERATORS(EmbeddedSKUPolicyFlags);

		struct IUserSettingManager : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE IsUserSwitchingAllowed(LogonUIRequestReason, BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE CancelUserSwitch() PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLoggedOnUserSidPresent(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LangID(WCHAR*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UserSid(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_UserSid(HSTRING) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CaretWidth(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LastLoggedOnUserSid(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TelemetryDataProvider(CredProvData::ITelemetryDataProvider**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLockScreenAllowed(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_IsLockScreenAllowed(BOOLEAN) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UserTheme(UserTheme*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLowMemoryDevice(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsFirstLogonAfterSignOutOrSwitchUser(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_EmbeddedSKUPolicy(EmbeddedSKUPolicyFlags*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShowAccentColorInsteadOfLogonBackgroundImage(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsFirstBoot(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldLaunchFirstLogonAnimation(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsOobeZDPRebootRequired(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldLaunchFirstSignInAnimationInUserSession(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_ShouldLaunchFirstSignInAnimationInUserSession(BOOLEAN) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsAudioHIDEnabled(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SessionDisconnected(WF::ITypedEventHandler<IUserSettingManager*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SessionDisconnected(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_ColorSetChanged(WF::ITypedEventHandler<IUserSettingManager*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_ColorSetChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_HighContrastChanged(WF::ITypedEventHandler<IUserSettingManager*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_HighContrastChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_CaretWidthChanged(WF::ITypedEventHandler<IUserSettingManager*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_CaretWidthChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SlideToShutdownDetected(WF::ITypedEventHandler<IUserSettingManager*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SlideToShutdownDetected(EventRegistrationToken) PURE;
		};

		struct IUnlockTrigger : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE TriggerUnlock() PURE;
			virtual HRESULT STDMETHODCALLTYPE SyncBackstop() PURE;
			virtual HRESULT STDMETHODCALLTYPE CheckCompletion() PURE;
		};

		enum class LogonUIFlags
		{
			None = 0x0,
			SecureGestureDisabled = 0x1,
			PasswordExpired = 0x2,
			AllowDirectUserSwitching = 0x4,
		};

		DEFINE_ENUM_FLAG_OPERATORS(LogonUIFlags);

		struct IBioFeedbackListener : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE OnBioFeedbackUpdate(CredProvData::BioFeedbackState, HSTRING) PURE;
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
			CredentialConnecting = 7,
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
			Restarting = 8,
		};

		enum class LogonUISecurityOptions
		{
			Cancel = 0x0,
			Lock = 0x1,
			LogOff = 0x2,
			ChangePassword = 0x4,
			TaskManager = 0x8,
			SwitchUser = 0x10,
		};

		DEFINE_ENUM_FLAG_OPERATORS(LogonUISecurityOptions);

		struct IRequestCredentialsData : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Credential(CredProvData::ICredentialSerialization**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SaveCredential(BOOLEAN*) PURE;
		};

		class RequestCredentialsData : public IRequestCredentialsData
		{

		};

		enum class LogonUICredProvResponse
		{
			LogonUIResponseDefault = 0,
			LogonUIResponseRetry = 1,
			LogonUIResponseAbort = 2,
		};

		struct IReportCredentialsData : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Message(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Response(LogonUICredProvResponse*) PURE;
		};

		class ReportCredentialsData : public IReportCredentialsData
		{
		};

		struct IMessageDisplayResult : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_ResultCode(UINT*) PURE;
		};

		class MessageDisplayResult : public IMessageDisplayResult
		{
		};

		enum class LogonUIShutdownChoice
		{
			None = 0x0,
			TurnOff = 0x1,
			Restart = 0x2,
			StandBy = 0x4,
			AdvStandBy = 0x8,
			Hibernate = 0x10,
			EmergencyRestart = 0x20,
			ForceCurrentSession = 0x40,
			ForceOtherSessions = 0x80,
			InstallUpdates = 0x100,
			HybridMode = 0x200,
			BootOptions = 0x400,
			Disconnect = 0x800,
		};

		DEFINE_ENUM_FLAG_OPERATORS(LogonUIShutdownChoice);

		struct ILogonUISecurityOptionsResult : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_SecurityOptionChoice(LogonUISecurityOptions*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShutdownChoice(LogonUIShutdownChoice*) PURE;
		};

		class LogonUISecurityOptionsResult : ILogonUISecurityOptionsResult
		{
		};

		MIDL_INTERFACE("fbc9fd2c-9b7e-4ed1-9b60-61cf17f4ec4c")
		ILogonUX : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE Start(IInspectable*, IRedirectionManager*, IUserSettingManager*, CredProvData::IDisplayStateProvider*, IBioFeedbackListener*) PURE;
			virtual HRESULT STDMETHODCALLTYPE DelayLock(BOOLEAN, IUnlockTrigger*) PURE;
			virtual HRESULT STDMETHODCALLTYPE HardLock(LogonUIRequestReason, BOOLEAN, IUnlockTrigger*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RequestCredentialsAsync(LogonUIRequestReason, LogonUIFlags, WF::IAsyncOperation<RequestCredentialsData*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ReportCredentialsAsync(LogonUIRequestReason, long, long, HSTRING, HSTRING, HSTRING, WF::IAsyncOperation<ReportCredentialsData*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayMessageAsync(LogonMessageMode, UINT, HSTRING, HSTRING, WF::IAsyncOperation<MessageDisplayResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayCredentialErrorAsync(long, long, UINT, HSTRING, HSTRING, WF::IAsyncOperation<MessageDisplayResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE DisplayStatusAsync(LogonUIState, HSTRING, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE TriggerLogonAnimationAsync(WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ResetCredentials() PURE;
			virtual HRESULT STDMETHODCALLTYPE ClearUIState(HSTRING) PURE;
			virtual HRESULT STDMETHODCALLTYPE RestoreFromFirstSignInAnimation() PURE;
			virtual HRESULT STDMETHODCALLTYPE ShowSecurityOptionsAsync(LogonUISecurityOptions, WF::IAsyncOperation<LogonUISecurityOptionsResult*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_WindowContainer(IInspectable**) PURE;
			virtual HRESULT STDMETHODCALLTYPE Hide() PURE;
			virtual HRESULT STDMETHODCALLTYPE Stop() PURE;
		};
	}
}
