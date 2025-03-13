#include "pch.h"

#include "logonviewmanager.h"

#include <WtsApi32.h>

#include "comboboxselectionview.h"
#include "credprovselectionview.h"
#include "EventDispatcher.h"
#include "lockedview.h"
#include "messageview.h"
#include "optionaldependencyprovider.h"
#include "securityoptionsview.h"
#include "selectedcredentialview.h"
#include "serializationfailedview.h"
#include "statusview.h"
#include "userselectionview.h"

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
			RETURN_IF_FAILED(factory->CreateRequestCredentialsData(args, LC::LogonUIShutdownChoice_None, nullptr, &data)); // 63

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
					RETURN_IF_FAILED(caption.Initialize(HINST_THISCOMPONENT, IDS_WINSECURITY)); // 108

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
						RETURN_IF_FAILED(ShowSerializationFailedView(nullptr, statusMessage.Get())); // 133
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
				RETURN_IF_FAILED(factory->CreateRequestCredentialsData(nullptr, LC::LogonUIShutdownChoice_None, nullptr, &data)); // 155

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
					//RETURN_IF_FAILED(((LCPD::ICredentialGroup*)selectedUserOrV1.Get())->put_SelectedCredential(nullptr)); // 265 @Note WTF??? Why cast like this?
					RETURN_IF_FAILED(group->put_SelectedCredential(nullptr)); //@Mod, fix this
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

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, autoLogonManagerRef, userSettingManagerRef, redirectionManagerRef, displayStateProviderRef, bioFeedbackListenerRef, &setContextCompleteEvent]() -> void
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
	LC::LogonUIRequestReason reason, LC::LogonUIFlags flags, HSTRING unk,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 370

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, reason, flags, completion,unk]() -> void
	{
		UNREFERENCED_PARAMETER(thisRef);
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IRequestCredentialsData>> deferral = completion;
		HRESULT hrInner = RequestCredentialsUIThread(reason, flags, unk, deferral);
		if (FAILED(hrInner))
		{
			deferral.Complete(hrInner);
		}
	});
	RETURN_IF_FAILED(hr); // 382
	return S_OK;
}

HRESULT LogonViewManager::ReportResult(
	LC::LogonUIRequestReason reason, NTSTATUS ntStatus, NTSTATUS ntSubStatus, HSTRING samCompatibleUserName,
	HSTRING displayName, HSTRING userSid,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>> completion)
{
	RETURN_IF_FAILED(EnsureUIStarted()); // 388

	ComPtr<CRefCountedObject<Wrappers::HString>> samCompatibleUserNameRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(samCompatibleUserNameRef->Set(samCompatibleUserName)); // 391

	ComPtr<CRefCountedObject<Wrappers::HString>> displayNameRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(displayNameRef->Set(displayName)); // 394

	ComPtr<CRefCountedObject<Wrappers::HString>> userSidRef = CreateRefCountedObj<Wrappers::HString>();
	RETURN_IF_FAILED(userSidRef->Set(userSid)); // 397

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, reason, ntStatus, ntSubStatus, samCompatibleUserNameRef, displayNameRef, userSidRef, completion]() -> void
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

	ComPtr<CRefCountedObject<Wrappers::HString>> statusRef = CreateRefCountedObj<Wrappers::HString>();

	if (status)
	{
		RETURN_IF_FAILED(statusRef->Set(status)); // 451
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, state, statusRef, completion]() -> void
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

	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef = CreateRefCountedObj<Wrappers::HString>();
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 475
	}

	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef = CreateRefCountedObj<Wrappers::HString>();
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 481
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, messageMode, messageBoxFlags, captionRef, messageRef, completion]() -> void
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

	ComPtr<CRefCountedObject<Wrappers::HString>> captionRef = CreateRefCountedObj<Wrappers::HString>();
	if (caption)
	{
		RETURN_IF_FAILED(captionRef->Set(caption)); // 505
	}

	ComPtr<CRefCountedObject<Wrappers::HString>> messageRef = CreateRefCountedObj<Wrappers::HString>();
	if (message)
	{
		RETURN_IF_FAILED(messageRef->Set(message)); // 511
	}

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, ntsStatus, ntsSubStatus, messageBoxFlags, captionRef, messageRef, completion]() -> void
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

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, options, completion]() -> void
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

	HRESULT hr = BeginInvoke(m_Dispatcher.Get(), [thisRef, this, completion]() -> void
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

	RETURN_IF_FAILED(CoCreateInstance(CLSID_InputSwitchControl, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_inputSwitchControl))); // 563
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
	RETURN_IF_FAILED(MakeAndInitialize<LockedView>(&lockView)); // 578

	RETURN_IF_FAILED(lockView->Advise(this)); // 580

	RETURN_IF_FAILED(SetActiveView(lockView.Get())); // 582

	m_currentView.Swap(lockView.Get());
	m_currentViewType = LogonView::Locked;
	m_showCredentialViewOnInitComplete = false;

	RETURN_IF_FAILED(StartCredProvsIfNecessary(reason, allowDirectUserSwitching,0)); // 588

	return S_OK;
}

HRESULT LogonViewManager::RequestCredentialsUIThread(
	LC::LogonUIRequestReason reason, LC::LogonUIFlags flags,HSTRING unk,
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
		RETURN_IF_FAILED(factory->CreateRequestCredentialsData(m_cachedSerialization.Get(), LC::LogonUIShutdownChoice_None, nullptr, &data)); // 613

		RETURN_IF_FAILED(m_requestCredentialsComplete->GetResult().Set(data.Get())); // 615

		m_requestCredentialsComplete->Complete(S_OK);
		m_requestCredentialsComplete.reset();
		m_cachedSerialization.Reset();
	}
	else
	{
		m_showCredentialViewOnInitComplete = true;
		RETURN_IF_FAILED(StartCredProvsIfNecessary(reason, (flags & LC::LogonUIFlags_AllowDirectUserSwitching) != 0, unk)); // 623
	}

	completeOnFailure.release();
	return S_OK;
}

template <typename TDelegateInterface, typename TOperation, typename TFunc>
HRESULT StartOperationAndThen(TOperation* pOperation, TFunc&& func)
{
	ComPtr<TDelegateInterface> spCallback = Callback<TDelegateInterface>([func](TOperation* pOperation, AsyncStatus status)
	{
		HRESULT hr = S_OK;
		if (status != AsyncStatus::Completed)
		{
			ComPtr<IAsyncInfo> spAsyncInfo;
			hr = pOperation->QueryInterface(IID_PPV_ARGS(&spAsyncInfo));
			if (SUCCEEDED(hr))
			{
				spAsyncInfo->get_ErrorCode(&hr);
			}
		}
		func(hr, pOperation);
		return S_OK;
	});

	HRESULT hr;
	if (spCallback.Get())
	{
		hr = pOperation->put_Completed(spCallback.Get());
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}

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
		WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>>& completionRef = const_cast<WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IReportCredentialsData>>&>(completion);
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

		//WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult> result = completion.GetResult(); // @Note: This is a copy of `completion`, not a reference, and is set below???
		RETURN_IF_FAILED(completion.GetResult().Set(messageResult.Get())); // 747

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

HRESULT LogonViewManager::ShowCredentialView()
{
	if (m_credentialsChangedToken.value)
	{
		ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
		RETURN_IF_FAILED(m_selectedGroup->get_Credentials(&credentials)); // 852
		RETURN_IF_FAILED(credentials->remove_VectorChanged(m_credentialsChangedToken)); // 853
		m_credentialsChangedToken.value = 0;
	}

	if (m_selectedCredentialChangedToken.value)
	{
		RETURN_IF_FAILED(m_selectedGroup->remove_SelectedCredentialChanged(m_selectedCredentialChangedToken)); // 859
		m_selectedCredentialChangedToken.value = 0;
	}

	ComPtr<IInspectable> selectedUserOrV1;
	RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUserOrV1Credential(&selectedUserOrV1)); // 864
	if (selectedUserOrV1.Get())
	{
		ComPtr<LCPD::IUser> selectedUser;
		ComPtr<LCPD::ICredential> selectedCredential;
		Wrappers::HString userName;

		if (SUCCEEDED(selectedUserOrV1.As(&selectedUser)))
		{
			RETURN_IF_FAILED(selectedUser->get_DisplayName(userName.ReleaseAndGetAddressOf())); // 873

			RETURN_IF_FAILED(selectedUser.As(&m_selectedGroup)); // 875

			ComPtr<WFC::IObservableVector<LCPD::Credential*>> credentials;
			RETURN_IF_FAILED(m_selectedGroup->get_Credentials(&credentials)); // 878
			RETURN_IF_FAILED(credentials->add_VectorChanged(this, &m_credentialsChangedToken)); // 879

			RETURN_IF_FAILED(m_selectedGroup->add_SelectedCredentialChanged(this, &m_selectedCredentialChangedToken)); // 881

			RETURN_IF_FAILED(m_selectedGroup->get_SelectedCredential(&selectedCredential)); // 883
		}
		else
		{
			m_selectedGroup.Reset();

			RETURN_IF_FAILED(selectedUserOrV1.As(&selectedCredential)); // 889
			RETURN_IF_FAILED(selectedCredential->get_LogoLabel(userName.ReleaseAndGetAddressOf())); // 890

			if (!userName.Get())
			{
				CoTaskMemNativeString defaultV1Label;
				RETURN_IF_FAILED(defaultV1Label.Initialize(HINST_THISCOMPONENT, IDS_USER)); // 895
				RETURN_IF_FAILED(defaultV1Label.Get() ? userName.Set(defaultV1Label.Get()) : E_POINTER); // 896
			}
		}

		if (selectedCredential.Get())
		{
			RETURN_IF_FAILED(ShowSelectedCredentialView(selectedCredential.Get(), userName.Get())); // 902
		}
		else
		{
			RETURN_IF_FAILED(ShowCredProvSelection(m_selectedGroup.Get(), userName.Get())); // 906
		}
	}
	else
	{
		RETURN_IF_FAILED(ShowUserSelection()); // 911
	}

	return S_OK;
}

HRESULT LogonViewManager::ShowUserSelection()
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 918

	ComPtr<UserSelectionView> userSelectionView;
	RETURN_IF_FAILED(MakeAndInitialize<UserSelectionView>(&userSelectionView, m_credProvDataModel.Get())); // 921

	RETURN_IF_FAILED(userSelectionView->UserSelectionView::Advise(this)); // 923

	RETURN_IF_FAILED(SetActiveView(userSelectionView.Get())); // 925

	m_currentView.Swap(userSelectionView.Get());
	m_currentViewType = LogonView::UserSelection;
	return S_OK;
}

HRESULT LogonViewManager::ShowCredProvSelection(LCPD::ICredentialGroup* group, HSTRING userName)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 935

	ComPtr<CredProvSelectionView> credProvSelectionView;
	RETURN_IF_FAILED(MakeAndInitialize<CredProvSelectionView>(&credProvSelectionView, group, userName)); // 938

	RETURN_IF_FAILED(credProvSelectionView->CredProvSelectionView::Advise(this)); // 940

	RETURN_IF_FAILED(SetActiveView(credProvSelectionView.Get())); // 942

	m_currentView.Swap(credProvSelectionView.Get());
	m_currentViewType = LogonView::CredProvSelection;
	return S_OK;
}

HRESULT LogonViewManager::ShowSelectedCredentialView(LCPD::ICredential* credential, HSTRING userName)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 952

	ComPtr<SelectedCredentialView> selectedCredentialView;
	RETURN_IF_FAILED(MakeAndInitialize<SelectedCredentialView>(&selectedCredentialView, m_currentReason, credential, userName)); // 955

	RETURN_IF_FAILED(selectedCredentialView->SelectedCredentialView::Advise(this)); // 957

	RETURN_IF_FAILED(SetActiveView(selectedCredentialView.Get())); // 959

	m_currentView.Swap(selectedCredentialView.Get());
	m_currentViewType = LogonView::SelectedCredential;
	return S_OK;
}

HRESULT LogonViewManager::ShowStatusView(HSTRING status)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 969

	ComPtr<LCPD::IUser> selectedUser;
	if (m_credProvDataModel.Get())
	{
		ComPtr<IInspectable> selectedUserOrV1;
		RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUserOrV1Credential(&selectedUserOrV1)); // 975

		if (selectedUserOrV1.Get())
		{
			selectedUserOrV1.As(&selectedUser);
		}
	}

	ComPtr<StatusView> statusView;
	RETURN_IF_FAILED(MakeAndInitialize<StatusView>(&statusView, status, selectedUser.Get())); // 983

	RETURN_IF_FAILED(statusView->StatusView::Advise(this)); // 985

	RETURN_IF_FAILED(SetActiveView(statusView.Get())); // 987

	m_currentView.Swap(statusView.Get());
	m_currentViewType = LogonView::Status;
	return S_OK;
}

HRESULT LogonViewManager::ShowMessageView(
	HSTRING caption, HSTRING message, UINT messageBoxFlags,
	WI::AsyncDeferral<WI::CMarshaledInterfaceResult<LC::IMessageDisplayResult>> completion)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 997
	ComPtr<LCPD::IUser> selectedUser;
	if (m_credProvDataModel.Get())
	{
		ComPtr<IInspectable> selectedUserOrV1;
		RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUserOrV1Credential(&selectedUserOrV1)); // 1002
		if (selectedUserOrV1.Get())
		{
			selectedUserOrV1.As(&selectedUser);
		}
	}

	ComPtr<MessageView> messageView;
	RETURN_IF_FAILED(MakeAndInitialize<MessageView>(&messageView, caption, message, messageBoxFlags, completion, selectedUser.Get())); // 1010

	RETURN_IF_FAILED(messageView->MessageView::Advise(this)); // 1012

	RETURN_IF_FAILED(SetActiveView(messageView.Get())); // 1014

	m_currentView.Swap(messageView);
	m_currentViewType = LogonView::Message;
	return S_OK;
}

// ComPtr<LCPD::IUser> selectedUser;
// ComPtr<IInspectable> selectedUserOrV1;
HRESULT LogonViewManager::ShowSerializationFailedView(HSTRING caption, HSTRING message)
{
	RETURN_IF_FAILED(DestroyCurrentView()); // 1024

	ComPtr<LCPD::IUser> selectedUser;
	if (m_credProvDataModel.Get())
	{
		ComPtr<IInspectable> selectedUserOrV1;
		RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUserOrV1Credential(&selectedUserOrV1)); // 1029
		if (selectedUserOrV1.Get())
		{
			selectedUserOrV1.As(&selectedUser);
		}
	}

	ComPtr<SerializationFailedView> serializationFailedView;
	RETURN_IF_FAILED(MakeAndInitialize<SerializationFailedView>(&serializationFailedView, caption, message, selectedUser.Get())); // 1037

	RETURN_IF_FAILED(serializationFailedView->SerializationFailedView::Advise(this)); // 1039

	RETURN_IF_FAILED(SetActiveView(serializationFailedView.Get())); // 1041

	m_currentView.Swap(serializationFailedView.Get());
	m_currentViewType = LogonView::SerializationFailed;
	return S_OK;
}

HRESULT LogonViewManager::DestroyCurrentView()
{
	if (m_currentView.Get())
	{
		RETURN_IF_FAILED(m_currentView->Unadvise()); // 1053
		RETURN_IF_FAILED(m_currentView->RemoveAll()); // 1054
		m_currentView.Reset();
	}

	return S_OK;
}

HRESULT LogonViewManager::StartCredProvsIfNecessary(LC::LogonUIRequestReason reason, BOOLEAN allowDirectUserSwitching, HSTRING unk)
{
	LCPD::CredProvScenario scenario = LCPD::CredProvScenario_Logon;
	if (reason == LC::LogonUIRequestReason_LogonUIUnlock)
	{
		scenario = allowDirectUserSwitching ? LCPD::CredProvScenario_Logon : LCPD::CredProvScenario_Unlock;
	}
	else if (reason == LC::LogonUIRequestReason_LogonUIChange)
	{
		scenario = LCPD::CredProvScenario_ChangePassword;
	}

	if (m_credProvDataModel.Get())
	{
		if (m_isCredentialResetRequired)
		{
			ComPtr<WF::IAsyncAction> resetAction;
			RETURN_IF_FAILED(m_credProvDataModel->ResetAsync(scenario, &resetAction)); // 1124
			m_credProvInitialized = false;

			ComPtr<LogonViewManager> thisRef = this;

			HRESULT hr = StartOperationAndThen<WF::IAsyncActionCompletedHandler>(resetAction.Get(), [thisRef, this](HRESULT hrAction, WF::IAsyncAction* asyncOp) -> HRESULT
			{
				UNREFERENCED_PARAMETER(thisRef);
				auto completeOnFailure = wil::scope_exit([this]() -> void
				{
					if (m_requestCredentialsComplete)
						m_requestCredentialsComplete->Complete(E_UNEXPECTED);
				});

				if (SUCCEEDED(hrAction))
				{
					m_credProvInitialized = true;
					hrAction = ShowCredentialView();
				}

				RETURN_IF_FAILED(hrAction); // 1147
				m_isCredentialResetRequired = false;
				completeOnFailure.release();
				return S_OK;
			});
			RETURN_IF_FAILED(hr); // 1147
		}
		else if (m_credProvInitialized)
		{
			if (m_showCredentialViewOnInitComplete)
			{
				RETURN_IF_FAILED(ShowCredentialView()); // 1153
			}

			ComPtr<LCPD::IUser> selectedUser;
			RETURN_IF_FAILED(m_credProvDataModel->get_SelectedUser(&selectedUser)); // 1157

			if (selectedUser.Get())
			{
				ComPtr<LCPD::ICredentialGroup> selectedCredentialGroup;
				RETURN_IF_FAILED(selectedUser.As(&selectedCredentialGroup)); // 1166
				RETURN_IF_FAILED(selectedCredentialGroup->RefreshSelection()); // 1167
			}
		}

		return S_OK;
	}

	ComPtr<LCPD::IUIThreadEventDispatcher> eventDispatcher;
	RETURN_IF_FAILED(MakeAndInitialize<EventDispatcher>(&eventDispatcher, m_Dispatcher.Get())); // 1081

	ComPtr<LCPD::IOptionalDependencyProvider> optionalDependencyProvider;
	RETURN_IF_FAILED(MakeAndInitialize<OptionalDependencyProvider>(&optionalDependencyProvider, reason, m_autoLogonManager.Get(), m_userSettingManager.Get(), m_displayStateProvider.Get())); // 1084

	//@MOD, dont think this is needed
	ComPtr<LCPD::ITelemetryDataProvider> telemetryProvider;
	RETURN_IF_FAILED(m_userSettingManager->get_TelemetryDataProvider(&telemetryProvider)); // 1087;

	ComPtr<LCPD::ICredProvDataModelFactory> credProvDataModelFactory;
	RETURN_IF_FAILED(WF::GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_Internal_UI_Logon_CredProvData_CredProvDataModel).Get(), &credProvDataModelFactory)); // 1090
	RETURN_IF_FAILED(credProvDataModelFactory->CreateCredProvDataModel(LCPD::SelectionMode_PLAP,eventDispatcher.Get(), optionalDependencyProvider.Get(), &m_credProvDataModel)); // 1091

	RETURN_IF_FAILED(m_credProvDataModel->add_SerializationComplete(this, &m_serializationCompleteToken)); // 1093
	RETURN_IF_FAILED(m_credProvDataModel->add_BioFeedbackStateChange(this, &m_bioFeedbackStateChangeToken)); // 1094

	ComPtr<WF::IAsyncAction> initAction;
	LANGID langID = 0;
	RETURN_IF_FAILED(m_userSettingManager->get_LangID(&langID)); // 1098
	RETURN_IF_FAILED(m_credProvDataModel->InitializeAsync(scenario, langID, LCPD::SelectionMode_UserAndV1Aggregate, unk, &initAction)); // 1099

	ComPtr<LogonViewManager> thisRef = this;

	HRESULT hr = StartOperationAndThen<WF::IAsyncActionCompletedHandler>(initAction.Get(), [thisRef, this](HRESULT hrAction, WF::IAsyncAction* asyncOp) -> HRESULT
	{
		UNREFERENCED_PARAMETER(thisRef);
		auto completeOnFailure = wil::scope_exit([this]() -> void
		{
			if (m_requestCredentialsComplete)
				m_requestCredentialsComplete->Complete(E_UNEXPECTED);
		});

		if (SUCCEEDED(hrAction))
		{
			m_credProvInitialized = true;
			hrAction = OnCredProvInitComplete();
		}

		RETURN_IF_FAILED(hrAction); // 1119
		completeOnFailure.release();
		return S_OK;
	});
	RETURN_IF_FAILED(hr); // 1119
	return S_OK;
}

HRESULT LogonViewManager::OnCredProvInitComplete()
{
	ComPtr<WFC::IObservableVector<IInspectable*>> usersAndV1Creds;
	RETURN_IF_FAILED(m_credProvDataModel->get_UsersAndV1Credentials(&usersAndV1Creds)); // 1177
	RETURN_IF_FAILED(usersAndV1Creds->add_VectorChanged(this, &m_usersChangedToken)); // 1178
	RETURN_IF_FAILED(m_credProvDataModel->add_SelectedUserOrV1CredentialChanged(this, &m_selectedUserChangeToken)); // 1179
	if (m_showCredentialViewOnInitComplete)
	{
		RETURN_IF_FAILED(ShowCredentialView()); // 1182
	}

	return S_OK;
}
