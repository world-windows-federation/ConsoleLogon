#include "pch.h"

#include "credentialfieldcontrolbase.h"

CredentialFieldControlBase::CredentialFieldControlBase()
	: m_token()
{
}

HRESULT CredentialFieldControlBase::Advise(LCPD::ICredentialField* dataSource)
{
	m_FieldInfo = dataSource;

	RETURN_IF_FAILED(m_FieldInfo->add_FieldChanged(this, &m_token)); // 19
	return S_OK;
}

HRESULT CredentialFieldControlBase::Invoke(LCPD::ICredentialField* sender, LCPD::CredentialFieldChangeKind args)
{
	if (!m_FieldInfo.Get())
		return S_OK;

	RETURN_IF_FAILED(v_OnFieldChange(args)); // 34

	return S_OK;
}

int CredentialFieldControlBase::HasFocus()
{
	return v_HasFocus();
}

CredentialFieldControlBase::~CredentialFieldControlBase()
{
}

HRESULT CredentialFieldControlBase::v_Unadvise()
{
	RETURN_IF_FAILED(m_FieldInfo->remove_FieldChanged(m_token)); // 25

	m_FieldInfo.Reset();
	return S_OK;
}

HRESULT CredentialFieldControlBase::GetVisibility(bool* pIsVisible)
{
	*pIsVisible = false;

	bool isHidden;
	RETURN_IF_FAILED(m_FieldInfo->get_IsHidden(&isHidden)); // 49

	bool isVisibleInSelectedTile;
	RETURN_IF_FAILED(m_FieldInfo->get_IsVisibleInSelectedTile(&isVisibleInSelectedTile)); //52

	*pIsVisible = !isHidden && isVisibleInSelectedTile != 0;

	return S_OK;
}
