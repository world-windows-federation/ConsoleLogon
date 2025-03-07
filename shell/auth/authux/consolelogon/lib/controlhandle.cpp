#include "controlhandle.h"
#include <wil\resource.h>

using namespace Microsoft::WRL;

ControlHandle::ControlHandle()
	: m_offsetFromRoot(0)
	, m_size(0)
	, m_index(0)
{
}

ControlHandle::~ControlHandle()
{
}

HRESULT ControlHandle::RuntimeClassInitialize(UINT offset, UINT size, UINT index, IConsoleUIControl* controlCallback)
{
	m_offsetFromRoot = offset;
	m_controlCallback = controlCallback;
	m_size = size;
	m_index = index;

	return S_OK;
}

UINT ControlHandle::GetOffsetFromRoot()
{
	return m_offsetFromRoot;
}

HRESULT ControlHandle::UpdateOffsetFromRoot(UINT offset)
{
	m_offsetFromRoot = offset;
	return S_OK;
}

UINT ControlHandle::GetSize()
{
	return m_size;
}

HRESULT ControlHandle::SetSize(UINT size)
{
	m_size = size;
	return S_OK;
}

BOOL ControlHandle::IsFocusable()
{
	return m_controlCallback->IsFocusable() && m_size;
}

HRESULT ControlHandle::SetFocus(BOOL hasFocus)
{
	return m_controlCallback->OnFocusChange(hasFocus);
}

UINT ControlHandle::GetIndexInTable()
{
	return m_index;
}

HRESULT ControlHandle::HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = 0;
	return m_controlCallback->HandleKeyInput(keyEvent, wasHandled);
}

HRESULT ControlHandle::Destroy()
{
	 RETURN_IF_FAILED(m_controlCallback->Unadvise());
	 return S_OK;
}
