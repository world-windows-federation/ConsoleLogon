#include "pch.h"

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>
#include <wil/token_helpers.h>
#include <Shlwapi.h>

#include "InternalAsync.h"
#include "logoninterfaces.h"
#include "consoleuimanager.h"
#include "RefCountedObject.h"

ConsoleUIManager::ConsoleUIManager()
    : m_continueProcessingInput(true)
    , m_UIThreadInitResult(E_FAIL)
{
}

HRESULT ConsoleUIManager::Initialize()
{
}

HRESULT ConsoleUIManager::StartUI()
{
    auto scopeExit = wil::scope_exit([this]() -> void { StopUI(); });
    
    Microsoft::WRL::Wrappers::Details::SyncLockExclusive lock = m_lock.LockExclusive();
    
    if (!m_UIThreadHandle)
    {
        wil::unique_handle quitEvent(CreateEventExW(0,0,CREATE_EVENT_MANUAL_RESET,EVENT_ALL_ACCESS));
        RETURN_LAST_ERROR_IF_NULL(quitEvent); //43

        m_UIThreadQuitEvent = std::move(quitEvent);
        
        RETURN_IF_WIN32_BOOL_FALSE(SHCreateThreadWithHandle(
            s_UIThreadHostThreadProc,
            this,
            CTF_COINIT,
            s_UIThreadHostStartThreadProc,
            &m_UIThreadHandle)); //52
    }
    
    scopeExit.release();
    return S_OK;
}

HRESULT ConsoleUIManager::StopUI()
{
    Microsoft::WRL::Wrappers::Details::SyncLockExclusive lock = m_lock.LockExclusive();
    if (m_UIThreadHandle)
    {
        DWORD dwIndex = 0;
        
        SetEvent(m_UIThreadQuitEvent.get());
        CoWaitForMultipleHandles(COWAIT_DISPATCH_WINDOW_MESSAGES|COWAIT_DISPATCH_CALLS|COWAIT_ALERTABLE, 0xFFFFFFFF, 1, m_UIThreadHandle.addressof(), &dwIndex);
        m_UIThreadHandle.reset();
        ResetEvent(m_UIThreadQuitEvent.get());
        m_UIThreadInitResult = E_FAIL;
    }

    return S_OK;
}

HRESULT ConsoleUIManager::SetActiveView(IConsoleUIView* view)
{
    Microsoft::WRL::ComPtr<IConsoleUIViewInternal> newView;
    void* screenBuffer = nullptr;
    
    HRESULT hr = view->QueryInterface(IID_PPV_ARGS(&newView));
    RETURN_IF_FAILED(hr); // 105
    
    hr = newView->GetScreenBuffer(&screenBuffer);
    RETURN_IF_FAILED(hr); // 108
    
    RETURN_IF_WIN32_BOOL_FALSE(SetConsoleActiveScreenBuffer(screenBuffer)); // 110
    RETURN_IF_FAILED(hr = newView->InitializeFocus()); // 111

    m_activeView = newView;

    return S_OK;
}

HRESULT ConsoleUIManager::EnsureUIStarted()
{
    RETURN_HR_IF(E_ABORT, !m_Dispatcher.Get());
    return S_OK;
}

//TODO: double check this has been done correctly
HRESULT ConsoleUIManager::HandleIncomingInput(INPUT_RECORD input)
{
    if (input.EventType == KEY_EVENT && (m_activeView.Get() && input.Event.KeyEvent.bKeyDown))
        RETURN_IF_FAILED(HandleKeyboardInput(input.Event.KeyEvent)); // 129
}

HRESULT ConsoleUIManager::HandleKeyboardInput(KEY_EVENT_RECORD keyRecord)
{
    RETURN_IF_FAILED(m_activeView->HandleKeyInput(&keyRecord)); // 147
    return S_OK;
}

DWORD ConsoleUIManager::s_UIThreadHostStartThreadProc(void* parameter)
{
    auto pThis = static_cast<ConsoleUIManager*>(parameter);
    pThis->AddRef();
    pThis->UIThreadHostStartThreadProc();
}

HRESULT ConsoleUIManager::UIThreadHostStartThreadProc()
{
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
}
