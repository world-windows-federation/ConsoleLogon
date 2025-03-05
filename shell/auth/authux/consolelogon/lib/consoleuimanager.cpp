#include "pch.h"

#include <wrl/implements.h>
#include <wrl/wrappers/corewrappers.h>

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
}

HRESULT ConsoleUIManager::StopUI()
{
}

ConsoleUIManager::~ConsoleUIManager()
{
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
