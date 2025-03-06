#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include "consoleuiview.h"
#include "logoninterfaces.h"

struct IQueryFocus : public IUnknown
{
	BOOL HasFocus();
};

class ControlBase
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IConsoleUIControl>
{
public:
	HRESULT Initialize(BOOL isFocusable, unsigned int height, IConsoleUIView* view);
	STDMETHODIMP_(BOOL) IsFocusable() override;
	STDMETHODIMP OnFocusChange(BOOL hasFocus) override;
	STDMETHODIMP HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	STDMETHODIMP Unadvise() override;

	enum ColorScheme
	{
		Normal = 0,
		Inverted = 1
	};

protected:
	virtual HRESULT v_OnFocusChange(BOOL hasFocus) PURE;
	virtual HRESULT v_HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) PURE;
	virtual HRESULT v_Unadvise();
	
	HRESULT PaintArea(const WCHAR* data, unsigned int length, ColorScheme colorScheme, unsigned int width, unsigned int height);
	HRESULT ConvertStringToCharInfo(const WCHAR* content, unsigned int length, CHAR_INFO* charData, ColorScheme colorScheme);
	HRESULT BackFillCharDataBuffer(wchar_t _char /* optimised out */, ColorScheme colorScheme, unsigned int offset, unsigned int length, CHAR_INFO* charData);

	Microsoft::WRL::ComPtr<IUnknown> m_outputHandle;
	Microsoft::WRL::ComPtr<IConsoleUIView> m_view;

private:
	WORD InvertColorAttributes(WORD colorAttributes);
	BOOL m_isFocusable;
};