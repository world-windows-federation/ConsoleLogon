#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include "consoleuiview.h"
#include "logoninterfaces.h"

class ConsoleUIManager 
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IUnknown>
{
public:
	ConsoleUIManager();

	HRESULT Initialize();
	HRESULT StartUI();
	HRESULT StopUI();

protected:
	~ConsoleUIManager() = default;

	HRESULT SetActiveView(IConsoleUIView*);
	HRESULT EnsureUIStarted();

	Microsoft::WRL::ComPtr<INotificationDispatcher> m_Dispatcher;

private:
	HRESULT HandleIncomingInput(INPUT_RECORD);
	HRESULT HandleKeyboardInput(KEY_EVENT_RECORD);

	Microsoft::WRL::ComPtr<IConsoleUIViewInternal> m_activeView;

	static DWORD WINAPI s_UIThreadHostStartThreadProc(void*);
	HRESULT UIThreadHostStartThreadProc();

	static DWORD WINAPI s_UIThreadHostThreadProc(void*);
	DWORD UIThreadHostThreadProc();

	Microsoft::WRL::Wrappers::SRWLock m_lock;
	bool m_continueProcessingInput;
	wil::unique_handle m_UIThreadHandle;
	HRESULT m_UIThreadInitResult;
	wil::unique_handle m_UIThreadQuitEvent;
};
