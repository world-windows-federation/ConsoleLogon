#include "consoleuiview.h"
#include "controlhandle.h"
#include "SimpleArray.h"
#include <wil\resource.h>

class ConsoleUIView
	: public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
		IConsoleUIView,
		IConsoleUIViewInternal>
{
public:
	ConsoleUIView();
	HRESULT Initialize();
	HRESULT Advise(INavigationCallback*);
	HRESULT Unadvise();
	HRESULT AppendControl(unsigned int, IConsoleUIControl*, IUnknown**);
	HRESULT WriteOutput(IUnknown*, PCHAR_INFO, COORD, PSMALL_RECT);
	HRESULT GetColorAttributes(unsigned int*);
	HRESULT ResizeControl(IUnknown*, unsigned int);
	HRESULT RemoveAll();
	HRESULT GetConsoleWidth(unsigned int*);
	HRESULT SetCursorPos(IUnknown*, COORD, bool);
	HRESULT GetScreenBuffer(void**);
	HRESULT HandleKeyInput(PKEY_EVENT_RECORD);
	HRESULT InitializeFocus();

protected:
	HRESULT v_OnKeyInput(PKEY_EVENT_RECORD, int*);
	void v_Unadvise();
	int GetFocusIndex();
	Microsoft::WRL::ComPtr<INavigationCallback> m_navigationCallback;

private:
	HRESULT ShiftVisuals(unsigned int, int);
	HRESULT MoveFocusToNext();
	HRESULT MoveFocusToPrevious();
	wil::unique_handle m_screenBuffer;
	CCoSimpleArray<Microsoft::WRL::ComPtr<IControlHandle>, 4294967294, CSimpleArrayStandardCompareHelper<Microsoft::WRL::ComPtr<IControlHandle>>> m_controlTable;
	int m_focusIndex;
};

ConsoleUIView::ConsoleUIView()
{
	this->m_focusIndex = -1;
}

HRESULT ConsoleUIView::Initialize()
{
	m_screenBuffer = wil::unique_handle(
		CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL));

	RETURN_LAST_ERROR_IF_NULL(m_screenBuffer);
}

HRESULT ConsoleUIView::Advise(INavigationCallback* callback)
{
	m_navigationCallback = callback;
	return S_OK;
}

HRESULT ConsoleUIView::Unadvise()
{
	// TODO
	m_navigationCallback.Reset();
}

HRESULT ConsoleUIView::AppendControl(unsigned int height, IConsoleUIControl* control, IUnknown** ppControlHandle)
{
	// TODO
	// needs ControlHandle
	Microsoft::WRL::ComPtr<IControlHandle> controlHandle;
	Microsoft::WRL::ComPtr<IControlHandle> lastControl;
	*ppControlHandle = NULL;
	IControlHandle* ptr;

	UINT celem = m_controlTable._celem;
	if (celem)
	{
		ptr = this->m_controlTable._parray[celem - 1].Get();
		lastControl = ptr;

	}
}

HRESULT ConsoleUIView::WriteOutput(IUnknown*, PCHAR_INFO, COORD, PSMALL_RECT)
{
	// TODO
	// needs ControlHandle
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::GetColorAttributes(unsigned int* pAttributes)
{
	*pAttributes = NULL;

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HRESULT hr = GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo);
	RETURN_IF_FAILED(hr);
	*pAttributes = consoleInfo.wAttributes;
	return S_OK;
}

HRESULT ConsoleUIView::ResizeControl(IUnknown*, unsigned int)
{
	// TODO
	// needs ControlHandle
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::RemoveAll()
{
	for (int i = 0; i < m_controlTable._celem; ++i)
	{
		m_controlTable._parray[i].Reset();
	}
	// theres some CTSimpleArray stuff here

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HRESULT hr = GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo);
	RETURN_IF_FAILED(hr);

	DWORD numWritten;
	COORD origin;
	origin.X = 0;
	hr = FillConsoleOutputCharacterW(m_screenBuffer.get(), 0x20u, consoleInfo.dwSize.X * consoleInfo.dwSize.Y, origin, &numWritten);
	RETURN_IF_FAILED(hr);

	return S_OK;
}

HRESULT ConsoleUIView::GetConsoleWidth(unsigned int* pWidth)
{
	*pWidth = NULL;

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	HRESULT hr = GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo);
	RETURN_IF_FAILED(hr);
	*pWidth = consoleInfo.dwSize.X;
	return S_OK;
}

HRESULT ConsoleUIView::SetCursorPos(IUnknown*, COORD, bool)
{
	// TODO
	// needs ControlHandle
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::GetScreenBuffer(void** pScreenBuffer)
{
	*pScreenBuffer = m_navigationCallback.Get();
	return S_OK;
}

HRESULT ConsoleUIView::HandleKeyInput(PKEY_EVENT_RECORD)
{
	// TODO
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::InitializeFocus()
{
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::ShiftVisuals(unsigned int, int)
{
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::MoveFocusToNext()
{
	return E_NOTIMPL;
}

HRESULT ConsoleUIView::MoveFocusToPrevious()
{
	return E_NOTIMPL;
}



