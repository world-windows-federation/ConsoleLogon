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
		enum UserAccountKind
		{
			UserAccountKind_Unknown = 0,
			UserAccountKind_Local = 1,
			UserAccountKind_Domain = 2,
			UserAccountKind_Connected = 3,
			UserAccountKind_CloudDomain = 4,
		};

		struct ITelemetryDataProvider : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE GetUserAccountKind(HSTRING, UserAccountKind*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CurrentLogonUIRequestReason(DWORD*) PURE;
		};

		enum DisplayStateFlags
		{
			DisplayStateFlags_None = 0,
			DisplayStateFlags_DisplayOn = 1,
			DisplayStateFlags_DisplayDim = 2,
			DisplayStateFlags_LockScreenVisible = 4,
			DisplayStateFlags_LockScreenManipulated = 8,
			DisplayStateFlags_AppOnLockScreen = 0x10,
			DisplayStateFlags_UserSwitching = 0x20,
			DisplayStateFlags_SessionLocked = 0x40,
			DisplayStateFlags_AcceptingCreds = 0x80,
			DisplayStateFlags_LidClosed = 0x100,
			DisplayStateFlags_DelayLocked = 0x200,
			DisplayStateFlags_LoggedOut = 0x400,
			DisplayStateFlags_ResumeFromHibernateOrScreenSaver = 0x800,
		};

		DEFINE_ENUM_FLAG_OPERATORS(DisplayStateFlags);

		struct IDisplayStateProvider : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_DisplayState(DisplayStateFlags*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_DisplayStateChanged(WF::ITypedEventHandler<IDisplayStateProvider*, DisplayStateFlags>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_DisplayStateChanged(EventRegistrationToken) PURE;
		};

		enum BioFeedbackState
		{
			BioFeedbackState_None = 0,
			BioFeedbackState_Error = 1,
			BioFeedbackState_PowerOn = 2,
			BioFeedbackState_Searching = 3,
			BioFeedbackState_Authenticated = 4,
			BioFeedbackState_TooFar = 5,
			BioFeedbackState_TooClose = 6,
			BioFeedbackState_MessageOnly = 7,
		};

		enum SerializationResponse
		{
			SerializationResponse_NoCredentialIncomplete = 0,
			SerializationResponse_NoCredentialComplete = 1,
			SerializationResponse_ReturnCredentialComplete = 2,
			SerializationResponse_ReturnNoCredentialComplete = 3,
		};

		enum CredentialProviderStatusIcon
		{
			CredentialProviderStatusIcon_None = 0,
			CredentialProviderStatusIcon_Error = 1,
			CredentialProviderStatusIcon_Warning = 2,
			CredentialProviderStatusIcon_Success = 3,
		};

		struct ICredentialSerialization : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_CredentialId(UINT*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationResponse(SerializationResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SerializationIcon(CredentialProviderStatusIcon*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusMessage(HSTRING*) PURE;
		};

		struct IComboBoxField : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Items(WF::Collections::IObservableVector<HSTRING> **) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedIndex(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedIndex(int) PURE;
		};

	}

	namespace Controller
	{
		enum LogonErrorRedirectorResponse
		{
			LogonErrorRedirectorResponse_Invalid = 0,
			LogonErrorRedirectorResponse_NotHandled = 1,
			LogonErrorRedirectorResponse_HandledShowLocally = 2,
			LogonErrorRedirectorResponse_HandledDoNotShowLocally = 3,
			LogonErrorRedirectorResponse_HandledDoNotShowLocallyStartOver = 4,
		};

		enum LogonUIRequestReason
		{
			LogonUIRequestReason_LogonUILogon = 0,
			LogonUIRequestReason_LogonUIUnlock = 1,
			LogonUIRequestReason_LogonUIChange = 2,
			LogonUIRequestReason_LogonUISessionRecovery = 3,
			LogonUIRequestReason_LogonUIAlternate = 4,
			LogonUIRequestReason_CredUI = 5,
		};

		struct IRedirectionManager : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE OnBeginPainting() PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectStatus(HSTRING, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectMessage(HSTRING, HSTRING, UINT, UINT*, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE RedirectLogonError(long, long, HSTRING, HSTRING, UINT, UINT*, LogonErrorRedirectorResponse*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsRedirectMode(BOOLEAN*) PURE;
		};

		enum UserTheme
		{
			UserTheme_Dark = 0,
			UserTheme_HCDark = 1,
			UserTheme_Light = 2,
			UserTheme_HCLight = 3,
		};

		enum EmbeddedSKUPolicyFlags
		{
			EmbeddedSKUPolicyFlags_DisableAll = 0,
			EmbeddedSKUPolicyFlags_EnableShutdownOption = 0x1,
			EmbeddedSKUPolicyFlags_EnableLanguageOption = 0x2,
			EmbeddedSKUPolicyFlags_EnableEaseOfAccessOption = 0x4,
			EmbeddedSKUPolicyFlags_EnableBackButton = 0x10,
			EmbeddedSKUPolicyFlags_EnableBSDR = 0x20,
			EmbeddedSKUPolicyFlags_EnableAll = 0x3F,
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

		enum LogonUIFlags
		{
			LogonUIFlags_None = 0x0,
			LogonUIFlags_SecureGestureDisabled = 0x1,
			LogonUIFlags_PasswordExpired = 0x2,
			LogonUIFlags_AllowDirectUserSwitching = 0x4,
		};

		DEFINE_ENUM_FLAG_OPERATORS(LogonUIFlags);

		struct IBioFeedbackListener : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE OnBioFeedbackUpdate(CredProvData::BioFeedbackState, HSTRING) PURE;
		};

		enum LogonMessageMode
		{
			LogonMessageMode_None = 0,
			LogonMessageMode_Message = 1,
			LogonMessageMode_LogonError = 2,
			LogonMessageMode_NoCredentialNotFinished = 3,
			LogonMessageMode_NoCredentialFinished = 4,
			LogonMessageMode_EmergencyRestart = 5,
			LogonMessageMode_TerminalServiceDisconnectOptions = 6,
			LogonMessageMode_CredentialConnecting = 7,
		};

		enum LogonUIState
		{
			LogonUIState_Start = 0,
			LogonUIState_Welcome = 1,
			LogonUIState_Logon = 2,
			LogonUIState_Authenticate = 3,
			LogonUIState_LoggingOn = 4,
			LogonUIState_LoggingOff = 5,
			LogonUIState_StandingBy = 6,
			LogonUIState_ShuttingDown = 7,
			LogonUIState_Restarting = 8,
		};

		enum LogonUISecurityOptions
		{
			LogonUISecurityOptions_Cancel = 0x0,
			LogonUISecurityOptions_Lock = 0x1,
			LogonUISecurityOptions_LogOff = 0x2,
			LogonUISecurityOptions_ChangePassword = 0x4,
			LogonUISecurityOptions_TaskManager = 0x8,
			LogonUISecurityOptions_SwitchUser = 0x10,
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

		enum LogonUICredProvResponse
		{
			LogonUICredProvResponse_LogonUIResponseDefault = 0,
			LogonUICredProvResponse_LogonUIResponseRetry = 1,
			LogonUICredProvResponse_LogonUIResponseAbort = 2,
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

		enum LogonUIShutdownChoice
		{
			LogonUIShutdownChoice_None = 0x0,
			LogonUIShutdownChoice_TurnOff = 0x1,
			LogonUIShutdownChoice_Restart = 0x2,
			LogonUIShutdownChoice_StandBy = 0x4,
			LogonUIShutdownChoice_AdvStandBy = 0x8,
			LogonUIShutdownChoice_Hibernate = 0x10,
			LogonUIShutdownChoice_EmergencyRestart = 0x20,
			LogonUIShutdownChoice_ForceCurrentSession = 0x40,
			LogonUIShutdownChoice_ForceOtherSessions = 0x80,
			LogonUIShutdownChoice_InstallUpdates = 0x100,
			LogonUIShutdownChoice_HybridMode = 0x200,
			LogonUIShutdownChoice_BootOptions = 0x400,
			LogonUIShutdownChoice_Disconnect = 0x800,
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
