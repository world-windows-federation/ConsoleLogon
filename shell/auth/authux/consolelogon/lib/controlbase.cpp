#include "controlbase.h"

using namespace Microsoft::WRL;

BOOL ControlBase::IsFocusable()
{
	return m_isFocusable;
}

STDMETHODIMP ControlBase::OnFocusChange(BOOL hasFocus)
{
	return v_OnFocusChange(hasFocus);
}

STDMETHODIMP ControlBase::HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;
	return v_HandleKeyInput(keyEvent, wasHandled);
}

STDMETHODIMP ControlBase::Unadvise()
{
	LOG_IF_FAILED(v_Unadvise());
	m_outputHandle.Reset();
	m_view.Reset();

	return S_OK;
}

HRESULT ControlBase::Initialize(BOOL isFocusable, unsigned int height, IConsoleUIView* view)
{
    m_isFocusable = isFocusable;
    m_outputHandle.Reset();
    RETURN_IF_FAILED(view->AppendControl(
        height,
        this,
        m_outputHandle.GetAddressOf()));

    m_view = view;

    return S_OK;
}

HRESULT ControlBase::PaintArea(const WCHAR* data, unsigned int length, ColorScheme colorScheme, unsigned int width, unsigned int height)
{
    CHAR_INFO* _Ptr = (CHAR_INFO *)CoTaskMemAlloc(4 * height * width);

    // @MOD Scope exit which did not exist in the original (probably an inlined smart pointer):
    auto scopeExit = wil::scope_exit([_Ptr]() -> void {
        if (_Ptr)
            CoTaskMemFree(_Ptr);
    });

    RETURN_IF_NULL_ALLOC(_Ptr);

    RETURN_IF_FAILED(ControlBase::ConvertStringToCharInfo(data, length, _Ptr, colorScheme));

    RETURN_IF_FAILED(BackFillCharDataBuffer(
        0,
        colorScheme,
        length,
        width * height,
        _Ptr));

    SMALL_RECT writeRect = { 0 };
    COORD coord;
    coord.X = width;
    coord.Y = height;
    writeRect.Right = width - 1;
    writeRect.Bottom = height - 1;

    RETURN_IF_FAILED(m_view->WriteOutput(
        m_outputHandle.Get(),
        _Ptr,
        coord,
        &writeRect));

    return S_OK;
}

HRESULT ControlBase::ConvertStringToCharInfo(const WCHAR* content, unsigned int length, CHAR_INFO* charData, ColorScheme colorScheme)
{
    WORD colorAttributes;
    RETURN_IF_FAILED(m_view->GetColorAttributes(&colorAttributes));

    for (int i = 0; i < length; i++)
    {
        charData[i].Char.UnicodeChar = content[i] == L'\r' || content[i] == L'\n'
            ? L' '
            : content[i];

        WORD invertedColorAttributes;
        if (colorScheme == Inverted)
        {
            invertedColorAttributes = InvertColorAttributes(colorAttributes);
        }
        else
        {
            invertedColorAttributes = colorAttributes;
        }
        charData[i].Attributes = invertedColorAttributes;
    }

    return S_OK;
}

HRESULT ControlBase::BackFillCharDataBuffer(wchar_t _char /* optimised out */, ColorScheme colorScheme, unsigned int offset, unsigned int length, CHAR_INFO* charData)
{
    WORD colorAttributes;
    RETURN_IF_FAILED(m_view->GetColorAttributes(&colorAttributes));

    for (int i = offset; i < length; i++)
    {
        charData[i].Char.UnicodeChar = L' ';
        WORD invertedAttributes = colorAttributes;
        if (colorScheme == Inverted)
            invertedAttributes = InvertColorAttributes(colorAttributes);
        charData[i].Attributes = invertedAttributes;
    }

    return S_OK;
}

WORD ControlBase::InvertColorAttributes(WORD colorAttributes)
{
    return colorAttributes & 0xFF00 | (16 * (colorAttributes & 0xF)) | ((BYTE)colorAttributes >> 4);
}

STDMETHODIMP ControlBase::v_Unadvise()
{
	return S_OK;
}