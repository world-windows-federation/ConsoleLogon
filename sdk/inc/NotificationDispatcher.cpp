#include "pch.h"

#include "NotificationDispatcher.h"

#include <wil/result_macros.h>

#include "ResultUtils.h"

using namespace Microsoft::WRL;

typedef HWND (*SHCreateWorkerWindowW_t)(WNDPROC, HWND, DWORD, DWORD, HMENU, LONG_PTR); // 188
HWND SHCreateWorkerWindowW(WNDPROC wndProc, HWND hWndParent, DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, LONG_PTR wnd_extra)
{
	static SHCreateWorkerWindowW_t fn = nullptr;
	if (!fn)
	{
		HMODULE h = GetModuleHandleW(L"shcore.dll");
		if (h)
			fn = (SHCreateWorkerWindowW_t)GetProcAddress(h, MAKEINTRESOURCEA(188));
		FAIL_FAST_IF_NULL(fn);
	}
	return fn(wndProc, hWndParent, dwExStyle, dwStyle, hMenu, wnd_extra);
}

CNotificationDispatcherBase::CNotificationDispatcherBase()
	: _hwndMsgWindow(nullptr)
	, _fProcessNotifications(true)
{
}

HRESULT CNotificationDispatcherBase::QueueNotification(IDispatchNotification* pNotification)
{
	Wrappers::SRWLock::SyncLockExclusive lock = _srwLock.LockExclusive();
	HRESULT hr = S_OK;

	if (_fProcessNotifications && _hwndMsgWindow)
	{
		hr = _srgPendingNotifications.Add(pNotification);
		if (hr >= 0)
		{
			PostMessageW(_hwndMsgWindow, 0x400, 0, 0);
		}
	}

	return hr;
}

void CNotificationDispatcherBase::StartProcessingNotifications()
{
	Wrappers::SRWLock::SyncLockExclusive lock = _srwLock.LockExclusive();
	_fProcessNotifications = true;
}

void CNotificationDispatcherBase::StopProcessingNotifications()
{
	Wrappers::SRWLock::SyncLockExclusive lock = _srwLock.LockExclusive();
	_fProcessNotifications = false;
}

HRESULT CNotificationDispatcherBase::RemoveBackReferences()
{
	StopProcessingNotifications();

	HWND hwndToDestroy = _hwndMsgWindow;
	{
		Wrappers::SRWLock::SyncLockExclusive lock = _srwLock.LockExclusive();
		_hwndMsgWindow = nullptr;
	}
	if (hwndToDestroy)
	{
		DestroyWindow(hwndToDestroy);
	}

	return S_OK;
}

HRESULT CNotificationDispatcherBase::Initialize()
{
	_hwndMsgWindow = SHCreateWorkerWindowW(s_WndProc, nullptr, 0, 0, nullptr, (LONG_PTR)this);
	return ResultFromWin32Bool(_hwndMsgWindow != nullptr);
}

bool CNotificationDispatcherBase::CanProcessWork()
{
	return true;
}

CNotificationDispatcherBase::~CNotificationDispatcherBase()
{
}

LRESULT CNotificationDispatcherBase::s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CNotificationDispatcherBase* pThis = (CNotificationDispatcherBase*)GetWindowLongPtrW(hwnd, 0);
	if (pThis && uMsg == 0x400 && hwnd == pThis->_hwndMsgWindow)
	{
		bool fHaveWork = true;
		while (fHaveWork && pThis->CanProcessWork())
		{
			ComPtr<IDispatchNotification> spNotification;
			bool fProcessNotification = pThis->_fProcessNotifications;

			{
				Wrappers::SRWLock::SyncLockExclusive lock = pThis->_srwLock.LockExclusive();
				if (pThis->_srgPendingNotifications._celem)
				{
					spNotification = pThis->_srgPendingNotifications._parray[0];
					pThis->_srgPendingNotifications.RemoveAt(0);
				}
				else
				{
					fHaveWork = false;
				}
			}

			if (spNotification && fProcessNotification)
			{
				spNotification->Dispatch();
			}
		}
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

CNotificationDispatcher::CNotificationDispatcher()
{
}

HRESULT CNotificationDispatcher::RuntimeClassInitialize()
{
	return Initialize();
}

HRESULT CNotificationDispatcher::GetForCurrentThread(INotificationDispatcher** ppDispatcher)
{
	return E_NOTIMPL; // Function body unknown
}
