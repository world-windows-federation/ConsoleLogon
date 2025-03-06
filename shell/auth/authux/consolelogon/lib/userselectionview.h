#pragma once

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

#include "consoleuiview.h"
#include "controlbase.h"
#include "InternalAsync.h"
#include "logoninterfaces.h"

class UserSelectionView
	: public Microsoft::WRL::RuntimeClass<ConsoleUIView>
{
public:
	UserSelectionView();
	~UserSelectionView() override;

	// ReSharper disable once CppHidingFunction
	HRESULT RuntimeClassInitialize(LCPD::ICredProvDataModel* dataSource);
	
protected:
	HRESULT v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled) override;
	
private:
	Microsoft::WRL::ComPtr<LCPD::ICredProvDataModel> m_dataModel;
};
