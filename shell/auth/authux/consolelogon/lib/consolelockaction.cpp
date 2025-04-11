#include "pch.h"
#include "consolelockaction.h"

const __declspec(selectany) _Null_terminated_ WCHAR ConsoleLockAsyncAction[] = L"Windows.Foundation.IAsyncAction Windows.Internal.UI.Logon.Controller.ConsoleLockAction";

ConsoleLockAction::ConsoleLockAction()
{
}

ConsoleLockAction::~ConsoleLockAction()
{
}

HRESULT ConsoleLockAction::RuntimeClassInitialize(HSTRING domainName, HSTRING userName, HSTRING friendlyName)
{
	RETURN_IF_FAILED(m_domainName.Set(domainName)); // 16
	RETURN_IF_FAILED(m_userName.Set(userName)); // 17
	RETURN_IF_FAILED(m_friendlyName.Set(friendlyName)); // 18
	RETURN_IF_FAILED(Start()); // 19
	//RETURN_IF_FAILED(TriggerUnlock()); // 19

	return S_OK;
}

HRESULT ConsoleLockAction::TriggerUnlock()
{
	RETURN_IF_FAILED(FireCompletion()); //26

	return S_OK;
}

HRESULT ConsoleLockAction::SyncBackstop()
{
	return S_OK;
}

HRESULT ConsoleLockAction::CheckCompletion()
{
	return S_OK;
}

HRESULT ConsoleLockAction::get_VisualOwner(LC::LockDisplayOwner* value)
{
	*value = LC::LockDisplayOwner_LogonUX;
	return S_OK;
}

HRESULT ConsoleLockAction::get_DomainName(HSTRING* value)
{
	*value = nullptr;
	if (m_domainName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_domainName.CopyTo(value)); // 47

	return S_OK;
}

HRESULT ConsoleLockAction::get_UserName(HSTRING* value)
{
	*value = nullptr;
	if (m_userName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_userName.CopyTo(value)); // 57

	return S_OK;
}

HRESULT ConsoleLockAction::get_FriendlyName(HSTRING* value)
{
	*value = nullptr;
	if (m_friendlyName.IsEmpty())
		return S_OK;

	RETURN_IF_FAILED(m_friendlyName.CopyTo(value)); // 67

	return S_OK;
}

HRESULT ConsoleLockAction::get_RequireSecureGesture(bool* value)
{
	*value = true;
	return S_OK;
}

HRESULT ConsoleLockAction::get_ShowSpeedBump(bool* value)
{
	*value = false;
	return S_OK;
}

HRESULT ConsoleLockAction::get_RequireSecureGestureString(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT ConsoleLockAction::get_SpeedBumpString(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT ConsoleLockAction::get_IsLostMode(bool* value)
{
	*value = false;
	return S_OK;
}

HRESULT ConsoleLockAction::get_LostModeMessage(HSTRING* value)
{
	*value = nullptr;
	return S_OK;
}

HRESULT ConsoleLockAction::add_UserActivity(WF::ITypedEventHandler<LC::ILockInfo*, LC::LockActivity>* handler,
                                            EventRegistrationToken* token)
{
	token->value = 0;
	RETURN_IF_FAILED(m_userActivityEvent.Add(handler,token)); // 99

	return S_OK;
}

HRESULT ConsoleLockAction::remove_UserActivity(EventRegistrationToken token)
{
	RETURN_IF_FAILED(m_userActivityEvent.Remove(token)); // 105
	return S_OK;
}

HRESULT ConsoleLockAction::put_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>* pRequestHandler)
{
	RETURN_IF_FAILED(PutOnComplete(pRequestHandler)); // 112
	return S_OK;
}

HRESULT ConsoleLockAction::get_Completed(WF::IAsyncOperationCompletedHandler<HSTRING>** ppRequestHandler)
{
	*ppRequestHandler = nullptr;
	RETURN_IF_FAILED(GetOnComplete(ppRequestHandler)); // 119

	return S_OK;
}

HRESULT ConsoleLockAction::GetResults(HSTRING* results)
{
	return S_OK;
}

HRESULT ConsoleLockAction::OnStart()
{
	return S_OK;
}

void ConsoleLockAction::OnClose()
{
}

void ConsoleLockAction::OnCancel()
{
	FireCompletion();
}
