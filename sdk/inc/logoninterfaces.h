#pragma once

#include "pch.h"

#include <windows.foundation.collections.h>
#include <windows.foundation.numerics.h>
#include <windows.storage.streams.h>

namespace Windows::Internal
{
}

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

		MIDL_INTERFACE("ff3e0ba7-f55b-40db-91d9-58626f36aead")
		IComboBoxField : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Items(WFC::IObservableVector<HSTRING>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedIndex(int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedIndex(int) PURE;
		};

		//TODO: verify iid
		MIDL_INTERFACE("5cad4c8f-6f35-4a41-ba52-bdf26571c77e")
		ICheckBoxField : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_BoxLabel(HSTRING* boxLabel) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Checked(bool* isChecked) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_Checked(bool isChecked) PURE;
		};

		MIDL_INTERFACE("20486918-5147-4725-a0cb-12b952a5f0c3")
		ICommandLinkField : IInspectable
		{
			HRESULT get_Content(HSTRING* outContent);
			HRESULT Invoke();
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

		struct IUserTileImage : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_TileStream(ABI::Windows::Storage::Streams::IRandomAccessStream**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TileStreamType(TileStreamType*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Size(UserTileImageSize*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_TilePath(HSTRING*) PURE;
		};

		class User;

		MIDL_INTERFACE("837c3232-b75b-4f12-bf4c-d85f57d3882f")
		IUser : IInspectable
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

		struct ICredentialField : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Label(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsVisibleInSelectedTile(bool*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsVisibleInDeselectedTile(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsHidden(bool*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsInteractiveStateFocused(unsigned char*) PURE;
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

		struct ISerializationProgressInfo : IInspectable
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

			SerializationProgressInfo();
		};

		class Credential;

		MIDL_INTERFACE("a450753a-7095-4042-9ce1-d2847167ee58")
		ICredential : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Fields(WFC::IVectorView<ICredentialField*>**) PURE;
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

		struct IReportResultInfo : IInspectable
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

		struct ICredProvDataModel : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE InitializeAsync(CredProvScenario, unsigned short, SelectionMode, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE InitializeWithContextAsync(CredProvScenario, unsigned short, SelectionMode, UINT, unsigned char, IInspectable*, GUID, ABI::Windows::Storage::Streams::IBuffer*, WF::IAsyncAction**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectionMode(SelectionMode*) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectionMode(SelectionMode) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Users(WFC::IObservableVector<User*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedUser(IUser**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedUser(IUser*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedUserChanged(WF::ITypedEventHandler<User*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedUserChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_PLAPCredentials(WFC::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedPLAPCredential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedPLAPCredential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedPLAPCredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedPLAPCredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_V1Credentials(WFC::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedV1Credential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedV1Credential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedV1CredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedV1CredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_UsersAndV1Credentials(WFC::IObservableVector<IInspectable*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedUserOrV1Credential(IInspectable**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedUserOrV1Credential(IInspectable*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedUserOrV1CredentialChanged(WF::ITypedEventHandler<IInspectable*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedUserOrV1CredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_FlatCredentials(WFC::IObservableVector<Credential*>**) PURE;
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

		struct IDispatchEvent : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE Dispatch() PURE;
		};

		MIDL_INTERFACE("edc8d4ae-643f-4ebf-b869-b1463e38b829")
		IUIThreadEventDispatcher : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE DispatchEvent(IDispatchEvent*) PURE;
		};

		enum OptionalDependencyKind
		{
			OptionalDependencyKind_AutoLogonManager = 0,
			OptionalDependencyKind_WindowContainer = 1,
			OptionalDependencyKind_DefaultSelector = 2,
			OptionalDependencyKind_DisplayState = 3,
			OptionalDependencyKind_ViewGeneratedCredentialField = 4,
			OptionalDependencyKind_TelemetryData = 5,
			OptionalDependencyKind_UserTileImage = 6,
		};

		MIDL_INTERFACE("ef207a42-36ca-4e16-81c0-116cccc45deb")
		IOptionalDependencyProvider : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE GetOptionalDependency(OptionalDependencyKind, IInspectable**) PURE;
		};

		MIDL_INTERFACE("4b869909-e780-424e-ae03-210fe46f4527")
		ICredProvDataModelFactory : IInspectable
		{
			virtual HRESULT CreateCredProvDataModel(IUIThreadEventDispatcher*, IOptionalDependencyProvider*, ICredProvDataModel**) PURE;
		};

		MIDL_INTERFACE("ba6e72f5-f47d-4aad-8bb6-1bba3a750e9f")
		ICredentialGroup : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Credentials(WFC::IObservableVector<Credential*>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SelectedCredential(ICredential**) PURE;
			virtual HRESULT STDMETHODCALLTYPE put_SelectedCredential(ICredential*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_SelectedCredentialChanged(WF::ITypedEventHandler<Credential*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_SelectedCredentialChanged(EventRegistrationToken) PURE;
			virtual HRESULT STDMETHODCALLTYPE RefreshSelection() PURE;
			virtual HRESULT STDMETHODCALLTYPE get_IsAutoSubmitPending(unsigned char*) PURE;
			virtual HRESULT STDMETHODCALLTYPE add_IsAutoSubmitPendingChanged(WF::ITypedEventHandler<ICredentialGroup*, IInspectable*>*, EventRegistrationToken*) PURE;
			virtual HRESULT STDMETHODCALLTYPE remove_IsAutoSubmitPendingChanged(EventRegistrationToken) PURE;
		};

		MIDL_INTERFACE("9a5f2170-ef01-4fe4-83c1-141ae408d40b")
		ICredProvDefaultSelector : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_UseLastLoggedOnProvider(BOOLEAN*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_PreferredProviders(WFC::IVectorView<GUID>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ExcludedProviders(WFC::IVectorView<GUID>**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_DefaultUserSid(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE AllowAutoSubmitOnSelection(IUser*, BOOLEAN*) PURE;
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
			virtual HRESULT STDMETHODCALLTYPE get_LangID(LANGID*) PURE;
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

		MIDL_INTERFACE("3b1f419e-0b45-42b1-a18c-3a2a8e1715db")
		IRequestCredentialsData : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Credential(CredProvData::ICredentialSerialization**) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_SaveCredential(BOOLEAN*) PURE;
		};

		class RequestCredentialsData : public IRequestCredentialsData
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

		MIDL_INTERFACE("49931e22-78c2-41af-83de-4e02876bdb3c")
		IRequestCredentialsDataFactory : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE CreateRequestCredentialsData(CredProvData::ICredentialSerialization*, LogonUIShutdownChoice, IRequestCredentialsData**) PURE;
		};

		enum LogonUICredProvResponse
		{
			LogonUICredProvResponse_LogonUIResponseDefault = 0,
			LogonUICredProvResponse_LogonUIResponseRetry = 1,
			LogonUICredProvResponse_LogonUIResponseAbort = 2,
		};

		MIDL_INTERFACE("ba2419f9-f552-4e37-9145-3c1673104057")
		IReportCredentialsData : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_Message(HSTRING*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_Response(LogonUICredProvResponse*) PURE;
		};

		class ReportCredentialsData : public IReportCredentialsData
		{
		};

		MIDL_INTERFACE("2e6a5994-4adf-4e53-80ea-106294d15f73")
		IReportCredentialsDataFactory : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE CreateReportCredentialsData(LogonUICredProvResponse, HSTRING, IReportCredentialsData**) PURE;
		};

		MIDL_INTERFACE("63ace640-87c6-4bda-ac7c-793c6e16cce1")
		IMessageDisplayResult : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_ResultCode(UINT*) PURE;
		};

		class MessageDisplayResult : public IMessageDisplayResult
		{
		};

		MIDL_INTERFACE("a95d3ab1-c7b8-43a3-935d-26f432e72e0f")
		IMessageDisplayResultFactory : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE CreateMessageDisplayResult(UINT, IMessageDisplayResult**) PURE;
		};

		MIDL_INTERFACE("e1b3be00-a1e8-4ab4-a1b9-ea94cc55ed0d")
		ILogonUISecurityOptionsResult : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE get_SecurityOptionChoice(LogonUISecurityOptions*) PURE;
			virtual HRESULT STDMETHODCALLTYPE get_ShutdownChoice(LogonUIShutdownChoice*) PURE;
		};

		class LogonUISecurityOptionsResult : ILogonUISecurityOptionsResult
		{
		};

		MIDL_INTERFACE("d54e91cd-1605-4e92-a736-b18f5b26cedd")
		ILogonUISecurityOptionsResultFactory : IInspectable
		{
			virtual HRESULT STDMETHODCALLTYPE CreateSecurityOptionsResult(LogonUISecurityOptions, LogonUIShutdownChoice, ILogonUISecurityOptionsResult**) PURE;
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

extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_CredProvData_CredProvDataModel[] = L"Windows.Internal.UI.Logon.CredProvData.CredProvDataModel";

extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_RequestCredentialsData[] = L"Windows.Internal.UI.Logon.Controller.RequestCredentialsData";
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ReportCredentialsData[] = L"Windows.Internal.UI.Logon.Controller.ReportCredentialsData";
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_MessageDisplayResult[] = L"Windows.Internal.UI.Logon.Controller.MessageDisplayResult";
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_LogonUISecurityOptionsResult[] = L"Windows.Internal.UI.Logon.Controller.LogonUISecurityOptionsResult";

namespace LC = Windows::Internal::UI::Logon::Controller;
namespace LCPD = Windows::Internal::UI::Logon::CredProvData;

#pragma region "Specializations for templated types"
// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor

namespace ABI::Windows::Foundation
{
	template <>
	struct __declspec(uuid("125cf70d-ac9b-5238-a2a6-831df1af645b"))
	ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::CredentialSerialization*>
		: ITypedEventHandler_impl<
			  Internal::AggregateType<LCPD::CredProvDataModel*, LCPD::ICredProvDataModel*>
			, Internal::AggregateType<LCPD::CredentialSerialization*, LCPD::ICredentialSerialization*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.TypedEventHandler`2<Windows.Internal.UI.Logon.CredProvData.CredProvDataModel, Windows.Internal.UI.Logon.CredProvData.CredentialSerialization>";
		}
	};

	template <>
	struct __declspec(uuid("fe8d3c74-46c4-5425-be8f-121f86ded03a"))
	ITypedEventHandler<LCPD::CredProvDataModel*, LCPD::BioFeedbackState>
		: ITypedEventHandler_impl<
			  Internal::AggregateType<LCPD::CredProvDataModel*, LCPD::ICredProvDataModel*>, LCPD::BioFeedbackState
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.TypedEventHandler`2<Windows.Internal.UI.Logon.CredProvData.CredProvDataModel, Windows.Internal.UI.Logon.CredProvData.BioFeedbackState>";
		}
	};

	template <>
	struct __declspec(uuid("c7e65ce2-fad5-5e3b-9c58-186ca8c1dd57"))
	ITypedEventHandler<IInspectable*, IInspectable*>
		: ITypedEventHandler_impl<IInspectable*, IInspectable*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.TypedEventHandler`2<Object, Object>";
		}
	};

	template <>
	struct __declspec(uuid("bb2bff59-99c0-5e2c-8dd8-0eb219b6df5e"))
	ITypedEventHandler<LCPD::Credential*, IInspectable*>
		: ITypedEventHandler_impl<
			  Internal::AggregateType<LCPD::Credential*, LCPD::ICredential*>
			, IInspectable*
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.TypedEventHandler`2<Windows.Internal.UI.Logon.CredProvData.Credential, Object>";
		}
	};

	template <>
	struct
	IAsyncOperation<LCPD::ReportResultInfo*>
		: IAsyncOperation_impl<
			  Internal::AggregateType<LCPD::ReportResultInfo*, LCPD::IReportResultInfo*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.CredProvData.ReportResultInfo>";
		}
	};

	template <>
	struct __declspec(uuid("21b7b9a0-3262-5d33-bab9-93bf7441e227"))
	IAsyncOperation<LC::LogonUISecurityOptionsResult*>
		: IAsyncOperation_impl<
			  Internal::AggregateType<LC::LogonUISecurityOptionsResult*, LC::ILogonUISecurityOptionsResult*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.Controller.LogonUISecurityOptionsResult>";
		}
	};

	template <>
	struct __declspec(uuid("55a51b4d-565d-5406-a96c-330ce9523a75"))
	IAsyncOperation<LC::MessageDisplayResult*>
		: IAsyncOperation_impl<
			  Internal::AggregateType<LC::MessageDisplayResult*, LC::IMessageDisplayResult*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.Controller.MessageDisplayResult>";
		}
	};

	template <>
	struct __declspec(uuid("b1cba636-d8a4-5a7c-8ea5-73c457d4a53c"))
	IAsyncOperation<LC::ReportCredentialsData*>
		: IAsyncOperation_impl<
			  Internal::AggregateType<LC::ReportCredentialsData*, LC::IReportCredentialsData*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.Controller.ReportCredentialsData>";
		}
	};

	template <>
	struct __declspec(uuid("dfa58656-8538-5608-a288-2c63273a655e"))
	IAsyncOperation<LC::RequestCredentialsData*>
		: IAsyncOperation_impl<
			  Internal::AggregateType<LC::RequestCredentialsData*, LC::IRequestCredentialsData*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.IAsyncOperation`1<Windows.Internal.UI.Logon.Controller.RequestCredentialsData>";
		}
	};

	template <>
	struct __declspec(uuid("ab2e40b5-1d90-529c-a514-45d6219c2f75"))
	IAsyncOperationCompletedHandler<LCPD::ReportResultInfo*>
		: IAsyncOperationCompletedHandler_impl<
			  Internal::AggregateType<LCPD::ReportResultInfo*, LCPD::IReportResultInfo*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Internal.UI.Logon.CredProvData.ReportResultInfo>";
		}
	};

	template <>
	struct __declspec(uuid("db4ea180-872b-5393-96c5-f2617c05b8d5"))
	IAsyncOperationCompletedHandler<LC::LogonUISecurityOptionsResult*>
		: IAsyncOperationCompletedHandler_impl<
			  Internal::AggregateType<LC::LogonUISecurityOptionsResult*, LC::ILogonUISecurityOptionsResult*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Internal.UI.Logon.Controller.LogonUISecurityOptionsResult>";
		}
	};

	template <>
	struct __declspec(uuid("159cd2c8-ce8c-5397-9590-b2f0b17306e7"))
	IAsyncOperationCompletedHandler<LC::MessageDisplayResult*>
		: IAsyncOperationCompletedHandler_impl<
			  Internal::AggregateType<LC::MessageDisplayResult*, LC::IMessageDisplayResult*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Internal.UI.Logon.Controller.MessageDisplayResult>";
		}
	};

	template <>
	struct __declspec(uuid("bed8c32f-53c8-5309-a701-c81353ecc80d"))
	IAsyncOperationCompletedHandler<LC::ReportCredentialsData*>
		: IAsyncOperationCompletedHandler_impl<
			  Internal::AggregateType<LC::ReportCredentialsData*, LC::IReportCredentialsData*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Internal.UI.Logon.Controller.ReportCredentialsData>";
		}
	};

	template <>
	struct __declspec(uuid("613f6d50-f93e-568a-bd91-af580034347c"))
	IAsyncOperationCompletedHandler<LC::RequestCredentialsData*>
		: IAsyncOperationCompletedHandler_impl<
			  Internal::AggregateType<LC::RequestCredentialsData*, LC::IRequestCredentialsData*>
		>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.AsyncOperationCompletedHandler`1<Windows.Internal.UI.Logon.Controller.RequestCredentialsData>";
		}
	};
}

namespace ABI::Windows::Foundation::Collections
{
	// WFC::IVector<IInspectable*>

	template <>
	struct __declspec(uuid("b32bdca4-5e52-5b27-bc5d-d66a1a268c2a"))
	IVector<IInspectable*>
		: IVector_impl<IInspectable*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.IVector`1<Object>";
		}
	};

	template <>
	struct
	IObservableVector<HSTRING>
		: IObservableVector_impl<HSTRING>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.IObservableVector`1<String>";
		}
	};

	template <>
	struct
	IObservableVector<IInspectable*>
		: IObservableVector_impl<IInspectable*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.IObservableVector`1<Object>";
		}
	};

	template <>
	struct
	IObservableVector<LCPD::Credential*>
		: IObservableVector_impl<LCPD::Credential*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.IObservableVector`1<Windows.Internal.UI.Logon.CredProvData.Credential>";
		}
	};

	template <>
	struct __declspec(uuid("cb6c396f-4861-5296-b14b-bd90b941a3e0"))
	VectorChangedEventHandler<HSTRING>
		: VectorChangedEventHandler_impl<HSTRING>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.VectorChangedEventHandler`1<String>";
		}
	};

	template <>
	struct __declspec(uuid("b423a801-d35e-56b9-813b-00889536cb98"))
	VectorChangedEventHandler<IInspectable*>
		: VectorChangedEventHandler_impl<IInspectable*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.VectorChangedEventHandler`1<Object>";
		}
	};

	template <>
	struct __declspec(uuid("a1181e1b-89d9-5148-8b13-c202a0e595f4"))
	VectorChangedEventHandler<LCPD::Credential*>
		: VectorChangedEventHandler_impl<LCPD::Credential*>
	{
		static const wchar_t* z_get_rc_name_impl()
		{
			return L"Windows.Foundation.Collections.VectorChangedEventHandler`1<Windows.Internal.UI.Logon.CredProvData.Credential>";
		}
	};
}

// ReSharper restore CppPolymorphicClassWithNonVirtualPublicDestructor
#pragma endregion "Specializations for templated types"
