#include "basictextcontrol.h"
#include "pch.h"
#include <wil\resource.h>

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

HRESULT BasicTextControl::RuntimeClassInitialize(IConsoleUIView* view, PWCHAR dataSource, bool acceptFocus)
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

HRESULT BasicTextControl::Repaint(IConsoleUIView* view)
{
	UINT length = m_dataSource.GetCount();
	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 47

	UINT height = length / consoleWidth + 1;
	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(this->Initialize(m_acceptFocus, height, view)); // 53
		m_isInitialized = true;
	}
	if (height != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), height)); // 59
	}
	m_VisibleControlSize = height;

	RETURN_IF_FAILED(this->PaintArea(m_dataSource.Get(), length, (ColorScheme)m_hasFocus, consoleWidth, m_VisibleControlSize)); // 63
	return S_OK;
}
