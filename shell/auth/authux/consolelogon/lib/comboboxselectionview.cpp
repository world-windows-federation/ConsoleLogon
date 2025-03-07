#include "pch.h"

#include "comboboxselectionview.h"

#include "basictextcontrol.h"

using namespace Microsoft::WRL;

ComboboxSelectionView::ComboboxSelectionView()
	: m_itemsChangedToken()
{
}

ComboboxSelectionView::~ComboboxSelectionView()
{
}

HRESULT ComboboxSelectionView::RuntimeClassInitialize(LCPD::IComboBoxField* dataSource)
{
	RETURN_IF_FAILED(Initialize()); // 22

	m_dataSource = dataSource;
	RETURN_IF_FAILED(m_dataSource->get_Items(&m_items)); // 25

	RETURN_IF_FAILED(m_items->add_VectorChanged(this, &m_itemsChangedToken)); // 27

	RETURN_IF_FAILED(Reset());
	return S_OK;
}

HRESULT ComboboxSelectionView::Invoke(WFC::IObservableVector<HSTRING>* sender, WFC::IVectorChangedEventArgs* args)
{
	RETURN_IF_FAILED(ConsoleUIView::RemoveAll()); // 36
	RETURN_IF_FAILED(Reset()); // 37
	return S_OK;
}

HRESULT ComboboxSelectionView::v_OnKeyInput(const KEY_EVENT_RECORD* keyEvent, BOOL* wasHandled)
{
	*wasHandled = FALSE;

	switch (keyEvent->wVirtualKeyCode)
	{
		case VK_BACK:
		case VK_ESCAPE:
		{
			RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 49
			*wasHandled = TRUE;
			break;
		}
		case VK_RETURN:
		{
			RETURN_IF_FAILED(m_dataSource->put_SelectedIndex(GetFocusIndex() - 1)); // 58
			RETURN_IF_FAILED(m_navigationCallback->OnNavigation()); // 59
			*wasHandled = TRUE;
			break;
		}
	}

	return S_OK;
}

void ComboboxSelectionView::v_Unadvise()
{
	if (m_items.Get() && m_itemsChangedToken.value)
	{
		m_items->remove_VectorChanged(m_itemsChangedToken);
		m_items.Reset();
	}
}

HRESULT ComboboxSelectionView::Reset()
{
	CoTaskMemNativeString navigationString;
	RETURN_IF_FAILED(navigationString.InitializeResFormat(HINST_THISCOMPONENT, 104)); // 82

	ComPtr<BasicTextControl> navigationMessage;
	RETURN_IF_FAILED(MakeAndInitialize<BasicTextControl>(&navigationMessage, this, navigationString.Get(), false)); // 86

	ComPtr<WFC::IVector<HSTRING>> items;
	RETURN_IF_FAILED(m_items.As<WFC::IVector<HSTRING>>(&items)); // 89

	UINT numItems;
	RETURN_IF_FAILED(items->get_Size(&numItems)); // 92

	for (UINT i = 0; i < numItems; ++i)
	{
		Wrappers::HString option;
		RETURN_IF_FAILED(items->GetAt(i, option.ReleaseAndGetAddressOf())); // 97

		ComPtr<BasicTextControl> control;
		RETURN_IF_FAILED(Details::MakeAndInitialize<BasicTextControl>(&control, this, option.GetRawBuffer(nullptr), true)); // 100
	}

	return S_OK;
}
