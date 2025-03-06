#include "pch.h"

#include "logonviewmanager.h"

#include <WtsApi32.h>

#include "NativeString.h"
#include "RefCountedObject.h"

using namespace Microsoft::WRL;

LogonViewManager::LogonViewManager()
	: m_currentViewType(LogonView::None)
	, m_serializationCompleteToken()
	, m_bioFeedbackStateChangeToken()
	, m_usersChangedToken()
	, m_selectedUserChangeToken()
	, m_credentialsChangedToken()
	, m_selectedCredentialChangedToken()
	, m_isCredentialResetRequired(false)
	, m_credProvInitialized(false)
	, m_showCredentialViewOnInitComplete(false)
	, m_currentReason(LC::LogonUIRequestReason_LogonUILogon)
{
}

HRESULT LogonViewManager::RuntimeClassInitialize()
{
	RETURN_IF_FAILED(Initialize()); // 46
	return S_OK;
}

HRESULT LogonViewManager::Invoke(LCPD::ICredential* sender, IInspectable* args)
{
	if (m_currentViewType == LogonView::UserSelection
		|| m_currentViewType == LogonView::CredProvSelection
		|| m_currentViewType == LogonView::SelectedCredential
		|| m_currentViewType == LogonView::ComboBox)
	{
		RETURN_IF_FAILED(ShowCredentialView()); // 235
	}

	return S_OK;
}

HRESULT LogonViewManager::Invoke(WFC::IObservableVector<LCPD::Credential*>* sender, WFC::IVectorChangedEventArgs* args)
{
	if (m_currentViewType == LogonView::CredProvSelection)
	{
		Wrappers::HString userName;

		ComPtr<LCPD::IUser> user;
		RETURN_IF_FAILED(m_selectedGroup.As(&user)); // 218

		RETURN_IF_FAILED(user->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 220

		RETURN_IF_FAILED(ShowCredProvSelection(m_selectedGroup.Get(), userName.Get())); // 222
	}

	return S_OK;
}

HRESULT LogonViewManager::Invoke(WFC::IObservableVector<IInspectable*>* sender, WFC::IVectorChangedEventArgs* args)
{
	if (m_currentViewType == LogonView::UserSelection)
	{
		RETURN_IF_FAILED(ShowUserSelection()); // 205
	}

	return S_OK;
}

HRESULT LogonViewManager::Invoke(IInspectable* sender, IInspectable* args)
{
	if (m_currentViewType == LogonView::UserSelection
		|| m_currentViewType == LogonView::CredProvSelection
		|| m_currentViewType == LogonView::SelectedCredential
		|| m_currentViewType == LogonView::ComboBox)
	{
		RETURN_IF_FAILED(ShowCredentialView()); // 196
	}

	return S_OK;
}

HRESULT LogonViewManager::Invoke(LCPD::ICredProvDataModel* sender, LCPD::BioFeedbackState args)
{
	if (m_bioFeedbackListener.Get())
	{
		LCPD::BioFeedbackState state;
		RETURN_IF_FAILED(m_credProvDataModel->get_CurrentBioFeedbackState(&state)); // 178

		Wrappers::HString label;
		RETURN_IF_FAILED(m_credProvDataModel->get_BioFeedbackLabel(label.ReleaseAndGetAddressOf())); // 181

		RETURN_IF_FAILED(m_bioFeedbackListener->OnBioFeedbackUpdate(state, label.Get())); // 183
	}

	return S_OK;
}

HRESULT LogonViewManager::Invoke(LCPD::ICredProvDataModel* sender, LCPD::ICredentialSerialization* args)
{
	if (m_requestCredentialsComplete)
	{
		LCPD::SerializationResponse response;
		RETURN_IF_FAILED(args->get_SerializationResponse(&response)); // 55

		if (response == LCPD::SerializationResponse_ReturnCredentialComplete)
		{
			wil::unique_cotaskmem_ptr<BYTE> aaa = wil::make_unique_cotaskmem_nothrow<BYTE>(20 * 20);
			ComPtr<LC::IRequestCredentialsDataFactory> factory;
			RETURN_IF_FAILED(WF::GetActivationFactory(
				Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_RequestCredentialsData).Get(), &factory)); // 60

			ComPtr<LC::IRequestCredentialsData> data;
			RETURN_IF_FAILED(factory->CreateRequestCredentialsData(args, LC::LogonUIShutdownChoice_None, &data)); // 63

			RETURN_IF_FAILED(m_requestCredentialsComplete->GetResult().Set(data.Get())); // 65

			m_requestCredentialsComplete->Complete(S_OK);
			m_requestCredentialsComplete.reset();
		}
		else
		{
			bool b = true;

			if (response != LCPD::SerializationResponse_ReturnNoCredentialComplete)
			{
				Wrappers::HString statusMessage;
				RETURN_IF_FAILED(args->get_StatusMessage(statusMessage.ReleaseAndGetAddressOf())); // 75
				if (statusMessage.Get())
				{
					UINT uType = 0;
					if (response == LCPD::SerializationResponse_NoCredentialIncomplete)
					{
						uType = 3;
					}
					else if (response == LCPD::SerializationResponse_NoCredentialComplete)
					{
						uType = 4;
					}

					LCPD::CredentialProviderStatusIcon statusIcon;
					RETURN_IF_FAILED(args->get_SerializationIcon(&statusIcon)); // 92

					UINT logonMessageMode;
					if (statusIcon == LCPD::CredentialProviderStatusIcon_Error)
					{
						logonMessageMode = 0x10;
					}
					else if (statusIcon == LCPD::CredentialProviderStatusIcon_Warning)
					{
						logonMessageMode = 0x10 | 0x20;
					}
					else
					{
						logonMessageMode = 0x40;
					}

					CoTaskMemNativeString caption;
					RETURN_IF_FAILED(caption.Initialize(HINST_THISCOMPONENT, 128)); // 108

					UINT redirectResult;
					LC::LogonErrorRedirectorResponse errorResponse;
					RETURN_IF_FAILED(m_redirectionManager->RedirectMessage(Wrappers::HStringReference(caption.Get()).Get(), statusMessage.Get(), logonMessageMode, &redirectResult, &errorResponse)); // 112
					if (errorResponse == LC::LogonErrorRedirectorResponse_HandledDoNotShowLocally && uType == 3)
					{
						errorResponse = LC::LogonErrorRedirectorResponse_HandledShowLocally;
					}

					if (errorResponse != LC::LogonErrorRedirectorResponse_HandledDoNotShowLocally
						&& errorResponse != LC::LogonErrorRedirectorResponse_HandledDoNotShowLocallyStartOver)
					{
						b = false;
						RETURN_IF_FAILED(m_redirectionManager->OnBeginPainting()); // 132
						RETURN_IF_FAILED(ShowSerializationFailedView(Wrappers::HStringReference(caption.Get()).Get(), statusMessage.Get())); // 133
					}
				}
				else
				{
					if (response == LCPD::SerializationResponse_NoCredentialIncomplete
						|| response == LCPD::SerializationResponse_NoCredentialComplete)
					{
						RETURN_IF_FAILED(m_redirectionManager->OnBeginPainting()); // 142
						RETURN_IF_FAILED(ShowCredentialView()); // 143
						b = false;
					}
				}
			}

			if (b)
			{
				ComPtr<LC::IRequestCredentialsDataFactory> factory;
				RETURN_IF_FAILED(WF::GetActivationFactory(
					Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_RequestCredentialsData).Get(), &factory)); // 152
				ComPtr<LC::IRequestCredentialsData> data;
				RETURN_IF_FAILED(factory->CreateRequestCredentialsData(nullptr, LC::LogonUIShutdownChoice_None, &data)); // 155

				RETURN_IF_FAILED(m_requestCredentialsComplete->GetResult().Set(data.Get())); // 157

				m_requestCredentialsComplete->Complete(S_OK);
			}
		}
	}
	else if (m_unlockTrigger.Get())
	{
		m_cachedSerialization = args;
		RETURN_IF_FAILED(m_unlockTrigger->TriggerUnlock()); // 165
		m_unlockTrigger.Reset();
	}

	return S_OK;
}

HRESULT LogonViewManager::OnNavigation()
{
	switch (m_currentViewType)
	{
		case LogonView::CredProvSelection:
		{
			if (!GetSystemMetrics(SM_REMOTESESSION))
			{
				if (m_currentReason == LC::LogonUIRequestReason_LogonUIUnlock)
				{
					RETURN_IF_FAILED(m_userSettingManager->put_IsLockScreenAllowed(FALSE)); // 281
					WTSDisconnectSession(nullptr, WTS_CURRENT_SESSION, FALSE);
				}
				else if (m_currentReason == LC::LogonUIRequestReason_LogonUILogon)
				{
					RETURN_IF_FAILED(m_credProvDataModel->put_SelectedUserOrV1Credential(nullptr)); // 286
				}
			}

			break;
		}
		case LogonView::SelectedCredential:
		{
			if (m_currentReason == LC::LogonUIRequestReason_LogonUIChange)
			{
				if (m_requestCredentialsComplete)
				{
					m_requestCredentialsComplete->Complete(HRESULT_FROM_WIN32(ERROR_CANCELLED));
				}
			}
			else
			{
				ComPtr<IInspectable> selectedUserOrV1;
				RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUserOrV1Credential(&selectedUserOrV1)); // 260

				ComPtr<LCPD::ICredentialGroup> group;
				if (SUCCEEDED(selectedUserOrV1.As(&group)))
				{
					RETURN_IF_FAILED(((LCPD::ICredentialGroup*)selectedUserOrV1.Get())->put_SelectedCredential(nullptr)); // 265 @Note WTF??? Why cast like this?
				}
				else
				{
					RETURN_IF_FAILED(m_credProvDataModel->put_SelectedUserOrV1Credential(nullptr)); // 269
				}
			}

			break;
		}
		case LogonView::ComboBox:
		{
			RETURN_IF_FAILED(ShowCredentialView()); // 245
			break;
		}
		case LogonView::Locked:
		{
			if (m_unlockTrigger.Get())
			{
				RETURN_IF_FAILED(m_unlockTrigger->TriggerUnlock()); // 294
				m_unlockTrigger.Reset();
			}

			break;
		}
		case LogonView::SerializationFailed:
		{
			RETURN_IF_FAILED(ShowCredentialView()); // 300
			break;
		}
	}

	return S_OK;
}

HRESULT LogonViewManager::ShowComboBox(LCPD::IComboBoxField* dataSource)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 311

	ComPtr<ComboboxSelectionView> comboBoxView;
	RETURN_IF_FAILED(MakeAndInitialize<ComboboxSelectionView>(&comboBoxView, dataSource)); // 314

	RETURN_IF_FAILED(comboBoxView->ComboboxSelectionView::Advise(this)); // 316

	RETURN_IF_FAILED(ConsoleUIManager::SetActiveView(comboBoxView.Get())); // 318

	m_currentView.Swap(comboBoxView.Get());
	m_currentViewType = LogonView::ComboBox;
	return S_OK;
}

template <typename T>
class CDispatchNotification final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IDispatchNotification>
{
public:
	CDispatchNotification(const T& func)
		: _func(func)
	{
	}

	void Dispatch() override
	{
		_func();
	}

private:
	T _func;
};

template <typename T>
HRESULT BeginInvoke(INotificationDispatcher* pDispatcher, const T& func)
{
	ComPtr<IDispatchNotification> spDispatchNotification = Make<CDispatchNotification<T>>(func);
	HRESULT hr = spDispatchNotification.Get() ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pDispatcher->QueueNotification(spDispatchNotification.Get());
	}

	return hr;
}

HRESULT LogonViewManager::SetContext(
	IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
	LC::IRedirectionManager* redirectionManager, LCPD::IDisplayStateProvider* displayStateProvider,
	LC::IBioFeedbackListener* bioFeedbackListener)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 328

	ComPtr<IInspectable> autoLogonManagerRef = autoLogonManager;
	ComPtr<LC::IUserSettingManager> userSettingManagerRef = userSettingManager;
	ComPtr<LC::IRedirectionManager> redirectionManagerRef = redirectionManager;
	ComPtr<LCPD::IDisplayStateProvider> displayStateProviderRef = displayStateProvider;
	ComPtr<LC::IBioFeedbackListener> bioFeedbackListenerRef = bioFeedbackListener;
	ComPtr<LogonViewManager> thisRef = this;

	wil::unique_event_nothrow setContextCompleteEvent;
	RETURN_IF_FAILED(setContextCompleteEvent.create(wil::EventOptions::ManualReset)); // 339

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, &setContextCompleteEvent]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		SetContextUIThread(
			autoLogonManagerRef.Get(), userSettingManagerRef.Get(), redirectionManagerRef.Get(),
			displayStateProviderRef.Get(), bioFeedbackListenerRef.Get());
		setContextCompleteEvent.SetEvent();
	});
	RETURN_IF_FAILED(hr); // 344
	WaitForSingleObject(setContextCompleteEvent.get(), INFINITE);
	return S_OK;
}

HRESULT LogonViewManager::Lock(LC::LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, LC::IUnlockTrigger* unlockTrigger)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 353

	ComPtr<LC::IUnlockTrigger> unlockTriggerRef = unlockTrigger;
	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		HRESULT hrInner = LockUIThread(reason, allowDirectUserSwitching, unlockTriggerRef.Get());
		if (FAILED(hrInner))
		{
			unlockTriggerRef->TriggerUnlock();
		}
	});
	RETURN_IF_FAILED(hr); // 364
	return S_OK;
}

HRESULT LogonViewManager::RequestCredentials(
	LC::LogonUIRequestReason reason, LC::LogonUIFlags flags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 370

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> deferral = completion;
		HRESULT hrInner = RequestCredentialsUIThread(reason, flags, deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 382
	return S_OK;
}

#define REFCOUNT_HSTRING_REF(obj) ComPtr<CRefCountedObject<Wrappers::HString>> obj; \
	CreateRefCountedObj<Wrappers::HString>(&obj);

HRESULT LogonViewManager::ReportResult(
	LC::LogonUIRequestReason reason, NTSTATUS ntStatus, NTSTATUS ntSubStatus, HSTRING samCompatibleUserName,
	HSTRING displayName, HSTRING userSid,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 388

	REFCOUNT_HSTRING_REF(samCompatibleUserNameRef);
	RETURN_IF_FAILED(samCompatibleUserNameRef->Set(samCompatibleUserName)); // 391

	REFCOUNT_HSTRING_REF(displayNameRef);
	RETURN_IF_FAILED(displayNameRef->Set(displayName)); // 394

	REFCOUNT_HSTRING_REF(userSidRef);
	RETURN_IF_FAILED(userSidRef->Set(userSid)); // 397

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> deferral = completion;
		HRESULT hrInner = ReportResultUIThread(reason, ntStatus, ntSubStatus, samCompatibleUserNameRef->Get(), displayNameRef->Get(), userSidRef->Get(), deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 409
	return S_OK;
}

HRESULT LogonViewManager::ClearCredentialState()
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 415

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		ClearCredentialStateUIThread();
	});
	RETURN_IF_FAILED(hr); // 421
	return S_OK;
}

HRESULT LogonViewManager::DisplayStatus(LC::LogonUIState state, HSTRING status, WI::AsyncDeferral<WI::CNoResult> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 445

	REFCOUNT_HSTRING_REF(statusRef);

	if (status)
	{
		RETURN_IF_FAILED(statusRef->Set(status)); // 451
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CNoResult> deferral = completion;
		HRESULT hrInner = DisplayStatusUIThread(state, statusRef->Get(), deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 464
	return S_OK;
}

HRESULT LogonViewManager::DisplayMessage(
	LC::LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 470

	REFCOUNT_HSTRING_REF(captionRef);
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 475
	}

	REFCOUNT_HSTRING_REF(messageRef);
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 481
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> deferral = completion;
		HRESULT hrInner = DisplayMessageUIThread(messageMode, messageBoxFlags, captionRef->Get(), messageRef->Get(), deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 494
	return S_OK;
}

HRESULT LogonViewManager::DisplayCredentialError(
	NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 500

	REFCOUNT_HSTRING_REF(captionRef);
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 505
	}

	REFCOUNT_HSTRING_REF(messageRef);
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 511
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> deferral = completion;
		HRESULT hrInner = DisplayCredentialErrorUIThread(ntsStatus, ntsSubStatus, messageBoxFlags, captionRef->Get(), messageRef->Get(), deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 524
	return S_OK;
}

HRESULT LogonViewManager::ShowSecurityOptions(
	LC::LogonUISecurityOptions options,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 427

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> deferral = completion;
		HRESULT hrInner = ShowSecurityOptionsUIThread(options, deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 439
	return S_OK;
}

HRESULT LogonViewManager::Cleanup(WI::AsyncDeferral<WI::CNoResult> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 530

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [=, completion]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CNoResult> deferral = completion;
		HRESULT hrInner = CleanupUIThread(deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 542
	return S_OK;
}

HRESULT LogonViewManager::SetContextUIThread(
	IInspectable* autoLogonManager, LC::IUserSettingManager* userSettingManager,
	LC::IRedirectionManager* redirectionManager, LCPD::IDisplayStateProvider* displayStateProvider,
	LC::IBioFeedbackListener* bioFeedbackListener)
{
	m_autoLogonManager = autoLogonManager;
	m_userSettingManager = userSettingManager;
	m_redirectionManager = redirectionManager;
	m_displayStateProvider = displayStateProvider;
	m_bioFeedbackListener = bioFeedbackListener;

	LANGID langID = 0;
	RETURN_IF_FAILED(m_userSettingManager->get_LangID(&langID)); // 556

	if (langID)
	{
		SetThreadUILanguage(langID);
	}

	RETURN_IF_FAILED(CoCreateInstance(__uuidof(IInputSwitchControl), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_inputSwitchControl))); // 563
	RETURN_IF_FAILED(m_inputSwitchControl->Init(ISCT_IDL_LOGONUI)); // 564
	m_inputSwitchControl->RegisterHotkeys();
	return S_OK;
}

HRESULT LogonViewManager::LockUIThread(
	LC::LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, LC::IUnlockTrigger* unlockTrigger)
{
	m_requestCredentialsComplete.reset();

	RETURN_IF_FAILED(DestroyCurrentView()); // 573
	m_currentReason = reason;
	m_unlockTrigger = unlockTrigger;

	ComPtr<LockedView> lockView;
	RETURN_IF_FAILED(Details::MakeAndInitialize<LockedView>(&lockView)); // 578

	RETURN_IF_FAILED(lockView->LockedView::Advise(this)); // 580

	RETURN_IF_FAILED(SetActiveView(lockView.Get())); // 582

	m_currentView.Swap(lockView.Get());
	m_currentViewType = LogonView::Locked;
	m_showCredentialViewOnInitComplete = false;

	RETURN_IF_FAILED(StartCredProvsIfNecessary(reason, allowDirectUserSwitching)); // 588

	return S_OK;
}

HRESULT LogonViewManager::RequestCredentialsUIThread(
	LC::LogonUIRequestReason reason, LC::LogonUIFlags flags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> completion)
{
	auto completeOnFailure = wil::scope_exit([this]() -> void { m_requestCredentialsComplete->Complete(E_UNEXPECTED); });

	m_unlockTrigger.Reset();
	m_currentReason = reason;
	m_requestCredentialsComplete = wil::make_unique_nothrow<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>>>(completion);
	RETURN_IF_NULL_ALLOC(m_requestCredentialsComplete); // 598

	if (m_cachedSerialization.Get())
	{
		ComPtr<LC::IRequestCredentialsDataFactory> factory;
		RETURN_IF_FAILED(WF::GetActivationFactory(
			Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_RequestCredentialsData).Get(),
			&factory)); // 610

		ComPtr<LC::IRequestCredentialsData> data;
		RETURN_IF_FAILED(factory->CreateRequestCredentialsData(m_cachedSerialization.Get(), LC::LogonUIShutdownChoice_None, &data)); // 613

		RETURN_IF_FAILED(m_requestCredentialsComplete->GetResult().Set(data.Get())); // 615

		m_requestCredentialsComplete->Complete(S_OK);
		m_requestCredentialsComplete.reset();
		m_cachedSerialization.Reset();
	}
	else
	{
		m_showCredentialViewOnInitComplete = true;
		RETURN_IF_FAILED(StartCredProvsIfNecessary(reason, (flags & LC::LogonUIFlags_AllowDirectUserSwitching) != 0)); // 623
	}

	completeOnFailure.release();
	return S_OK;
}

template <typename TDelegateInterface, typename TActionInterface, typename TLambda>
HRESULT StartOperationAndThen(TActionInterface* action, TLambda&& lambda)
{
	ComPtr<TDelegateInterface> callback = Callback<TDelegateInterface>([lambda](TActionInterface* asyncInfo, AsyncStatus asyncStatus)
	{
		HRESULT errorCode = S_OK;
		if (asyncStatus != AsyncStatus::Completed)
		{
			ComPtr<IAsyncInfo> spAsyncInfo;
			errorCode = asyncInfo->QueryInterface(IID_PPV_ARGS(&spAsyncInfo));
			if (SUCCEEDED(errorCode))
				spAsyncInfo->get_ErrorCode(&errorCode);
		}
		lambda(errorCode);
		return S_OK;
	});

	HRESULT hr;
	if (callback.Get())
		hr = action->put_Completed(callback.Get());
	else
		hr = E_OUTOFMEMORY;

	return hr;
}

HRESULT LogonViewManager::ReportResultUIThread(
	LC::LogonUIRequestReason reason, NTSTATUS ntStatus, NTSTATUS ntSubStatus, HSTRING samCompatibleUserName,
	HSTRING displayName, HSTRING userSid,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completion)
{
	m_unlockTrigger.Reset();
	m_requestCredentialsComplete.reset();

	ComPtr<WF::IAsyncOperation<LCPD::ReportResultInfo*>> asyncOp;
	RETURN_IF_FAILED(m_credProvDataModel->ReportResultAsync(ntStatus, ntSubStatus, userSid, &asyncOp)); // 635

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = StartOperationAndThen<WF::IAsyncOperationCompletedHandler<LCPD::ReportResultInfo*>>(asyncOp.Get(), [completion, thisRef, this](HRESULT hrAction, WF::IAsyncOperation<LCPD::ReportResultInfo*>* asyncOp) -> HRESULT
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completionRef = completion;
		auto completeOnFailure = wil::scope_exit([&completionRef]() -> void { completionRef.Complete(E_UNEXPECTED); });
		RETURN_IF_FAILED(hrAction);

		ComPtr<LCPD::IReportResultInfo> resultInfo;
		RETURN_IF_FAILED(asyncOp->GetResults(&resultInfo));

		Wrappers::HString statusMessage;
		RETURN_IF_FAILED(resultInfo->get_StatusMessage(statusMessage.ReleaseAndGetAddressOf()));

		ComPtr<LC::IReportCredentialsDataFactory> factory;
		RETURN_IF_FAILED(WF::GetActivationFactory(
			Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_ReportCredentialsData).Get(), &factory));

		ComPtr<LC::IReportCredentialsData> resultData;
		RETURN_IF_FAILED(factory->CreateReportCredentialsData(LC::LogonUICredProvResponse_LogonUIResponseDefault, statusMessage.Get(), &resultData)); // 667

		m_lastReportResultInfo = resultInfo;
		completionRef.GetResult().Set(resultData.Get());
		completionRef.Complete(S_OK);
		completeOnFailure.release();
		return S_OK;
	});
	RETURN_IF_FAILED(hr); // 667
	return S_OK;
}

HRESULT LogonViewManager::ShowSecurityOptionsUIThread(
	LC::LogonUISecurityOptions options,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::ILogonUISecurityOptionsResult>> completion)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 686

	ComPtr<SecurityOptionsView> view;
	RETURN_IF_FAILED(MakeAndInitialize<SecurityOptionsView>(&view, options, completion)); // 689

	RETURN_IF_FAILED(view->SecurityOptionsView::Advise(this)); // 691

	RETURN_IF_FAILED(SetActiveView(view.Get())); // 693

	m_currentView.Swap(view.Get());
	m_currentViewType = LogonView::SecurityOptions;
	return S_OK;
}

HRESULT LogonViewManager::DisplayStatusUIThread(LC::LogonUIState state, HSTRING status, WI::AsyncDeferral<WI::CNoResult> completion)
{
	if (status)
	{
		LC::LogonErrorRedirectorResponse errorResponse;
		RETURN_IF_FAILED(m_redirectionManager->RedirectStatus(status, &errorResponse)); // 706

		if (errorResponse != LC::LogonErrorRedirectorResponse_HandledDoNotShowLocally
			&& errorResponse != LC::LogonErrorRedirectorResponse_HandledDoNotShowLocallyStartOver)
		{
			RETURN_IF_FAILED(m_redirectionManager->OnBeginPainting()); // 716
			RETURN_IF_FAILED(ShowStatusView(status)); // 717
		}
	}

	completion.Complete(S_OK);
	return S_OK;
}

HRESULT LogonViewManager::DisplayMessageUIThread(
	LC::LogonMessageMode messageMode, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	auto completeOnFailure = wil::scope_exit([&completion]() -> void { completion.Complete(E_UNEXPECTED); });

	UINT redirectResult;
	LC::LogonErrorRedirectorResponse errorResponse;
	RETURN_IF_FAILED(m_redirectionManager->RedirectMessage(caption, message, messageBoxFlags, &redirectResult, &errorResponse)); // 733

	if (errorResponse == LC::LogonErrorRedirectorResponse_HandledDoNotShowLocally
		|| errorResponse == LC::LogonErrorRedirectorResponse_HandledDoNotShowLocallyStartOver)
	{
		ComPtr<LC::IMessageDisplayResultFactory> factory;
		RETURN_IF_FAILED(WF::GetActivationFactory<ComPtr<LC::IMessageDisplayResultFactory>>(
			Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_MessageDisplayResult).Get(), &factory)); // 741

		ComPtr<LC::IMessageDisplayResult> messageResult;
		RETURN_IF_FAILED(factory->CreateMessageDisplayResult(redirectResult, &messageResult)); // 744

		WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult> result = completion.GetResult(); // @Note: This is a copy of `completion`, not a reference, and is set below???
		RETURN_IF_FAILED(result.Set(messageResult.Get())); // 747

		completion.Complete(S_OK);
	}
	else
	{
		RETURN_IF_FAILED(m_redirectionManager->OnBeginPainting()); // 754
		RETURN_IF_FAILED(ShowMessageView(caption, message, messageBoxFlags, completion)); // 755
	}

	completeOnFailure.release();
	return S_OK;
}

HRESULT LogonViewManager::DisplayCredentialErrorUIThread(
	NTSTATUS ntsStatus, NTSTATUS ntsSubStatus, UINT messageBoxFlags, HSTRING caption, HSTRING message,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	auto completeOnFailure = wil::scope_exit([&completion]() -> void { completion.Complete(E_UNEXPECTED); });

	if (m_lastReportResultInfo.Get())
	{
		LCPD::CredentialProviderStatusIcon statusIcon;
		RETURN_IF_FAILED(m_lastReportResultInfo->get_StatusIcon(&statusIcon)); // 775
	}

	m_lastReportResultInfo.Reset();

	UINT redirectResult;
	LC::LogonErrorRedirectorResponse errorResponse;
	RETURN_IF_FAILED(m_redirectionManager->RedirectLogonError(
		ntsStatus, ntsSubStatus, caption, message, messageBoxFlags, &redirectResult, &errorResponse)); // 796

	if (errorResponse == LC::LogonErrorRedirectorResponse_HandledDoNotShowLocally
		|| errorResponse == LC::LogonErrorRedirectorResponse_HandledDoNotShowLocallyStartOver)
	{
		ComPtr<LC::IMessageDisplayResultFactory> factory;
		RETURN_IF_FAILED(WF::GetActivationFactory(
			Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_Controller_MessageDisplayResult).Get(), &factory)); // 804

		ComPtr<LC::IMessageDisplayResult> messageResult;
		RETURN_IF_FAILED(factory->CreateMessageDisplayResult(redirectResult, &messageResult)); // 807

		Windows::Internal::CMarshaledInterfaceResult<LC::IMessageDisplayResult> result = completion.GetResult(); // @Note: This is a copy of `completion`, not a reference, and is set below???
		RETURN_IF_FAILED(result.Set(messageResult.Get())); // 810

		completion.Complete(S_OK);
	}
	else
	{
		RETURN_IF_FAILED(m_redirectionManager->OnBeginPainting()); // 816
		RETURN_IF_FAILED(ShowMessageView(caption, message, messageBoxFlags, completion)); // 817
	}

	completeOnFailure.release();
	return S_OK;
}

HRESULT LogonViewManager::ClearCredentialStateUIThread()
{
	if (m_credProvDataModel.Get())
	{
		RETURN_IF_FAILED(m_credProvDataModel->ClearState()); // 676
		m_isCredentialResetRequired = true;
		m_cachedSerialization.Reset();
		m_showCredentialViewOnInitComplete = false;
	}

	return S_OK;
}

HRESULT LogonViewManager::CleanupUIThread(WI::AsyncDeferral<WI::CNoResult> completion)
{
	m_bioFeedbackListener.Reset();

	if (m_credProvDataModel.Get())
	{
		RETURN_IF_FAILED(m_credProvDataModel->remove_BioFeedbackStateChange(m_bioFeedbackStateChangeToken)); // 830
		RETURN_IF_FAILED(m_credProvDataModel->remove_SerializationComplete(m_serializationCompleteToken)); // 831

		ComPtr<WFC::IObservableVector<IInspectable*>> usersAndV1Creds;
		RETURN_IF_FAILED(m_credProvDataModel->get_UsersAndV1Credentials(&usersAndV1Creds)); // 834
		RETURN_IF_FAILED(usersAndV1Creds->remove_VectorChanged(m_usersChangedToken)); // 835
		RETURN_IF_FAILED(m_credProvDataModel->remove_SelectedUserChanged(m_selectedUserChangeToken)); // 836
	}

	m_inputSwitchControl.Reset();

	RETURN_IF_FAILED(DestroyCurrentView()); // 841
	completion.Complete(S_OK);
	return S_OK;
}

HRESULT LogonViewManager::ShowMessageView(
	HSTRING caption, HSTRING message, UINT messageBoxFlags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
}
