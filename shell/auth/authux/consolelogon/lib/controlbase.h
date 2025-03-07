#pragma once

#include "pch.h"

#include "consoleuiview.h"

MIDL_INTERFACE("12af1a6b-0e79-4ad6-b317-436d849b1c11")
IQueryFocus : IUnknown
{
	virtual BOOL STDMETHODCALLTYPE HasFocus() PURE;
};

class ControlBase
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IConsoleUIControl>
{
public:
	HRESULT Initialize(BOOL isFocusable, UINT height, IConsoleUIView* view);

	//~ Begin IConsoleUIControl Interface
	STDMETHODIMP_(BOOL) IsFocusable() override;
	STDMETHODIMP OnFocusChange(BOOL hasFocus) override;
	STDMETHODIMP HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	STDMETHODIMP Unadvise() override;
	//~ End IConsoleUIControl Interface

	enum ColorScheme
	{
		Normal = 0,
		Inverted = 1
	};

protected:
	virtual HRESULT v_OnFocusChange(BOOL hasFocus) PURE;
	virtual HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) PURE;
	virtual HRESULT v_Unadvise();

	HRESULT PaintArea(const WCHAR* data, UINT length, ColorScheme colorScheme, UINT width, UINT height);
	HRESULT ConvertStringToCharInfo(const WCHAR* content, UINT length, CHAR_INFO* charData, ColorScheme colorScheme);
	HRESULT BackFillCharDataBuffer(WCHAR fillCharacter /* optimised out */, ColorScheme colorScheme, UINT offset, UINT length, CHAR_INFO* charData);

	Microsoft::WRL::ComPtr<IUnknown> m_outputHandle;
	Microsoft::WRL::ComPtr<IConsoleUIView> m_view;

private:
	WORD InvertColorAttributes(WORD colorAttributes);

	BOOL m_isFocusable;
};
