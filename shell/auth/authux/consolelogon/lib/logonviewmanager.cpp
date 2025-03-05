#include "pch.h"

#include "logonviewmanager.h"

#include <WtsApi32.h>

#include "NativeString.h"
#include "ResultUtils.h"

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

	m_currentView.Swap(comboBoxView);
	m_currentViewType = LogonView::ComboBox;
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
