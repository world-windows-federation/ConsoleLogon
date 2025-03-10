#pragma once

#include "pch.h"
#include "logoninterfaces.h"

#include "credentialfieldcontrolbase.h"

class StaticTextControl final : public Microsoft::WRL::RuntimeClass<CredentialFieldControlBase>
{
public:
	StaticTextControl();
	~StaticTextControl() override;

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind) override;
	bool v_HasFocus() override;

	HRESULT Repaint(IConsoleUIView* view);

	Microsoft::WRL::ComPtr<LCPD::ICredentialTextField> m_dataSource;
	UINT m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
};
