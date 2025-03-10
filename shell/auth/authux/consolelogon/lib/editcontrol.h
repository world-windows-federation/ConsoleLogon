#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "credentialfieldcontrolbase.h"

class EditControl final : public Microsoft::WRL::RuntimeClass<CredentialFieldControlBase>
{
public:
	EditControl();
	~EditControl() override;

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind) override;
	bool v_HasFocus() override;

	HRESULT Repaint(IConsoleUIView* view);

	Microsoft::WRL::ComPtr<LCPD::ICredentialEditField> m_dataSource;
	UINT m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
	bool m_HasFocus;
	UINT m_EditStartIndex;
};
