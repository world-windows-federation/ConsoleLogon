#include "pch.h"
#include "logoninterfaces.h"

#include "credentialfieldcontrolbase.h"

class StaticTextControl
	: public Microsoft::WRL::RuntimeClass<
		CredentialFieldControlBase
	>
{
public:
	StaticTextControl();
	~StaticTextControl();

	HRESULT RuntimeClassInitialize(IConsoleUIView* view, Windows::Internal::UI::Logon::CredProvData::ICredentialField*);

private:
	HRESULT v_OnFocusChange(BOOL hasFocus) override;
	HRESULT v_HandleKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	HRESULT v_OnFieldChange(Windows::Internal::UI::Logon::CredProvData::CredentialFieldChangeKind) override;
	virtual bool v_HasFocus() PURE;

	HRESULT Repaint(IConsoleUIView* view);

	Microsoft::WRL::ComPtr<Windows::Internal::UI::Logon::CredProvData::ICredentialTextField> m_dataSource;
	UINT m_VisibleControlSize;
	bool m_isInitialized;
	bool m_IsVisible;
};
