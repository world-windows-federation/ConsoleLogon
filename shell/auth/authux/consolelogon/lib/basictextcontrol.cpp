#include "pch.h"

#include "basictextcontrol.h"

#include <wil/resource.h>

BasicTextControl::BasicTextControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_acceptFocus(false)
	, m_hasFocus(false)
{
}

BasicTextControl::~BasicTextControl()
{
}

HRESULT BasicTextControl::RuntimeClassInitialize(IConsoleUIView* view, const WCHAR* dataSource, bool acceptFocus)
{
	m_acceptFocus = acceptFocus;
	RETURN_IF_FAILED(m_dataSource.Initialize(dataSource)); // 21

	RETURN_IF_FAILED(Repaint(view)); // 23
	return S_OK;
}

HRESULT BasicTextControl::RuntimeClassInitialize(IConsoleUIView* view, UINT stringResourceId)
{
	RETURN_IF_FAILED(m_dataSource.Initialize(HINST_THISCOMPONENT, stringResourceId)); // 30

	RETURN_IF_FAILED(Repaint(view)); // 32
	return S_OK;
}

BOOL BasicTextControl::HasFocus()
{
	return m_VisibleControlSize;
}

HRESULT BasicTextControl::v_OnFocusChange(BOOL hasFocus)
{
	if (m_acceptFocus)
	{
		m_hasFocus = hasFocus != 0;
		RETURN_IF_FAILED(Repaint(m_view.Get())); // 73
	}
	else
	{
		RETURN_HR(E_NOTIMPL); // 78
	}

	return S_OK;
}

HRESULT BasicTextControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return S_OK;
}

HRESULT BasicTextControl::Repaint(IConsoleUIView* view)
{
	UINT contentLength = (UINT)m_dataSource.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 47

	UINT controlSize = contentLength / consoleWidth + 1;
	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(m_acceptFocus, controlSize, view)); // 53
		m_isInitialized = true;
	}

	if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 59
	}
	m_VisibleControlSize = controlSize;

	RETURN_IF_FAILED(PaintArea(m_dataSource.Get(), contentLength, FocusToColorScheme(m_hasFocus), consoleWidth, m_VisibleControlSize)); // 63
	return S_OK;
}
