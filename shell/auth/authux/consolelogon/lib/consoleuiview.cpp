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
	~ConsoleUIView();
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
	virtual HRESULT v_OnKeyInput(PKEY_EVENT_RECORD, int*) PURE;
	virtual void v_Unadvise() PURE;
	virtual int GetFocusIndex() PURE;

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

ConsoleUIView::~ConsoleUIView()
{

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
	this->v_Unadvise();
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

HRESULT ConsoleUIView::HandleKeyInput(PKEY_EVENT_RECORD keyEvent)
{
	int keyHandled = 0;
	if (m_focusIndex < 0)
	{
		HRESULT hr = this->v_OnKeyInput(keyEvent, &keyHandled);
		RETURN_IF_FAILED(hr);
	}
	else
	{
		IControlHandle* ptr = m_controlTable._parray[m_focusIndex].Get();
		HRESULT hr = ptr->HandleKeyInput(keyEvent, &keyHandled);
		RETURN_IF_FAILED(hr);
	}
	if (!keyHandled)
	{
		switch (keyEvent->wVirtualKeyCode)
		{
		case VK_TAB:
		{
			if ((keyEvent->dwControlKeyState & 0x10) != 0)
			{
				RETURN_IF_FAILED(this->MoveFocusToPrevious());
			}
			else
			{
				RETURN_IF_FAILED(this->MoveFocusToNext());
			}
		}
		break;
		case VK_UP:
		{
			RETURN_IF_FAILED(this->MoveFocusToPrevious());
		}
		break;
		case VK_DOWN:
		{
			RETURN_IF_FAILED(this->MoveFocusToNext());
		}
		break;
		}
	}
	return S_OK;
}

HRESULT ConsoleUIView::InitializeFocus()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 4;

	RETURN_IF_FAILED(SetConsoleCursorInfo(m_screenBuffer.get(), &cursorInfo));

	int counter = 0;
	UINT celem = m_controlTable._celem;
	if (celem)
	{
		while (this->m_focusIndex < 0)
		{
			IControlHandle* ptr = m_controlTable._parray[counter].Get();
			if (ptr->IsFocusable() && ptr->GetSize())
			{
				m_focusIndex = counter;
				RETURN_IF_FAILED(ptr->SetFocus(TRUE));
			}
			if (++counter >= celem)
			{
				return 0;
			}
		}
	}
	return S_OK;
}

HRESULT ConsoleUIView::ShiftVisuals(unsigned int startIndex, int shiftDistance)
{
	if (!shiftDistance) return S_OK;

	UINT celem = this->m_controlTable._celem;
	IControlHandle* ptr = m_controlTable._parray[startIndex].Get();
	UINT counter = startIndex;
	UINT destinationCoord = ptr->GetOffsetFromRoot();

	IControlHandle* lastControl = m_controlTable._parray[celem - 1].Get();
	if (lastControl)
	{
		lastControl->AddRef();
	}
	UINT height = lastControl->GetSize() + lastControl->GetOffsetFromRoot();

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &screenBufferInfo));
	SMALL_RECT scrollRect;
	scrollRect.Left = 0;
	scrollRect.Bottom = height;
	scrollRect.Right = screenBufferInfo.dwSize.X - 1;
	scrollRect.Top = destinationCoord;

	SMALL_RECT clipRect = scrollRect;
	if (shiftDistance <= 0)
	{
		clipRect.Top = shiftDistance + scrollRect.Top;
	}
	else
	{
		clipRect.Bottom = shiftDistance + scrollRect.Bottom;
	}

	CHAR_INFO fillCharacter;
	fillCharacter.Attributes = 0;
	fillCharacter.Char.UnicodeChar = 32;

	COORD destinationCoordinatea;
	destinationCoordinatea.X = 0;
	destinationCoordinatea.Y = shiftDistance + destinationCoord;

	RETURN_IF_WIN32_BOOL_FALSE(ScrollConsoleScreenBufferW(
		m_screenBuffer.get(),
		&scrollRect,
		&clipRect,
		destinationCoordinatea,
		&fillCharacter));

	while (TRUE)
	{
		if (counter >= celem)
		{
			return S_OK;
		}
		UINT offset = m_controlTable._parray[counter].Get()->GetOffsetFromRoot();
		HRESULT hr = m_controlTable._parray[counter].Get()->UpdateOffsetFromRoot(shiftDistance + offset);
		RETURN_IF_FAILED(hr);
		++counter;
	}
	// not sure
	return HRESULT_FROM_WIN32(GetLastError());

}

HRESULT ConsoleUIView::MoveFocusToNext()
{

}

HRESULT ConsoleUIView::MoveFocusToPrevious()
{
	return E_NOTIMPL;
}



