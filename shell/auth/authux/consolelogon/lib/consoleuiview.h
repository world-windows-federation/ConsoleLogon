#pragma once
#include <windows.h>
#include <Windows.Foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\event.h>
#include <wrl\implements.h>
#include <wrl\client.h>

#include "logoninterfaces.h"

struct INavigationCallback : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE OnNavigation() PURE;
	virtual HRESULT STDMETHODCALLTYPE ShowComboBox(Windows::Internal::UI::Logon::CredProvData::IComboBoxField*) PURE;
};

struct IConsoleUIControl : IUnknown
{
	virtual int STDMETHODCALLTYPE IsFocusable() PURE;
	virtual HRESULT STDMETHODCALLTYPE OnFocusChange(int) PURE;
	virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(const PKEY_EVENT_RECORD, int*) PURE;
	virtual HRESULT STDMETHODCALLTYPE Unadvise() PURE;
};

// Belongs to internal inputswitchserver.h
DEFINE_GUID(CLSID_InputSwitchControl,
	0xb9bc2a50,
	0x43c3, 0x41aa, 0xa0, 0x86,
	0x5d, 0xb1, 0x4e, 0x18, 0x4b, 0xae
);

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
	virtual HRESULT STDMETHODCALLTYPE SetCursorPos(IUnknown*, COORD, bool) PURE;
};

struct IConsoleUIViewInternal : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE GetScreenBuffer(void**) PURE;
	virtual HRESULT STDMETHODCALLTYPE HandleKeyInput(PKEY_EVENT_RECORD) PURE;
	virtual HRESULT STDMETHODCALLTYPE InitializeFocus() PURE;
};