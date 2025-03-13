#include "pch.h"

#include "consoleuiview.h"

#include "controlhandle.h"

using namespace Microsoft::WRL;

ConsoleUIView::ConsoleUIView()
	: m_focusIndex(-1)
{
}

ConsoleUIView::~ConsoleUIView()
{
}

HRESULT ConsoleUIView::Initialize()
{
	m_screenBuffer = wil::unique_handle(
		CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr));
	RETURN_LAST_ERROR_IF_NULL(m_screenBuffer); // 24
	return S_OK;
}

HRESULT ConsoleUIView::Advise(INavigationCallback* callback)
{
	m_navigationCallback = callback;
	return S_OK;
}

HRESULT ConsoleUIView::Unadvise()
{
	v_Unadvise();
	m_navigationCallback.Reset();
	return S_OK;
}

HRESULT ConsoleUIView::AppendControl(UINT height, IConsoleUIControl* control, IUnknown** ppControlHandle)
{
	*ppControlHandle = nullptr;

	ComPtr<ControlHandle> controlHandle;

	if (!m_controlTable._celem)
	{
		RETURN_IF_FAILED(MakeAndInitialize<ControlHandle>(&controlHandle, 0, height, 0, control)); // 51
	}
	else
	{
		ComPtr<IControlHandle> lastControl = m_controlTable[m_controlTable._celem - 1];
		UINT startingOffset = lastControl->GetOffsetFromRoot() + lastControl->GetSize();

		RETURN_IF_FAILED(MakeAndInitialize<ControlHandle>(&controlHandle, startingOffset, height, (UINT)m_controlTable._celem, control)); // 58
	}

	RETURN_IF_FAILED(m_controlTable.Add(controlHandle)); // 61

	RETURN_IF_FAILED(controlHandle.CopyTo(ppControlHandle)); // 63
	return S_OK;
}

HRESULT ConsoleUIView::WriteOutput(IUnknown* handle, PCHAR_INFO data, COORD dataSize, PSMALL_RECT writeRegion)
{
	ComPtr<IControlHandle> controlHandle;
	RETURN_IF_FAILED(handle->QueryInterface(IID_PPV_ARGS(&controlHandle))); // 71

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &screenBufferInfo)); // 74


	RETURN_HR_IF(E_INVALIDARG, ((writeRegion->Top < 0) || (writeRegion->Bottom >= (SHORT)controlHandle->GetSize()))); // 77
	RETURN_HR_IF(E_INVALIDARG, ((writeRegion->Left < 0 ) || (writeRegion->Right >= (SHORT)screenBufferInfo.dwSize.X))); // 78
	RETURN_HR_IF(E_INVALIDARG, ((writeRegion->Top > writeRegion->Bottom)|| (writeRegion->Left > writeRegion->Right))); // 79

	SMALL_RECT actualWriteRect;
	actualWriteRect.Left = writeRegion->Left;
	actualWriteRect.Top = writeRegion->Top + controlHandle->GetOffsetFromRoot();
	actualWriteRect.Right = writeRegion->Right;
	actualWriteRect.Bottom = writeRegion->Bottom + controlHandle->GetOffsetFromRoot();

	RETURN_IF_WIN32_BOOL_FALSE(WriteConsoleOutputW(m_screenBuffer.get(), data, dataSize, (COORD)0, &actualWriteRect)); // 96

	writeRegion->Left = actualWriteRect.Left;
	writeRegion->Top = actualWriteRect.Top - controlHandle->GetOffsetFromRoot();
	writeRegion->Right = actualWriteRect.Right;
	writeRegion->Bottom = actualWriteRect.Bottom - controlHandle->GetOffsetFromRoot();

	return S_OK;
}

HRESULT ConsoleUIView::GetColorAttributes(WORD* pAttributes)
{
	*pAttributes = 0;

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo)); // 111
	*pAttributes = consoleInfo.wAttributes;
	return S_OK;
}

HRESULT ConsoleUIView::ResizeControl(IUnknown* handle, UINT newHeight)
{
	ComPtr<IControlHandle> controlHandle;
	RETURN_IF_FAILED(handle->QueryInterface(IID_PPV_ARGS(&controlHandle))); // 120

	UINT oldHeight = controlHandle->GetSize();
	UINT shiftDistance = newHeight - oldHeight;
	RETURN_IF_FAILED(ShiftVisuals(controlHandle->GetIndexInTable() + 1, shiftDistance)); // 126

	RETURN_IF_FAILED(controlHandle->SetSize(newHeight)); // 127

	return S_OK;
}

HRESULT ConsoleUIView::RemoveAll()
{
	for (size_t i = 0; i < m_controlTable.GetCapacity(); ++i)
	{
		RETURN_IF_FAILED(m_controlTable[i]->Destroy()); // 136
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo)); // 143

	int consoleSize = consoleInfo.dwSize.X * consoleInfo.dwSize.Y;
	COORD origin = {};
	DWORD numWritten;
	RETURN_IF_WIN32_BOOL_FALSE(FillConsoleOutputCharacterW(
		m_screenBuffer.get(),
		L' ',
		consoleSize,
		origin,
		&numWritten
	)); // 154

	return S_OK;
}

HRESULT ConsoleUIView::GetConsoleWidth(UINT* pWidth)
{
	*pWidth = 0;

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &consoleInfo)); // 163
	*pWidth = consoleInfo.dwSize.X;
	return S_OK;
}

HRESULT ConsoleUIView::SetCursorPos(IUnknown* handle, COORD position, bool isVisible)
{
	ComPtr<IControlHandle> controlHandle;
	RETURN_IF_FAILED(handle->QueryInterface(IID_PPV_ARGS(&controlHandle))); // 171

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &screenBufferInfo)); // 174

	RETURN_HR_IF(E_INVALIDARG, (position.Y < 0) || (position.Y >= (SHORT)controlHandle->GetSize())); // 177
	RETURN_HR_IF(E_INVALIDARG, (position.X < 0) || (position.X >= screenBufferInfo.dwSize.X)); // 178
	RETURN_HR_IF(E_ACCESSDENIED, m_focusIndex != controlHandle->GetIndexInTable()); // 179

	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 4;
	cursorInfo.bVisible = isVisible;
	RETURN_IF_WIN32_BOOL_FALSE(SetConsoleCursorInfo(m_screenBuffer.get(), &cursorInfo)); // 184

	COORD actualPosition = position;
	actualPosition.Y += controlHandle->GetOffsetFromRoot();
	RETURN_IF_WIN32_BOOL_FALSE(SetConsoleCursorPosition(m_screenBuffer.get(), actualPosition)); // 188

	return S_OK;
}

HRESULT ConsoleUIView::GetScreenBuffer(void** pScreenBuffer)
{
	*pScreenBuffer = m_screenBuffer.get();
	return S_OK;
}

HRESULT ConsoleUIView::HandleKeyInput(const KEY_EVENT_RECORD* keyEvent)
{
	BOOL keyHandled = FALSE;

	if (m_focusIndex >= 0)
	{
		RETURN_IF_FAILED(m_controlTable[m_focusIndex]->HandleKeyInput(keyEvent, &keyHandled)); // 205
	}

	if (!keyHandled)
	{
		RETURN_IF_FAILED(v_OnKeyInput(keyEvent, &keyHandled)); // 210
	}

	if (!keyHandled)
	{
		switch (keyEvent->wVirtualKeyCode)
		{
			case VK_TAB:
				if ((keyEvent->dwControlKeyState & 0x10) != 0)
				{
					RETURN_IF_FAILED(MoveFocusToPrevious()); // 220
				}
				else
				{
					RETURN_IF_FAILED(MoveFocusToNext()); // 224
				}
				break;

			case VK_UP:
				RETURN_IF_FAILED(MoveFocusToPrevious()); // 228
				break;

			case VK_DOWN:
				RETURN_IF_FAILED(MoveFocusToNext()); // 231
				break;
		}
	}

	return S_OK;
}

HRESULT ConsoleUIView::InitializeFocus()
{
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 4;
	cursorInfo.bVisible = FALSE;

	RETURN_IF_WIN32_BOOL_FALSE(SetConsoleCursorInfo(m_screenBuffer.get(), &cursorInfo)); // 246

	size_t controlTableSize = m_controlTable.GetSize();
	for (size_t i = 0; i < controlTableSize && m_focusIndex < 0; ++i)
	{
		if (m_controlTable[i]->IsFocusable() && m_controlTable[i]->GetSize())
		{
			m_focusIndex = (int)i;
			RETURN_IF_FAILED(m_controlTable[i]->SetFocus(TRUE)); // 255
		}
	}

	return S_OK;
}

void ConsoleUIView::v_Unadvise()
{
}

int ConsoleUIView::GetFocusIndex() const
{
	return m_focusIndex;
}

HRESULT ConsoleUIView::ShiftVisuals(UINT startIndex, int shiftDistance)
{
	if (!shiftDistance)
		return S_OK;

	size_t controlTableSize = m_controlTable.GetSize();
	if (startIndex >= controlTableSize)
		return S_OK;

	UINT topOfRegionToShift = m_controlTable[startIndex]->GetOffsetFromRoot();

	ComPtr<IControlHandle> lastControl = m_controlTable[controlTableSize - 1];
	UINT bottomOfRegionToShift = lastControl->GetOffsetFromRoot() + lastControl->GetSize();

	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
	RETURN_IF_WIN32_BOOL_FALSE(GetConsoleScreenBufferInfo(m_screenBuffer.get(), &screenBufferInfo)); // 281

	SMALL_RECT scrollRect;
	scrollRect.Left = 0;
	scrollRect.Top = topOfRegionToShift;
	scrollRect.Right = screenBufferInfo.dwSize.X - 1;
	scrollRect.Bottom = bottomOfRegionToShift;

	SMALL_RECT clipRect = scrollRect;
	if (shiftDistance <= 0)
	{
		clipRect.Top = scrollRect.Top + shiftDistance;
	}
	else
	{
		clipRect.Bottom = scrollRect.Bottom + shiftDistance;
	}

	CHAR_INFO fillCharacter;
	fillCharacter.Attributes = 0;
	fillCharacter.Char.UnicodeChar = L' ';

	COORD destinationCoordinate;
	destinationCoordinate.X = 0;
	destinationCoordinate.Y = topOfRegionToShift + shiftDistance;

	RETURN_IF_WIN32_BOOL_FALSE(ScrollConsoleScreenBufferW(m_screenBuffer.get(), &scrollRect, &clipRect, destinationCoordinate, &fillCharacter)); // 321

	for (UINT i = startIndex; i < controlTableSize; ++i)
	{
		UINT offset = m_controlTable[i]->GetOffsetFromRoot();
		RETURN_IF_FAILED(m_controlTable[i]->UpdateOffsetFromRoot(shiftDistance + offset)); // 327
	}

	return S_OK;
}

HRESULT ConsoleUIView::MoveFocusToNext()
{
	if (m_focusIndex >= 0)
	{
		CONSOLE_CURSOR_INFO cursorInfo;
		cursorInfo.dwSize = 4;
		cursorInfo.bVisible = FALSE;
		RETURN_IF_WIN32_BOOL_FALSE(SetConsoleCursorInfo(m_screenBuffer.get(), &cursorInfo)); // 341
		RETURN_IF_FAILED(m_controlTable[m_focusIndex]->SetFocus(FALSE)); // 342
	}

	int oldFocusIndex = m_focusIndex;

	int i = oldFocusIndex;
	int controlTableSize = (int)m_controlTable._celem;
	while ((i = (i + 1) % controlTableSize) != oldFocusIndex)
	{
		if (m_controlTable[i]->IsFocusable())
		{
			m_focusIndex = i;
			RETURN_IF_FAILED(m_controlTable[i]->SetFocus(TRUE)); // 352
			break;
		}
	}

	if (m_focusIndex == oldFocusIndex)
	{
		if (m_controlTable[oldFocusIndex]->IsFocusable())
		{
			RETURN_IF_FAILED(m_controlTable[oldFocusIndex]->SetFocus(TRUE)); // 361
		}
		else
		{
			m_focusIndex = -1;
		}
	}

	return S_OK;
}

HRESULT ConsoleUIView::MoveFocusToPrevious()
{
	if (m_focusIndex >= 0)
	{
		CONSOLE_CURSOR_INFO cursorInfo;
		cursorInfo.dwSize = 4;
		cursorInfo.bVisible = FALSE;
		RETURN_IF_WIN32_BOOL_FALSE(SetConsoleCursorInfo(m_screenBuffer.get(), &cursorInfo)); // 380
		RETURN_IF_FAILED(m_controlTable[m_focusIndex]->SetFocus(FALSE)); // 381
	}

	int oldFocusIndex = m_focusIndex;

	int i = oldFocusIndex;
	int controlTableSize = (int)m_controlTable._celem;
	while ((i = (i > 0 ? i : controlTableSize) - 1) != oldFocusIndex)
	{
		if (m_controlTable[i]->IsFocusable())
		{
			m_focusIndex = i;
			RETURN_IF_FAILED(m_controlTable[i]->SetFocus(TRUE)); // 391
			break;
		}
	}

	if (m_focusIndex == oldFocusIndex)
	{
		if (m_controlTable[oldFocusIndex]->IsFocusable())
		{
			RETURN_IF_FAILED(m_controlTable[oldFocusIndex]->SetFocus(TRUE)); // 400
		}
		else
		{
			m_focusIndex = -1;
		}
	}

	return S_OK;
}
