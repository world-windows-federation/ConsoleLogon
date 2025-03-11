#pragma once

#include "pch.h"

#include "consoleuiview.h"

class UserSelectionView final
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	UserSelectionView();
	~UserSelectionView() override;

	HRESULT RuntimeClassInitialize(LCPD::ICredProvDataModel* dataSource);

protected:
	HRESULT v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;

private:
	Microsoft::WRL::ComPtr<LCPD::ICredProvDataModel> m_dataModel;
};
