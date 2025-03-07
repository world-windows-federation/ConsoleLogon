#include "lockedview.h"
#include "controlhandle.h"
#include "SimpleArray.h"
#include <wil\resource.h>

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

LockedView::LockedView()
{
}

LockedView::~LockedView()
{
}

HRESULT LockedView::RuntimeClassInitialize()
{
	RETURN_IF_FAILED(Initialize()); // 18

	ComPtr<BasicTextControl> control;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&control, this, 100)); // 21

	return S_OK;
}

HRESULT LockedView::v_OnKeyInput(KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = true;
	RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 30

	return S_OK;
}
