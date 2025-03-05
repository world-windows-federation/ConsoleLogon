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

HRESULT ConsoleUIManager::SetActiveView(IConsoleUIView*)
{
}

HRESULT ConsoleUIManager::EnsureUIStarted()
{
}

HRESULT ConsoleUIManager::HandleIncomingInput(INPUT_RECORD)
{
}

HRESULT ConsoleUIManager::HandleKeyboardInput(KEY_EVENT_RECORD)
{
}

DWORD ConsoleUIManager::s_UIThreadHostStartThreadProc(void*)
{
}

HRESULT ConsoleUIManager::UIThreadHostStartThreadProc()
{
}

DWORD ConsoleUIManager::s_UIThreadHostThreadProc(void*)
{
}

DWORD ConsoleUIManager::UIThreadHostThreadProc()
{
}
