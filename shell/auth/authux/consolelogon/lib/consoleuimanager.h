#pragma once
#include "logoninterfaces.h"
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

class ConsoleUIManager 
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IUnknown>
{
public:
	ConsoleUIManager();
	HRESULT Initialize();
	HRESULT StartUI();
	HRESULT StopUI();
protected:
	~ConsoleUIManager();
	HRESULT SetActiveView(struct IConsoleUIView*);
	HRESULT EnsureUIStarted();
	class Microsoft::WRL::ComPtr<INotificationDispatcher> m_Dispatcher;
private:
	HRESULT HandleIncomingInput(struct _INPUT_RECORD);
	HRESULT HandleKeyboardInput(struct _KEY_EVENT_RECORD);
	class Microsoft::WRL::ComPtr<IConsoleUIViewInternal> m_activeView;
	unsigned long s_UIThreadHostStartThreadProc(void*);
	HRESULT UIThreadHostStartThreadProc();
	unsigned long s_UIThreadHostThreadProc(void*);
	unsigned long UIThreadHostThreadProc();

	class Microsoft::WRL::Wrappers::SRWLock m_lock;
	bool m_continueProcessingInput;
	class wil::unique_any_t<wil::details::unique_storage<wil::details::handle_null_resource_policy<int(__stdcall*)(void*), &CloseHandle> > > m_UIThreadHandle;
	HRESULT m_UIThreadInitResult;
	class wil::unique_any_t<wil::details::unique_storage<wil::details::handle_null_resource_policy<int(__stdcall*)(void*), &CloseHandle> > > m_UIThreadQuitEvent;
};