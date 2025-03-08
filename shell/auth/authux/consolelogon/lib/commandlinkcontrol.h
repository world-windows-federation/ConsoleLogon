#pragma once

#include "pch.h"

#include "consoleuiview.h"
#include "credentialfieldcontrolbase.h"

class CommandLinkControl
	: public Microsoft::WRL::RuntimeClass<CredentialFieldControlBase>
{
public:
	CommandLinkControl();
	~CommandLinkControl() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(IConsoleUIView* view, LCPD::ICredentialField* dataSource);
	
private:
	HRESULT v_OnFocusChange(int hasFocus) override;
	HRESULT v_HandleKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled);
	HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind) override;
	bool v_HasFocus() override;
	HRESULT Repaint(IConsoleUIView* view);
	
	Microsoft::WRL::ComPtr<LCPD::ICommandLinkField> m_dataSource;
	unsigned int m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
	bool m_HasFocus;
};
