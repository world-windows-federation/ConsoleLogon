#include "pch.h"

#include "serializationfailedoptioncontrol.h"

SerializationFailedOptionControl::SerializationFailedOptionControl()
	: m_VisibleControlSize(0)
	, m_isInitialized(false)
	, m_hasFocus(false)
{
}

HRESULT SerializationFailedOptionControl::RuntimeClassInitialize(IConsoleUIView* view)
{
	RETURN_IF_FAILED(m_label.Initialize(HINST_THISCOMPONENT, IDS_OK)); // 23

	RETURN_IF_FAILED(Repaint(view));
	return S_OK;
}

HRESULT SerializationFailedOptionControl::v_OnFocusChange(BOOL hasFocus)
{
	m_hasFocus = hasFocus != 0;
	RETURN_IF_FAILED(Repaint(m_view.Get())); // 59
	return S_OK;
}

HRESULT SerializationFailedOptionControl::v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return S_OK;
}

HRESULT SerializationFailedOptionControl::Repaint(IConsoleUIView* view)
{
	UINT contentLength = (UINT)m_label.GetCount();

	UINT consoleWidth;
	RETURN_IF_FAILED(view->GetConsoleWidth(&consoleWidth)); // 35

	UINT controlSize = contentLength / consoleWidth + 1;

	if (!m_isInitialized)
	{
		RETURN_IF_FAILED(Initialize(TRUE, controlSize, view)); // 41
		m_isInitialized = true;
		m_VisibleControlSize = controlSize;
	}
	else if (controlSize != m_VisibleControlSize)
	{
		RETURN_IF_FAILED(view->ResizeControl(m_outputHandle.Get(), controlSize)); // 47
		m_VisibleControlSize = controlSize;
	}

	RETURN_IF_FAILED(PaintArea(m_label.Get(), contentLength, FocusToColorScheme(m_hasFocus), consoleWidth, m_VisibleControlSize)); // 51
	return S_OK;
}
