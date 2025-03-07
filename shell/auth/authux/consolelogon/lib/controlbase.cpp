#include "controlbase.h"

using namespace Microsoft::WRL;

BOOL ControlBase::IsFocusable()
{
	return m_isFocusable;
}

HRESULT ControlBase::OnFocusChange(BOOL hasFocus)
{
	return v_OnFocusChange(hasFocus);
}

HRESULT ControlBase::HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return v_HandleKeyInput(keyEvent, wasHandled);
}

HRESULT ControlBase::Unadvise()
{
	LOG_IF_FAILED(v_Unadvise());
	m_outputHandle.Reset();
	m_view.Reset();

	return S_OK;
}

HRESULT ControlBase::Initialize(BOOL isFocusable, UINT height, IConsoleUIView* view)
{
    m_isFocusable = isFocusable;
    RETURN_IF_FAILED(view->AppendControl(height, this, &m_outputHandle));

    m_view = view;

    return S_OK;
}

HRESULT ControlBase::PaintArea(const WCHAR* data, UINT length, ColorScheme colorScheme, UINT width, UINT height)
{
	UINT dataSize = 4 * height * width;
	wil::unique_cotaskmem_ptr<CHAR_INFO> _Ptr((CHAR_INFO*)CoTaskMemAlloc(dataSize));
    RETURN_IF_NULL_ALLOC(_Ptr);

    RETURN_IF_FAILED(ControlBase::ConvertStringToCharInfo(data, length, _Ptr.get(), colorScheme));

    RETURN_IF_FAILED(BackFillCharDataBuffer(L' ', colorScheme, length, width * height, _Ptr.get()));

    COORD coord;
    coord.X = width;
    coord.Y = height;

    SMALL_RECT writeRect;
	writeRect.Left = 0;
	writeRect.Top = 0;
    writeRect.Right = width - 1;
    writeRect.Bottom = height - 1;

    RETURN_IF_FAILED(m_view->WriteOutput(m_outputHandle.Get(), _Ptr.get(), coord, &writeRect));

    return S_OK;
}

HRESULT ControlBase::ConvertStringToCharInfo(const WCHAR* content, UINT length, CHAR_INFO* charData, ColorScheme colorScheme)
{
    WORD colorAttributes;
    RETURN_IF_FAILED(m_view->GetColorAttributes(&colorAttributes));

    for (UINT i = 0; i < length; i++)
    {
        charData[i].Char.UnicodeChar = content[i] == L'\r' || content[i] == L'\n' ? L' ' : content[i];

        WORD invertedAttributes;
        if (colorScheme == Inverted)
        {
            invertedAttributes = InvertColorAttributes(colorAttributes);
        }
        else
        {
            invertedAttributes = colorAttributes;
        }
        charData[i].Attributes = invertedAttributes;
    }

    return S_OK;
}

HRESULT ControlBase::BackFillCharDataBuffer(WCHAR fillCharacter, ColorScheme colorScheme, UINT offset, UINT length, CHAR_INFO* charData)
{
    WORD colorAttributes;
    RETURN_IF_FAILED(m_view->GetColorAttributes(&colorAttributes));

    for (UINT i = offset; i < length; i++)
    {
        charData[i].Char.UnicodeChar = fillCharacter;
        WORD invertedAttributes;
        if (colorScheme == Inverted)
        {
	        invertedAttributes = InvertColorAttributes(colorAttributes);
        }
        else
        {
	        invertedAttributes = colorAttributes;
        }
        charData[i].Attributes = invertedAttributes;
    }

    return S_OK;
}

WORD ControlBase::InvertColorAttributes(WORD colorAttributes)
{
    return colorAttributes & 0xFF00 | (16 * (colorAttributes & 0xF)) | ((BYTE)colorAttributes >> 4);
}

HRESULT ControlBase::v_Unadvise()
{
	return S_OK;
}
