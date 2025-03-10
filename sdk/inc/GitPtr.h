#pragma once

#include <wrl/client.h>

template <typename T>
class GitPtrSupportsAgile
{
public:
    GitPtrSupportsAgile()
    {
    }

    GitPtrSupportsAgile(const GitPtrSupportsAgile&) = default;
    GitPtrSupportsAgile(GitPtrSupportsAgile&&) noexcept = default;

    GitPtrSupportsAgile& operator=(const GitPtrSupportsAgile&) = default;
    GitPtrSupportsAgile& operator=(GitPtrSupportsAgile&&) noexcept = default;

    template <typename TInterface>
    HRESULT Initialize(TInterface* ptr)
    {
        Revoke();
        HRESULT hr = S_OK;
        if (ptr)
        {
            Microsoft::WRL::ComPtr<IAgileReference> spAgileReference;
            hr = RoGetAgileReference(AGILEREFERENCE_DEFAULT, __uuidof(TInterface), ptr, &spAgileReference);
            if (SUCCEEDED(hr))
            {
                _spAgileReference.Swap(spAgileReference);
            }
        }
        return hr;
    }

    bool IsInitialized() const
    {
        return _spAgileReference.Get() != nullptr;
    }

    HRESULT CopyLocal(REFIID riid, void** ptr)
    {
        HRESULT hr;
        Microsoft::WRL::ComPtr<IAgileReference> spAgileReference = _spAgileReference;
        if (spAgileReference.Get())
        {
            hr = spAgileReference->Resolve(riid, ptr);
        }
        else
        {
            hr = E_INVALIDARG;
            *ptr = nullptr;
        }
        return hr;
    }

    template <typename TDest>
    HRESULT CopyLocal(TDest** ptr)
    {
        return CopyLocal(__uuidof(TDest), reinterpret_cast<void**>(ptr));
    }

    HRESULT Revoke()
    {
        _spAgileReference.Reset();
        return S_OK;
    }

protected:
    Microsoft::WRL::ComPtr<IAgileReference> _spAgileReference;
};
