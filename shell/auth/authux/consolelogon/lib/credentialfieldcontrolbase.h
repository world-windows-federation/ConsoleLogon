#pragma once

#include "pch.h"

#include "controlbase.h"

class CredentialFieldControlBase
	: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
		, ControlBase
		, WF::ITypedEventHandler<LCPD::ICredentialField*, LCPD::CredentialFieldChangeKind>
		, IQueryFocus
	>
{
public:
	CredentialFieldControlBase();
	CredentialFieldControlBase(const CredentialFieldControlBase& other) = delete;

	HRESULT Advise(LCPD::ICredentialField* dataSource);

	//~ Begin WF::ITypedEventHandler<LCPD::ICredentialField*, LCPD::CredentialFieldChangeKind> Interface
	STDMETHODIMP Invoke(LCPD::ICredentialField* sender, LCPD::CredentialFieldChangeKind args) override;
	//~ End WF::ITypedEventHandler<LCPD::ICredentialField*, LCPD::CredentialFieldChangeKind> Interface

	//~ Begin IQueryFocus Interface
	STDMETHODIMP_(BOOL) HasFocus() override;
	//~ End IQueryFocus Interface

	CredentialFieldControlBase& operator=(const CredentialFieldControlBase&) = delete;

protected:
	~CredentialFieldControlBase();

	HRESULT v_Unadvise() override;
	HRESULT GetVisibility(bool* pIsVisible);

	virtual HRESULT v_OnFieldChange(LCPD::CredentialFieldChangeKind changeKind) PURE;
	virtual bool v_HasFocus() PURE;

	Microsoft::WRL::ComPtr<LCPD::ICredentialField> m_FieldInfo;

private:
	EventRegistrationToken m_token;
};
