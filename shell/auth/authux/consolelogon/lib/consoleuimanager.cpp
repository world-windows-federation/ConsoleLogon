#include "pch.h"

#include "consoleuimanager.h"

using namespace Microsoft::WRL;

ConsoleUIManager::ConsoleUIManager()
	: m_continueProcessingInput(true)
	, m_UIThreadInitResult(E_FAIL)
{
}

HRESULT ConsoleUIManager::Initialize()
{
	// m_UIThreadQuitEvent = nullptr; // @MOD Don't need this line
	m_UIThreadQuitEvent.reset(CreateEventExW(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
	return m_UIThreadQuitEvent ? S_OK : ResultFromKnownLastError();
}

HRESULT ConsoleUIManager::StartUI()
{
	auto scopeExit = wil::scope_exit([this]() -> void { StopUI(); });

	Wrappers::SRWLock::SyncLockExclusive lock = m_lock.LockExclusive();

	if (!m_UIThreadHandle)
	{
		wil::unique_handle quitEvent(CreateEventExW(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS));
		RETURN_LAST_ERROR_IF_NULL(quitEvent); // 43

		m_UIThreadQuitEvent = std::move(quitEvent);

		RETURN_IF_WIN32_BOOL_FALSE(SHCreateThreadWithHandle(
			s_UIThreadHostThreadProc,
			this,
			CTF_COINIT,
			s_UIThreadHostStartThreadProc,
			&m_UIThreadHandle)); // 52
	}

	scopeExit.release();
	return S_OK;
}

HRESULT ConsoleUIManager::StopUI()
{
	Wrappers::SRWLock::SyncLockExclusive lock = m_lock.LockExclusive();
	if (m_UIThreadHandle)
	{
		SetEvent(m_UIThreadQuitEvent.get());

		DWORD dwIndex = 0;
		CoWaitForMultipleHandles(COWAIT_ALERTABLE | COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES, INFINITE, 1, m_UIThreadHandle.addressof(), &dwIndex);
		m_UIThreadHandle.reset();

		ResetEvent(m_UIThreadQuitEvent.get());
		m_UIThreadInitResult = E_FAIL;
	}

	return S_OK;
}

HRESULT ConsoleUIManager::SetActiveView(IConsoleUIView* view)
{
	ComPtr<IConsoleUIViewInternal> newView;
	RETURN_IF_FAILED(view->QueryInterface(IID_PPV_ARGS(&newView))); // 105

	HANDLE screenBuffer;
	RETURN_IF_FAILED(newView->GetScreenBuffer(&screenBuffer)); // 108

	RETURN_IF_WIN32_BOOL_FALSE(SetConsoleActiveScreenBuffer(screenBuffer)); // 110
	RETURN_IF_FAILED(newView->InitializeFocus()); // 111

	m_activeView = newView;

	return S_OK;
}

HRESULT ConsoleUIManager::EnsureUIStarted()
{
	RETURN_HR_IF(E_ABORT, !m_Dispatcher.Get());
	return S_OK;
}

HRESULT ConsoleUIManager::HandleIncomingInput(INPUT_RECORD input)
{
	if (input.EventType == KEY_EVENT)
	{
		RETURN_IF_FAILED(HandleKeyboardInput(input.Event.KeyEvent)); // 129
	}

	return S_OK;
}

HRESULT ConsoleUIManager::HandleKeyboardInput(KEY_EVENT_RECORD keyRecord)
{
	if (keyRecord.bKeyDown && m_activeView.Get())
	{
		RETURN_IF_FAILED(m_activeView->HandleKeyInput(&keyRecord)); // 147
	}

	return S_OK;
}

DWORD ConsoleUIManager::s_UIThreadHostStartThreadProc(void* parameter)
{
	auto pThis = static_cast<ConsoleUIManager*>(parameter);
	pThis->AddRef();
	pThis->UIThreadHostStartThreadProc();
	return 0;
}

HRESULT ConsoleUIManager::UIThreadHostStartThreadProc()
{
	HRESULT hr;
	auto scopeExit = wil::scope_exit([&]() -> void { m_UIThreadInitResult = hr; });

	RETURN_IF_FAILED(hr = MakeNotificationDispatcher<CNotificationDispatcher>(&m_Dispatcher)); // 219

	RETURN_IF_WIN32_BOOL_FALSE(AllocConsole()); // 221
	return S_OK;
}

DWORD ConsoleUIManager::s_UIThreadHostThreadProc(void* parameter)
{
	auto pThis = static_cast<ConsoleUIManager*>(parameter);

	HRESULT hr = S_OK;
	if (SUCCEEDED(pThis->m_UIThreadInitResult))
	{
		hr = pThis->UIThreadHostThreadProc();
	}
	pThis->Release();
	return hr;
}

DWORD ConsoleUIManager::UIThreadHostThreadProc()
{
	DWORD dwIndex = WAIT_IO_COMPLETION;

	HANDLE stdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE waitHandles[] = { m_UIThreadQuitEvent.get(), stdIn };

	while (dwIndex == WAIT_IO_COMPLETION)
	{
		CoWaitForMultipleHandles(
			COWAIT_ALERTABLE | COWAIT_INPUTAVAILABLE | COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES,
			INFINITE, ARRAYSIZE(waitHandles), waitHandles, &dwIndex);
		if (dwIndex == 1)
		{
			INPUT_RECORD input;
			DWORD numInputEventsRead;
			while (PeekConsoleInputW(stdIn, &input, 1, &numInputEventsRead))
			{
				if (!numInputEventsRead || !ReadConsoleInputW(stdIn, &input, 1, &numInputEventsRead))
				{
					break;
				}

				HandleIncomingInput(input);
			}

			dwIndex = WAIT_IO_COMPLETION;
		}
	}

	if (m_Dispatcher.Get())
	{
		ComPtr<IObjectWithBackReferences> objWithBackRefs;
		if (SUCCEEDED(m_Dispatcher.As(&objWithBackRefs)))
		{
			objWithBackRefs->RemoveBackReferences();
		}

		m_Dispatcher.Reset();
	}

	FreeConsole();
	return 0;
}
