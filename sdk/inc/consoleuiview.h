#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include "logoninterfaces.h"

namespace Windows::Internal::UI::Logon
{
	namespace ConsoleUIView
	{

		struct INavigationCallback : IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE OnNavigation() PURE;
			virtual HRESULT STDMETHODCALLTYPE ShowComboBox(CredProvData::IComboBoxField*) PURE;
		};

		struct IConsoleUIControl : IUnknown
		{
			virtual int STDMETHODCALLTYPE IsFocusable() PURE;
			virtual HRESULT STDMETHODCALLTYPE OnFocusChange(int) PURE;
			virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(const PKEY_EVENT_RECORD, int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE Unadvise() PURE;
		};

		struct IConsoleUIView : IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE Advise(INavigationCallback*) PURE;
			virtual HRESULT STDMETHODCALLTYPE Unadvise() PURE;
			virtual HRESULT STDMETHODCALLTYPE AppendControl(unsigned int, IConsoleUIControl*, IUnknown**) PURE;
			virtual HRESULT STDMETHODCALLTYPE WriteOutput(IUnknown*, const PCHAR_INFO, COORD, PSMALL_RECT) PURE;
			virtual HRESULT STDMETHODCALLTYPE GetColorAttributes(unsigned short*) PURE;
			virtual HRESULT STDMETHODCALLTYPE ResizeControl(IUnknown*, unsigned int) PURE;
			virtual HRESULT STDMETHODCALLTYPE RemoveAll() PURE;
			virtual HRESULT STDMETHODCALLTYPE GetConsoleWidth(unsigned int*) PURE;
			virtual HRESULT STDMETHODCALLTYPE SetCursorPos(IUnknown*, struct COORD, bool) PURE;
		};

		struct IConsoleUIViewInternal : IUnknown
		{
			virtual HRESULT STDMETHODCALLTYPE GetScreenBuffer(void**) PURE;
			virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(PKEY_EVENT_RECORD) PURE;
			virtual HRESULT STDMETHODCALLTYPE InitializeFocus() PURE;
		};
	}
}
