#include "pch.h"

#include "credentialfieldcontrolbase.h"

#include "basictextcontrol.h"

//CredentialFieldControlBase::CredentialFieldControlBase(CredentialFieldControlBase& other)
//{
//}

CredentialFieldControlBase::CredentialFieldControlBase()
	: m_token()
{
	//can't use colon member initialization method here
	m_isFocusable = false;
}

HRESULT CredentialFieldControlBase::Advise(LCPD::ICredentialField* dataSource)
{
	//TODO: verify that operator= is right here
	m_FieldInfo = dataSource;

	RETURN_IF_FAILED(m_FieldInfo->add_FieldChanged(this,&m_token)); // 19
	return S_OK;
}

HRESULT CredentialFieldControlBase::Invoke(LCPD::ICredentialField* sender, LCPD::CredentialFieldChangeKind args)
{
	if (!m_token.value)
		return S_OK;

	RETURN_IF_FAILED(this->v_OnFieldChange(args)); // 34

	return S_OK;
}

int CredentialFieldControlBase::HasFocus()
{
	return v_HasFocus();
}

//CredentialFieldControlBase& CredentialFieldControlBase::operator=(CredentialFieldControlBase&)
//{
//}

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

	bool isVisible = false;
	
	bool isHidden = false;
	RETURN_IF_FAILED(m_FieldInfo->get_IsHidden(&isHidden)); // 49

	bool isVisibleInSelectedTile = false;
	RETURN_IF_FAILED(m_FieldInfo->get_IsVisibleInSelectedTile(&isVisibleInSelectedTile)); //52

	if (!isHidden)
		isVisible = isVisibleInSelectedTile != 0;
	*pIsVisible = isVisible;
	
	return S_OK;
}
