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
#include <windows.storage.streams.h>

namespace WF = ABI::Windows::Foundation;
namespace WFC = ABI::Windows::Foundation::Collections;
namespace WI = Windows::Internal;

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
			virtual HRESULT STDMETHODCALLTYPE get_Items(WF::Collections::IObservableVector<HSTRING>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedIndex(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedIndex(int) PURE;
		};

		enum CredProvScenario
		{
			CredProvScenario_Logon = 0,
			CredProvScenario_Unlock = 1,
			CredProvScenario_ChangePassword = 2,
			CredProvScenario_CredUI = 3
		};

		enum SelectionMode
		{
			SelectionMode_UserOnly = 0,
			SelectionMode_PLAP = 1,
			SelectionMode_UserAndV1Aggregate = 2,
			SelectionMode_FlatCredentials = 3,
			SelectionMode_SingleSelectedProvider = 4
		};

		enum TileStreamType
		{
			TileStreamType_Image = 0,
			TileStreamType_Video = 1
		};

		enum UserTileImageSize
		{
			UserTileImageSize_Large = 0,
			UserTileImageSize_Small = 1,
			UserTileImageSize_ExtraSmall = 2
		};

		struct IUserTileImage : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_TileStream(ABI::Windows::Storage::Streams::IRandomAccessStream**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TileStreamType(TileStreamType*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Size(UserTileImageSize*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TilePath(HSTRING*) PURE;
		};
		class User;
		struct IUser : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_IsLoggedIn(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsBuiltInGuest(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LogonStatus(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_EmailAddress(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_QualifiedUsername(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Sid(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LargeUserTileImage(IUserTileImage**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SmallUserTileImage(IUserTileImage**) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_ImageChanged(WF::ITypedEventHandler<User*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_ImageChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldShowEmail(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShouldShowDomainName(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsLocalNoPasswordUser(unsigned char*) PURE;
		};

		class User : /*public Platform::Object, */public IUser
		{
			class SelectedCredential
			{
			};
			class Credentials
			{
			};
			class IsAutoSubmitPending
			{
			};
			class DisplayName
			{
			};
			class EmailAddress
			{
			};
			class IsBuiltInGuest
			{
			};
			class IsLocalNoPasswordUser
			{
			};
			class IsLoggedIn
			{
			};
			class LargeUserTileImage
			{
			};
			class LogonStatus
			{
			};
			class QualifiedUsername
			{
			};
			class ShouldShowDomainName
			{
			};
			class ShouldShowEmail
			{
			};
			class Sid
			{
			};
			class SmallUserTileImage
			{
			};
			class ImageChanged
			{
			};
			class IsAutoSubmitPendingChanged
			{
			};
			class SelectedCredentialChanged
			{
			};
		public:
			void RefreshSelection();
		private:
			User();
		};

		enum CredentialFieldKind
		{
			CredentialFieldKind_Invalid = 0,
			CredentialFieldKind_StaticText = 1,
			CredentialFieldKind_CommandLink = 2,
			CredentialFieldKind_EditText = 3,
			CredentialFieldKind_TileImage = 4,
			CredentialFieldKind_CheckBox = 5,
			CredentialFieldKind_ComboBox = 6,
			CredentialFieldKind_SubmitButton = 7
		};

		enum CredentialFieldChangeKind
		{
			CredentialFieldChangeKind_State = 0,
			CredentialFieldChangeKind_InteractiveState = 1,
			CredentialFieldChangeKind_SetString = 2,
			CredentialFieldChangeKind_SetCheckbox = 3,
			CredentialFieldChangeKind_SetBitmap = 4,
			CredentialFieldChangeKind_SetComboBoxSelected = 5,
			CredentialFieldChangeKind_PasswordReveal = 6,
			CredentialFieldChangeKind_InputType = 7,
			CredentialFieldChangeKind_MaxLength = 8
		};

		struct ICredentialField : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Label(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsVisibleInSelectedTile(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsVisibleInDeselectedTile(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsHidden(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsInteractiveStateFocused(unsigned char*) PURE;;
			virtual HRESULT STDMETHODCALLTYPE get_IsInteractiveStateDisabled(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsInteractiveStateReadOnly(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Kind(CredentialFieldKind*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ID(UINT*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_FieldChanged(WF::ITypedEventHandler<ICredentialField*, CredentialFieldChangeKind>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_FieldChanged(EventRegistrationToken) PURE;
		};

		enum CredentialUIMode
		{
			CredentialUIMode_ComplexLayout = 0,
			CredentialUIMode_BasicPin = 1
		};

		enum ConnectionStatus
		{
			ConnectionStatus_NotConnecting = 0,
			ConnectionStatus_Connecting = 1,
			ConnectionStatus_Connected = 2,
			ConnectionStatus_ConnectionFailed = 3
		};

		struct ISerializationProgressInfo : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Status(ConnectionStatus*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusMessage(HSTRING*) PURE;

		};

		class SerializationProgressInfo : /*public Platform::Object,*/ public ISerializationProgressInfo
		{
			class Status
			{
			};
			class StatusMessage
			{
			};
		private:
			SerializationProgressInfo();
		};
		class Credential;
		struct ICredential : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Fields(WF::Collections::IVectorView<ICredentialField*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LogoImageField(ICredentialField**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LogoLabel(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SubmitButtonAdjacentID(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SubmitButtonChanged(WF::ITypedEventHandler<Credential*, int>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SubmitButtonChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SubmitButtonEnabled(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SubmitButtonEnabledChanged(WF::ITypedEventHandler<Credential*, bool>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SubmitButtonEnabledChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsPicturePassword(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ProviderId(GUID*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UIMode(CredentialUIMode*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsSelected(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SubmitButtonField(ICredentialField**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_HideUserTileImage(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_LargeUserTileImage(IUserTileImage**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SmallUserTileImage(IUserTileImage**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ExtraSmallUserTileImage(IUserTileImage**) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_UserImageChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_UserImageChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE Submit() PURE;
			virtual HRESULT STDMETHODCALLTYPE PicturePasswordSubmit(IInspectable*) PURE;
			virtual HRESULT STDMETHODCALLTYPE CancelSubmission() PURE;
			virtual HRESULT STDMETHODCALLTYPE RefreshSelection() PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SerializationProgressChanged(WF::ITypedEventHandler<SerializationProgressInfo*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SerializationProgressChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_UIModeChanged(WF::ITypedEventHandler<Credential*, CredentialUIMode>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_UIModeChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectionStateChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectionStateChanged(EventRegistrationToken) PURE;
		};

		class Credential : /*public Platform::Object,*/ public ICredential
		{
			class ExtraSmallUserTileImage
			{
			};
			class Fields
			{
			};
			class HideUserTileImage
			{
			};
			class IsPicturePassword
			{
			};
			class IsSelected
			{
			};
			class LargeUserTileImage
			{
			};
			class LogoImageField
			{
			};
			class LogoLabel
			{
			};
			class ProviderId
			{
			};
			class SmallUserTileImage
			{
			};
			class SubmitButtonAdjacentID
			{
			};
			class SubmitButtonEnabled
			{
			};
			class SubmitButtonField
			{
			};
			class UIMode
			{
			};
			class SelectionStateChanged
			{
			};
			class SerializationProgressChanged
			{
			};
			class SubmitButtonChanged
			{
			};
			class SubmitButtonEnabledChanged
			{
			};
			class UIModeChanged
			{
			};
			class UserImageChanged
			{
			};
		public:
			virtual HRESULT CancelSubmission();
			virtual HRESULT RefreshSelection();
			virtual HRESULT Submit();
			void PicturePasswordSubmit( /*Platform::Object*/void*);
		private:
			Credential();
		};

		class CredentialSerialization : /*public Platform::Object,*/ public ICredentialSerialization
		{
			class CredentialId
			{
			};
			class SerializationIcon
			{
			};
			class SerializationResponse
			{
			};
			class StatusMessage
			{
			};
		public:
			CredentialSerialization();
		};

		struct IReportResultInfo : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_ProviderCLSID(GUID*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusCode(long*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SubstatusCode(long*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusIcon(CredentialProviderStatusIcon*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_StatusMessage(HSTRING*) PURE;
		};

		class ReportResultInfo : /*public Platform::Object,*/ public IReportResultInfo
		{
			class ProviderCLSID
			{
			};
			class StatusCode
			{
			};
			class StatusIcon
			{
			};
			class StatusMessage
			{
			};
			class SubstatusCode
			{
			};
		public:
			ReportResultInfo();
		};
		class CredProvDataModel;
		struct ICredProvDataModel : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE InitializeAsync(CredProvScenario, unsigned short, SelectionMode, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE InitializeWithContextAsync(CredProvScenario, unsigned short, SelectionMode, UINT, unsigned char, IInspectable*, GUID, ABI::Windows::Storage::Streams::IBuffer*, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectionMode(SelectionMode*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectionMode(SelectionMode) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Users(WF::Collections::IObservableVector<User*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedUser(IUser**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedUser(IUser*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedUserChanged(WF::ITypedEventHandler<User*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedUserChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_PLAPCredentials(WF::Collections::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedPLAPCredential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedPLAPCredential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedPLAPCredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedPLAPCredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_V1Credentials(WF::Collections::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedV1Credential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedV1Credential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedV1CredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedV1CredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UsersAndV1Credentials(WF::Collections::IObservableVector<IInspectable*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedUserOrV1Credential(IInspectable**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedUserOrV1Credential(IInspectable*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedUserOrV1CredentialChanged(WF::ITypedEventHandler<IInspectable*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedUserOrV1CredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_FlatCredentials(WF::Collections::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedFlatCredential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedFlatCredential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedFlatCredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedFlatCredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SerializationComplete(WF::ITypedEventHandler<CredProvDataModel*, CredentialSerialization*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SerializationComplete(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_CurrentBioFeedbackState(BioFeedbackState*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_BioFeedbackLabel(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_BioFeedbackStateChange(WF::ITypedEventHandler<CredProvDataModel*, BioFeedbackState>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_BioFeedbackStateChange(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE ReportResultAsync(long, long, HSTRING, WF::IAsyncOperation<ReportResultInfo*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ClearState() PURE;
			virtual HRESULT STDMETHODCALLTYPE ResetAsync(CredProvScenario, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE ResetSelection() PURE;
			virtual HRESULT STDMETHODCALLTYPE Shutdown() PURE;
			virtual HRESULT STDMETHODCALLTYPE DisconnectCredentials() PURE;
		};

		class CredProvDataModel : /*public Platform::Object,*/ public ICredProvDataModel
		{
			class SelectionMode
			{
			};
			class SelectedV1Credential
			{
			};
			class SelectedUserOrV1Credential
			{
			};
			class SelectedUser
			{
			};
			class SelectedPLAPCredential
			{
			};
			class SelectedFlatCredential
			{
			};
			class BioFeedbackLabel
			{
			};
			class CurrentBioFeedbackState
			{
			};
			class FlatCredentials
			{
			};
			class PLAPCredentials
			{
			};
			class Users
			{
			};
			class UsersAndV1Credentials
			{
			};
			class V1Credentials
			{
			};
			class BioFeedbackStateChange
			{
			};
			class SelectedFlatCredentialChanged
			{
			};
			class SelectedPLAPCredentialChanged
			{
			};
			class SelectedUserChanged
			{
			};
			class SelectedUserOrV1CredentialChanged
			{
			};
			class SelectedV1CredentialChanged
			{
			};
			class SerializationComplete
			{
			};

			//not used in consolelogon
			//public:
			//	void DisconnectCredentials();
			//	struct WF::IAsyncAction* InitializeAsync(enum Windows::Internal::UI::Logon::CredProvData::CredProvScenario, unsigned short, enum Windows::Internal::UI::Logon::CredProvData::SelectionMode);
			//	struct WF::IAsyncAction* InitializeWithContextAsync(enum Windows::Internal::UI::Logon::CredProvData::CredProvScenario, unsigned short, enum Windows::Internal::UI::Logon::CredProvData::SelectionMode, UINT, bool, void/*class Platform::Object*/*, class Platform::Guid, struct Windows::Storage::Streams::IBuffer*);
			//	struct WF::IAsyncOperation<Windows::Internal::UI::Logon::CredProvData::ReportResultInfo* /*originally a ^ for c++/cx, made a ptr for now*/>* ReportResultAsync(int, int, void/*class Platform::String*/*);
			//	void ClearState();
			//	struct WF::IAsyncAction* ResetAsync(enum Windows::Internal::UI::Logon::CredProvData::CredProvScenario);
			//	void ResetSelection();
			//	void Shutdown();
		};

		struct ICredentialGroup : public IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Credentials(WF::Collections::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedCredential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedCredential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedCredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedCredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE RefreshSelection() PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsAutoSubmitPending(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_IsAutoSubmitPendingChanged(WF::ITypedEventHandler<ICredentialGroup*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_IsAutoSubmitPendingChanged(EventRegistrationToken) PURE;
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

struct IDispatchNotification : public IUnknown
{
	virtual void STDMETHODCALLTYPE Dispatch() PURE;
};

struct INotificationDispatcher : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE QueueNotification(IDispatchNotification*) PURE;
	virtual void STDMETHODCALLTYPE StartProcessingNotifications() PURE;
	virtual void STDMETHODCALLTYPE StopProcessingNotifications() PURE;
};

namespace LC = Windows::Internal::UI::Logon::Controller;
namespace LCPD = Windows::Internal::UI::Logon::CredProvData;
