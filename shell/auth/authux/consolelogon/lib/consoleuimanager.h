#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "NotificationDispatcher.h"

class ConsoleUIManager
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
		, IUnknown
	>
{
public:
	ConsoleUIManager();

	HRESULT Initialize();
	HRESULT StartUI();
	HRESULT StopUI();

protected:
	~ConsoleUIManager() = default;

	HRESULT SetActiveView(IConsoleUIView* view);
	HRESULT EnsureUIStarted();

	Microsoft::WRL::ComPtr<INotificationDispatcher> m_Dispatcher;

private:
	HRESULT HandleIncomingInput(INPUT_RECORD input);
	HRESULT HandleKeyboardInput(KEY_EVENT_RECORD keyRecord);

	Microsoft::WRL::ComPtr<IConsoleUIViewInternal> m_activeView;

	static DWORD WINAPI s_UIThreadHostStartThreadProc(void* parameter);
	HRESULT UIThreadHostStartThreadProc();

	static DWORD WINAPI s_UIThreadHostThreadProc(void* parameter);
	DWORD UIThreadHostThreadProc();

	Microsoft::WRL::Wrappers::SRWLock m_lock;
	bool m_continueProcessingInput;
	wil::unique_handle m_UIThreadHandle;
	HRESULT m_UIThreadInitResult;
	wil::unique_handle m_UIThreadQuitEvent;
};
