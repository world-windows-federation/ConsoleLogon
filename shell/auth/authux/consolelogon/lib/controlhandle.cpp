#include "controlhandle.h"
#include "consoleuiview.h"
#include <wil\resource.h>

using namespace Microsoft::WRL;

class ControlHandle
	: public RuntimeClass<RuntimeClassFlags<ClassicCom>
		, IControlHandle
	>
{
public:
	ControlHandle();
	~ControlHandle();

	//~ Begin IControlHandle Interface
	STDMETHODIMP RuntimeClassInitialize(UINT, UINT, UINT, IConsoleUIControl*);
	STDMETHODIMP_(UINT) GetOffsetFromRoot();
	STDMETHODIMP UpdateOffsetFromRoot(UINT);
	STDMETHODIMP_(UINT) GetSize();
	STDMETHODIMP SetSize(UINT);
	STDMETHODIMP_(INT) IsFocusable();
	STDMETHODIMP SetFocus(INT);
	STDMETHODIMP_(UINT) GetIndexInTable();
	STDMETHODIMP HandleKeyInput(PKEY_EVENT_RECORD, PINT);
	STDMETHODIMP Destroy();
	//~ End IControlHandle Interface

private:
	UINT m_offsetFromRoot;
	UINT m_size;
	UINT m_index;
	ComPtr<IConsoleUIControl> m_controlCallback;
};

ControlHandle::ControlHandle()
	: m_offsetFromRoot(0), 
		m_size(0), 
		m_index(0)
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

INT ControlHandle::IsFocusable()
{
	int isfocusable = m_controlCallback.Get()->IsFocusable();
	if (isfocusable)
		return m_size != 0;

	return S_OK;
}

HRESULT ControlHandle::SetFocus(INT hasFocus)
{
	return m_controlCallback.Get()->OnFocusChange(hasFocus);
}

UINT ControlHandle::GetIndexInTable()
{
	return m_index;
}

HRESULT ControlHandle::HandleKeyInput(PKEY_EVENT_RECORD keyEvent, PINT wasHandled)
{
	*wasHandled = 0;
	return m_controlCallback.Get()->HandleKeyInput(keyEvent, wasHandled);
}

HRESULT ControlHandle::Destroy()
{
	 RETURN_IF_FAILED(m_controlCallback.Get()->Unadvise());
	 return S_OK;
}