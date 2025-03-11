#pragma once

#include "pch.h"

#include "controlhandle.h"

MIDL_INTERFACE("04ae9c0f-e0cc-43c8-9d9f-a9ad229c114a")
INavigationCallback : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE OnNavigation() PURE;
	virtual HRESULT STDMETHODCALLTYPE ShowComboBox(Windows::Internal::UI::Logon::CredProvData::IComboBoxField*) PURE;
};

// Belongs to internal inputswitchserver.h
inline const GUID DECLSPEC_SELECTANY CLSID_InputSwitchControl = { 0xb9bc2a50, 0x43c3, 0x41aa, { 0xa0, 0x86, 0x5d, 0xb1, 0x4e, 0x18, 0x4b, 0xae } };

typedef enum __MIDL___MIDL_itf_inputswitchserver_0000_0000_0001
{
	ISCT_IDL_DESKTOP,
	ISCT_IDL_TOUCHKEYBOARD,
	ISCT_IDL_LOGONUI,
	ISCT_IDL_UAC,
	ISCT_IDL_SETTINGSPANE,
	ISCT_IDL_OOBE,
	ISCT_IDL_USEROOBE
} INPUT_SWITCH_IDL_CLIENT_TYPE;

typedef struct __MIDL___MIDL_itf_inputswitchserver_0000_0000_0002
{
	DWORD hkl;
	WCHAR* pszShortLanguageName;
	WCHAR* pszLanguageName;
	WCHAR* pszShortLayoutName;
	WCHAR* pszLayoutName;
	BOOL fShortLayoutNameIsDifferentiator;
	BOOL fIme;
	BOOL fDesktopIncompatibleIme;
	BOOL fImmersiveIncompatibleIme;
// #if TASKBAR_FOR >= TASKBAR_FOR_22621
	// BYTE gap[8];
// #endif
	WCHAR* pszBcp47;
	WCHAR* pszFontFaceName;
	int nFontSizeAdjustment;
	int nFontBaselineAdjustment;
	BOOL fFontVertical;
	WCHAR* pszImeIconFileName;
	int nImeIconIndex;
} INPUT_SWITCH_IDL_PROFILE_DATA;

typedef struct __MIDL___MIDL_itf_inputswitchserver_0000_0000_0003
{
	WCHAR* pszTooltip;
	HICON hIcon;
	BOOL fDisabled;
	BOOL fHidden;
	WCHAR* pszIconGlyph;
} INPUT_SWITCH_IDL_IME_MODE_ITEM_DATA;

typedef enum __MIDL___MIDL_itf_inputswitchserver_0000_0000_0004
{
	INPUT_SWITCH_IDL_IME_CLICK_TYPE_LEFT,
	INPUT_SWITCH_IDL_IME_CLICK_TYPE_RIGHT,
	INPUT_SWITCH_IDL_IME_CLICK_TYPE_LEFT_DISABLED
} INPUT_SWITCH_IDL_IME_CLICK_TYPE;

typedef enum __MIDL___MIDL_itf_inputswitchserver_0000_0000_0005
{
	INPUT_SWITCH_IDL_MODALITY_STANDARDKEYBOARD = 0x1,
	INPUT_SWITCH_IDL_MODALITY_SPLITKEYBOARD = 0x2,
	INPUT_SWITCH_IDL_MODALITY_CLASSICKEYBOARD = 0x4,
	INPUT_SWITCH_IDL_MODALITY_HANDWRITING = 0x8,
	INPUT_SWITCH_IDL_MODALITY_HIDE = 0x10,
	INPUT_SWITCH_IDL_MODALITY_ONEHANDEDKEYBOARD = 0x20,
} INPUT_SWITCH_IDL_MODALITY;

DEFINE_ENUM_FLAG_OPERATORS(INPUT_SWITCH_IDL_MODALITY);

typedef enum __MIDL___MIDL_itf_inputswitchserver_0000_0000_0006
{
	INPUT_SWITCH_IDL_ALIGN_DEFAULT,
	INPUT_SWITCH_IDL_ALIGN_RIGHT_EDGE,
	INPUT_SWITCH_IDL_ALIGN_LEFT_EDGE,
} INPUT_SWITCH_IDL_ALIGNMENT;

typedef enum __MIDL___MIDL_itf_inputswitchserver_0000_0000_0008
{
	INPUT_SWITCH_IDL_CFOM_NO_OVERRIDE,
	INPUT_SWITCH_IDL_CFOM_DESKTOP,
	INPUT_SWITCH_IDL_CFOM_IMMERSIVE,
} INPUT_SWITCH_IDL_CFOM;

interface IInputSwitchCallback;

MIDL_INTERFACE("b9bc2a50-43c3-41aa-a082-5db14e184bae")
IInputSwitchControl : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Init(INPUT_SWITCH_IDL_CLIENT_TYPE) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetCallback(IInputSwitchCallback*) = 0;
	virtual HRESULT STDMETHODCALLTYPE ShowInputSwitch(const RECT*) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProfileCount(UINT*, BOOL*) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentProfile(INPUT_SWITCH_IDL_PROFILE_DATA*) = 0;
	virtual HRESULT STDMETHODCALLTYPE RegisterHotkeys() = 0;
	virtual HRESULT STDMETHODCALLTYPE ClickImeModeItem(INPUT_SWITCH_IDL_IME_CLICK_TYPE, POINT, const RECT*) = 0;
// #if TASKBAR_FOR >= TASKBAR_FOR_22621
	// virtual HRESULT STDMETHODCALLTYPE ClickImeModeItemWithAnchor(INPUT_SWITCH_IDL_IME_CLICK_TYPE, IUnknown*) = 0;
// #endif
	virtual HRESULT STDMETHODCALLTYPE ForceHide() = 0;
	virtual HRESULT STDMETHODCALLTYPE ShowTouchKeyboardInputSwitch(const RECT*, INPUT_SWITCH_IDL_ALIGNMENT, int, DWORD, INPUT_SWITCH_IDL_MODALITY) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetContextFlags(DWORD*) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetContextOverrideMode(INPUT_SWITCH_IDL_CFOM) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentImeModeItem(INPUT_SWITCH_IDL_IME_MODE_ITEM_DATA*) = 0;
	virtual HRESULT STDMETHODCALLTYPE ActivateInputProfile(const WCHAR*) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetUserSid(const WCHAR*) = 0;
};

MIDL_INTERFACE("b9bc2a50-43c3-41aa-a083-5db14e184bae")
IInputSwitchCallback : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE OnUpdateProfile(const INPUT_SWITCH_IDL_PROFILE_DATA* pData) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnUpdateTsfFloatingFlags(DWORD dwFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnProfileCountChange(UINT nCount, BOOL bCertifiedImePresent) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnShowHide(BOOL bShown, BOOL a3, BOOL a4) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnImeModeItemUpdate(const INPUT_SWITCH_IDL_IME_MODE_ITEM_DATA* pData) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnModalitySelected(INPUT_SWITCH_IDL_MODALITY a2) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnContextFlagsChange(DWORD dwFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE OnTouchKeyboardManualInvoke() = 0;
};

extern "C" inline const IID IID_IInputSwitchCallback = __uuidof(IInputSwitchCallback);
// End internal inputswitchserver.h

MIDL_INTERFACE("d5ac38c1-7c50-4d6d-8392-6d717114084b")
IConsoleUIView : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Advise(INavigationCallback*) PURE;
	virtual HRESULT STDMETHODCALLTYPE Unadvise() PURE;
	virtual HRESULT STDMETHODCALLTYPE AppendControl(UINT, IConsoleUIControl*, IUnknown**) PURE;
	virtual HRESULT STDMETHODCALLTYPE WriteOutput(IUnknown*, const PCHAR_INFO, COORD, PSMALL_RECT) PURE;
	virtual HRESULT STDMETHODCALLTYPE GetColorAttributes(WORD*) PURE;
	virtual HRESULT STDMETHODCALLTYPE ResizeControl(IUnknown*, UINT) PURE;
	virtual HRESULT STDMETHODCALLTYPE RemoveAll() PURE;
	virtual HRESULT STDMETHODCALLTYPE GetConsoleWidth(UINT*) PURE;
	virtual HRESULT STDMETHODCALLTYPE SetCursorPos(IUnknown*, COORD, bool) PURE;
};

MIDL_INTERFACE("8224683b-9ca1-478d-82c2-f833b5a3e254")
IConsoleUIViewInternal : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetScreenBuffer(void**) PURE;
	virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(const KEY_EVENT_RECORD*) PURE;
	virtual HRESULT STDMETHODCALLTYPE InitializeFocus() PURE;
};

MIDL_INTERFACE("cb05bd7e-9f44-4dcb-8792-96e9630f1ae8")
IComboBoxSelectCallback : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE OnComboboxSelected(LCPD::IComboBoxField* comboBox) = 0;
};

class ConsoleUIView
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom> // @MOD This is not RuntimeClass but rather Implements
		, IConsoleUIView
		, IConsoleUIViewInternal
	>
{
public:
	ConsoleUIView();
	~ConsoleUIView()/* override*/;

	HRESULT Initialize();

	//~ Begin IConsoleUIView Interface
	STDMETHODIMP Advise(INavigationCallback*) override;
	STDMETHODIMP Unadvise() override;
	STDMETHODIMP AppendControl(UINT height, IConsoleUIControl* control, IUnknown** ppControlHandle) override;
	STDMETHODIMP WriteOutput(IUnknown* handle, PCHAR_INFO data, COORD dataSize, PSMALL_RECT writeRegion) override;
	STDMETHODIMP GetColorAttributes(WORD* pAttributes) override;
	STDMETHODIMP ResizeControl(IUnknown* handle, UINT newHeight) override;
	STDMETHODIMP RemoveAll() override;
	STDMETHODIMP GetConsoleWidth(UINT* pWidth) override;
	STDMETHODIMP SetCursorPos(IUnknown* handle, COORD position, bool isVisible) override;
	//~ End IConsoleUIView Interface

	//~ Begin IConsoleUIViewInternal Interface
	STDMETHODIMP GetScreenBuffer(void** pScreenBuffer) override;
	STDMETHODIMP HandleKeyInput(const KEY_EVENT_RECORD* keyEvent) override;
	STDMETHODIMP InitializeFocus() override;
	//~ End IConsoleUIViewInternal Interface

protected:
	virtual HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* keyHandled) PURE;
	virtual void v_Unadvise();

	int GetFocusIndex() const;

	Microsoft::WRL::ComPtr<INavigationCallback> m_navigationCallback;

private:
	HRESULT ShiftVisuals(UINT startIndex, int shiftDistance);
	HRESULT MoveFocusToNext();
	HRESULT MoveFocusToPrevious();

	wil::unique_handle m_screenBuffer;
	CCoSimpleArray<Microsoft::WRL::ComPtr<IControlHandle>> m_controlTable;
	int m_focusIndex;
};
