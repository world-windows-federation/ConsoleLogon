#pragma once

#include <Windows.h>

#include <wrl/client.h>

class AgileGitPtr
{
public:
	HRESULT Initialize(AgileReferenceOptions aro, REFIID riid, IUnknown* punk)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<IAgileReference> newRef;
		if (punk)
			hr = RoGetAgileReference(aro, riid, punk, newRef.ReleaseAndGetAddressOf());
		newRef.Swap(m_agileRef);
		return hr;
	}

	bool IsInitialized() const
	{
		return m_agileRef.Get() != nullptr;
	}

	HRESULT CopyLocal(REFIID riid, void** ppv) const
	{
		if (!IsInitialized())
		{
			*ppv = nullptr;
			return E_INVALIDARG;
		}
		return m_agileRef.CopyTo(riid, (IUnknown**)ppv);
	}

	template <typename T>
	HRESULT CopyLocal(Microsoft::WRL::Details::ComPtrRef<Microsoft::WRL::ComPtr<T>> ptr) const
	{
		return CopyLocal(__uuidof(T), ptr);
	}

	HRESULT Revoke()
	{
		m_agileRef.Swap(nullptr); // @Note: Yes, null ComPtr construction here, and then swapping. Could've been .Reset() but this is what they wrote.
		return S_OK;
	}

	Microsoft::WRL::AgileRef m_agileRef;
};
