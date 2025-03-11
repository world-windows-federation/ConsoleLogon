#pragma once

#include "pch.h"

#include "controlbase.h"

class SelectableUserOrCredentialControl final
	: public Microsoft::WRL::RuntimeClass<ControlBase>
{
public:
	SelectableUserOrCredentialControl();

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, IInspectable* dataSource);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

	HRESULT Repaint(IConsoleUIView* view);

	Microsoft::WRL::ComPtr<LCPD::ICredential> m_dataSourceCredential;
	Microsoft::WRL::ComPtr<LCPD::IUser> m_dataSourceUser;
	UINT m_VisibleControlSize;
	bool m_isInitialized;
	bool m_HasFocus;
};
