#pragma once

#include <intsafe.h>
#include <strsafe.h>

#include "ResultUtils.h"

namespace Windows::Internal
{
    class ResourceString
    {
    public:
        static bool FindAndSize(HINSTANCE hInstance, UINT uId, WORD wLanguage, const WCHAR** ppch, WORD* plen)
        {
            bool rv = false;
            *ppch = nullptr;
            if (plen)
                *plen = 0;
            HRSRC hRsrc = FindResourceExW(hInstance, RT_STRING, MAKEINTRESOURCEW((uId >> 4) + 1), wLanguage);
            if (hRsrc)
            {
                HGLOBAL hgRsrc = LoadResource(hInstance, hRsrc);
                if (hgRsrc)
                {
                    WCHAR* pRsrc = (WCHAR*)LockResource(hgRsrc);
                    if (pRsrc)
                    {
                        for (UINT v12 = (char)uId & 0xF; v12; --v12)
                            pRsrc += *pRsrc + 1;
                        *ppch = *pRsrc ? pRsrc + 1 : L"";
                        if (plen)
                            *plen = *pRsrc;
                        rv = true;
                    }
                }
            }
            return rv;
        }
    };

    template <typename ElementType>
    class CoTaskMemPolicy
    {
    public:
        static ElementType* Alloc(size_t bytes)
        {
            return (ElementType*)CoTaskMemAlloc(bytes);
        }

        static ElementType* Realloc(ElementType* p, size_t bytes)
        {
            return (ElementType*)CoTaskMemRealloc(p, bytes);
        }

        static void Free(ElementType* p)
        {
            CoTaskMemFree(p);
        }
    };

    template <typename ElementType>
    class LocalMemPolicy
    {
    public:
        static ElementType* Alloc(size_t bytes)
        {
            return (ElementType*)LocalAlloc(LMEM_FIXED, bytes);
        }

        static ElementType* Realloc(ElementType* p, size_t bytes)
        {
            return (ElementType*)LocalReAlloc(p, bytes, LMEM_MOVEABLE);
        }

        static void Free(ElementType* p)
        {
            LocalFree(p);
        }
    };

    template <typename Allocator>
    class NativeString
    {
    public:
        NativeString() : _pszStringData(nullptr), _cchStringData(0), _cchStringDataCapacity(0)
        {
        }

        NativeString(NativeString&& other) noexcept
            : _pszStringData(other._pszStringData)
            , _cchStringData(other._cchStringData)
            , _cchStringDataCapacity(other._cchStringDataCapacity)
        {
            other._pszStringData = nullptr;
            other._cchStringData = 0;
            other._cchStringDataCapacity = 0;
        }

    private:
        NativeString(const NativeString&) = delete;

    public:
        ~NativeString()
        {
            Free();
        }

        HRESULT Initialize(HINSTANCE hInstance, UINT uId)
        {
            const WCHAR* pch;
            WORD len;
            return ResourceString::FindAndSize(hInstance, uId, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), &pch, &len)
                       ? _Initialize(pch, len) : E_FAIL;
        }

        HRESULT Initialize(const NativeString& other)
        {
            return _Initialize(other._pszStringData, other.GetCount());
        }

        HRESULT Initialize(const WCHAR* psz)
        {
            return _Initialize(psz, (size_t)-1);
        }

        HRESULT Initialize(const WCHAR* psz, size_t cch)
        {
            return _Initialize(psz, cch);
        }

        HRESULT InitializeFormat(const WCHAR* pszFormat, ...)
        {
            va_list args;
            va_start(args, pszFormat);
            return InitializeFormat(pszFormat, args);
        }

        HRESULT InitializeFormat(const WCHAR* pszFormat, va_list argList)
        {
            return _InitializeHelper(pszFormat, argList, [](WCHAR* pszStringData, size_t cchStringDataCapacity, const WCHAR* pszFormat, va_list argList) -> HRESULT
            {
                _set_errno(0);
                HRESULT hr = StringCchVPrintfW(pszStringData, cchStringDataCapacity, pszFormat, argList);
                if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                {
                    errno_t v13;
                    _get_errno(&v13);
                    if (v13 == EINVAL)
                        return E_INVALIDARG;
                }
                return hr;
            });
        }

        HRESULT InitializeResFormat(HINSTANCE hInstance, UINT uId, ...)
        {
            va_list argList;
            va_start(argList, uId);
            NativeString spszFormat;
            HRESULT hr = spszFormat.Initialize(hInstance, uId);
            if (SUCCEEDED(hr))
            {
                hr = InitializeFormat(spszFormat._pszStringData, argList);
            }
            return hr;
        }

        HRESULT InitializeResMessage(HINSTANCE hInstance, UINT uId, ...)
        {
            va_list argList;
            va_start(argList, uId);
            NativeString spszFormat;
            HRESULT hr = spszFormat.Initialize(hInstance, uId);
            if (SUCCEEDED(hr))
            {
                hr = _InitializeHelper(spszFormat._pszStringData, argList, [](WCHAR* pszStringData, size_t cchStringDataCapacity, const WCHAR* pszFormat, va_list argList) -> HRESULT
                {
                    return FormatMessageW(FORMAT_MESSAGE_FROM_STRING, pszFormat, 0, 0, pszStringData, (DWORD)cchStringDataCapacity, &argList)
                        ? S_OK : ResultFromKnownLastError();
                });
            }
            return hr;
        }

        void Free()
        {
            _Free();
        }

        void Attach(WCHAR* psz)
        {
            Attach(psz, (size_t)-1);
        }

        void Attach(WCHAR* psz, size_t cch)
        {
            _Free();
            _pszStringData = psz;
            _cchStringData = cch;
            _cchStringDataCapacity = cch;
        }

        WCHAR* Detach()
        {
            WCHAR* pszStringData = _pszStringData;
            _pszStringData = nullptr;
            _cchStringData = 0;
            _cchStringDataCapacity = 0;
            return pszStringData;
        }

        HRESULT DetachInitializeIfEmpty(WCHAR** ppsz)
        {
            *ppsz = nullptr;
            HRESULT hr = S_OK;

            if (_pszStringData)
            {
                hr = Initialize(L"");
            }

            if (SUCCEEDED(hr))
            {
                *ppsz = Detach();
            }

            return hr;
        }

        WCHAR** FreeAndGetAddressOf()
        {
            _Free();
            _cchStringData = -1;
            _cchStringDataCapacity = -1;
            return &_pszStringData;
        }

        HRESULT CopyTo(WCHAR** ppszDest) const
        {
            HRESULT hr;
            *ppszDest = nullptr;
            if (_pszStringData)
            {
                NativeString spszT;
                hr = spszT.Initialize(*this);
                if (SUCCEEDED(hr))
                {
                    *ppszDest = spszT.Detach();
                }
            }
            else
            {
                hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            }
            return hr;
        }

        HRESULT CopyTo(WCHAR* pszDest, size_t cchDest) const
        {
            if (!_pszStringData)
            {
                if (cchDest)
                    *pszDest = 0;
                return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            }
            return StringCchCopyW(pszDest, cchDest, _pszStringData);
        }

        WCHAR* Get() const
        {
            return _pszStringData;
        }

        const WCHAR* GetNonNull() const
        {
            return _pszStringData ? _pszStringData : L"";
        }

        size_t GetCount()
        {
            _EnsureCount();
            return _cchStringData;
        }

        size_t GetCount() const
        {
            if (_cchStringData != -1)
                return _cchStringData;
            return _pszStringData ? wcslen(_pszStringData) : 0;
        }

        bool IsEmpty() const
        {
            return _IsEmpty();
        }

        bool HasLength() const
        {
            return !_IsEmpty();
        }

        int CompareOrdinal(const NativeString& other) const
        {
            return CompareOrdinal(other.GetNonNull(), other.GetCount());
        }

        int CompareOrdinal(const WCHAR* psz) const
        {
            return CompareOrdinal(psz, -1);
        }

        int CompareOrdinal(const WCHAR* psz, size_t cch) const
        {
            return CompareStringOrdinal(GetNonNull(), (int)GetCount(), psz ? psz : L"", psz ? (int)cch : 0, FALSE);
        }

        int CompareOrdinalIgnoreCase(const NativeString& other) const
        {
            return CompareOrdinalIgnoreCase(other.GetNonNull(), other.GetCount());
        }

        int CompareOrdinalIgnoreCase(const WCHAR* psz) const
        {
            return CompareOrdinalIgnoreCase(psz, -1);
        }

        int CompareOrdinalIgnoreCase(const WCHAR* psz, size_t cch) const
        {
            return CompareStringOrdinal(GetNonNull(), (int)GetCount(), psz ? psz : L"", psz ? (int)cch : 0, TRUE);
        }

        HRESULT Concat(WCHAR c)
        {
            WCHAR sz[2] = { c, 0 };
            return _Concat(sz, 1);
        }

        HRESULT Concat(const WCHAR* psz)
        {
            return _Concat(psz, psz ? wcslen(psz) : 0);
        }

        HRESULT Concat(const WCHAR* psz, const size_t cch)
        {
            return _Concat(psz, cch);
        }

        HRESULT Concat(const NativeString& other)
        {
            return _Concat(other.Get(), other.GetCount());
        }

        HRESULT ConcatFormat(const WCHAR* pszFormat, ...)
        {
            va_list argList;
            va_start(argList, pszFormat);
            return ConcatFormat(pszFormat, argList);
        }

        HRESULT ConcatFormat(const WCHAR* pszFormat, va_list argList)
        {
            if (IsEmpty())
            {
                return InitializeFormat(pszFormat, argList);
            }
            NativeString strT;
            HRESULT hr = strT.InitializeFormat(pszFormat, argList);
            if (SUCCEEDED(hr))
            {
                hr = Concat(strT);
            }
            return hr;
        }

        bool RemoveAt(size_t iElem, size_t cchElem)
        {
            return _RemoveAt(iElem, cchElem);
        }

        bool TrimStart(const WCHAR* pszTrim)
        {
            return _TrimStart(pszTrim);
        }

        bool TrimEnd(const WCHAR* pszTrim)
        {
            return _TrimEnd(pszTrim);
        }

        inline static const WCHAR* const s_pszTrimWhitespaceCharacterSet =
            L"\u0020" // Space
            L"\u0009" // Tab
            L"\u3000" // Ideographic Space
            L"\u17D2" // Khmer Sign Coeng
            L"\u0F0B" // Tibetan Mark Intersyllabic Tsheg
            L"\u1680" // Ogham Space Mark
            L"\u180E" // Mongolian Vowel Separator
        ;

        bool TrimWhitespace()
        {
            bool fWasCharacterTrimmedEnd = _TrimEnd(s_pszTrimWhitespaceCharacterSet);
            bool fWasCharacterTrimmedStart = _TrimStart(s_pszTrimWhitespaceCharacterSet);
            return fWasCharacterTrimmedStart || fWasCharacterTrimmedEnd;
        }

        void ReplaceChars(const WCHAR wcFind, const WCHAR wcReplace)
        {
            _EnsureCount();
            for (size_t i = 0; i < _cchStringData; i++)
            {
                if (_pszStringData[i] == wcFind)
                    _pszStringData[i] = wcReplace;
            }
        }

        NativeString& operator=(NativeString&& other) noexcept
        {
            _Free();
            _pszStringData = other._pszStringData;
            _cchStringData = other._cchStringData;
            _cchStringDataCapacity = other._cchStringDataCapacity;
            other._pszStringData = nullptr;
            other._cchStringData = 0;
            other._cchStringDataCapacity = 0;
            return *this;
        }

    private:
        NativeString& operator=(const NativeString& other) = delete;

    public:
        WCHAR** operator&()
        {
            return FreeAndGetAddressOf();
        }

        bool operator==(const WCHAR* pszOther) const
        {
            return pszOther ? CompareOrdinal(pszOther) == CSTR_EQUAL : !_pszStringData;
        }

        bool operator!=(const WCHAR* pszOther) const
        {
            return !operator==(pszOther);
        }

        HRESULT EnsureCapacity(size_t cchDesired)
        {
            return _EnsureCapacity(cchDesired);
        }

    private:
        void _EnsureCount()
        {
            if (_cchStringData == -1)
            {
                _cchStringData = _pszStringData ? wcslen(_pszStringData) : 0;
            }
        }

        HRESULT _EnsureCapacity(size_t cchDesired)
        {
            size_t desiredCapacity = cchDesired + 1;
            if (cchDesired + 1 < cchDesired)
                return INTSAFE_E_ARITHMETIC_OVERFLOW;
            if (_cchStringDataCapacity == -1)
            {
                _EnsureCount();
                _cchStringDataCapacity = _pszStringData ? _cchStringData + 1 : 0;
            }
            if (_cchStringDataCapacity == 0) // First allocation
            {
                size_t bytes;
                HRESULT hr = SizeTMult(desiredCapacity, sizeof(WCHAR), &bytes);
                if (FAILED(hr))
                    return hr;
                WCHAR* newStringData = Allocator::Alloc(bytes);
                if (!newStringData)
                    return E_OUTOFMEMORY;
                _cchStringDataCapacity = desiredCapacity;
                _pszStringData = newStringData;
                newStringData[0] = 0;
            }
            else if (desiredCapacity > _cchStringDataCapacity) // Growing
            {
                size_t newCapacity;
                HRESULT hr = SizeTMult(_cchStringDataCapacity, 2, &newCapacity); // Double the capacity
                if (FAILED(hr))
                    return hr;
                if (newCapacity - _cchStringDataCapacity > 2048)
                    newCapacity = _cchStringDataCapacity + 2048; // Make sure it doesn't grow too much; TODO Check disassembly
                if (desiredCapacity <= newCapacity)
                    desiredCapacity = newCapacity;
                WCHAR* reallocatedStringData = Allocator::Realloc(_pszStringData, sizeof(WCHAR) * desiredCapacity);
                if (!reallocatedStringData)
                    return E_OUTOFMEMORY;
                _cchStringDataCapacity = desiredCapacity;
                _pszStringData = reallocatedStringData;
            }
            return S_OK;
        }

        bool _IsEmpty() const
        {
            return !_pszStringData || !_pszStringData[0];
        }

        HRESULT _Initialize(const WCHAR* psz, size_t cch)
        {
            size_t cchStringDataCapacity = cch;
            size_t cchStringData;
            HRESULT hr = S_OK;
            if (psz)
            {
                if (cch == -1)
                {
                    cchStringDataCapacity = wcslen(psz);
                    cchStringData = cchStringDataCapacity;
                }
                else if (cch >= wcslen(psz))
                {
                    cchStringData = wcslen(psz);
                }
                else
                {
                    cchStringData = cchStringDataCapacity;
                }
                hr = _EnsureCapacity(cchStringDataCapacity);
                if (SUCCEEDED(hr))
                {
                    StringCchCopyNW(_pszStringData, cchStringDataCapacity + 1, psz, cchStringData);
                    _cchStringData = cchStringData;
                }
            }
            else
            {
                _Free();
            }
            return hr;
        }

        template <typename T>
        HRESULT _InitializeHelper(const WCHAR* pszFormat, va_list argList, const T& callback)
        {
            HRESULT hr;
            size_t v6 = 32;
            while (true)
            {
                hr = _EnsureCapacity(v6);
                if (FAILED(hr))
                    break;

                hr = callback(_pszStringData, _cchStringDataCapacity, pszFormat, argList);
                if (hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
                    break;

                v6 = _cchStringDataCapacity + 32;
                if (v6 < _cchStringDataCapacity)
                {
                    hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
                    break;
                }
                // Keep growing
            }
            if (FAILED(hr))
            {
                _Free();
                return hr;
            }
            _cchStringData = -1;
            return hr;
        }

        HRESULT _Concat(const WCHAR* psz, size_t cch)
        {
            HRESULT hr = S_OK;
            if (psz)
            {
                _EnsureCount();
                hr = _EnsureCapacity(cch + _cchStringData);
                if (SUCCEEDED(hr))
                {
                    StringCchCopyNW(&_pszStringData[_cchStringData], cch + 1, psz, cch);
                    _cchStringData += cch;
                }
            }
            return hr;
        }

        bool _RemoveAt(size_t iElem, size_t cchElem)
        {
            _EnsureCount();

            bool fRet = false;

            if (iElem < _cchStringData)
            {
                size_t removeLen = min(cchElem, _cchStringData - iElem);
                if (removeLen)
                {
                    memmove(&_pszStringData[iElem], &_pszStringData[iElem + removeLen], sizeof(WCHAR) * (_cchStringData - iElem - removeLen));
                    _cchStringData -= removeLen;
                }
                _pszStringData[_cchStringData] = 0;
                fRet = true;
            }

            return fRet;
        }

        bool _TrimStart(const WCHAR* pszTrim)
        {
            _EnsureCount();

            bool fNeedsTrimming = false;

            size_t cch;
            for (cch = 0; cch < _cchStringData; ++cch)
            {
                if (!wcschr(pszTrim, _pszStringData[cch]))
                    break;
            }

            if (cch)
            {
                fNeedsTrimming = true;
                memmove(_pszStringData, &_pszStringData[cch], sizeof(WCHAR) * (_cchStringData - cch) + sizeof(WCHAR));
                _cchStringData -= cch;
            }

            return fNeedsTrimming;
        }

        bool _TrimEnd(const WCHAR* pszTrim)
        {
            _EnsureCount();

            size_t cch;
            for (cch = _cchStringData; cch; --cch)
            {
                if (!wcschr(pszTrim, _pszStringData[cch - 1]))
                    break;
            }

            bool fNeedsTrimming = false;

            if (cch != _cchStringData)
            {
                fNeedsTrimming = true;
                _pszStringData[cch] = 0;
                _cchStringData = cch;
            }

            return fNeedsTrimming;
        }

        void _Free()
        {
            if (_pszStringData)
            {
                Allocator::Free(_pszStringData);
                _pszStringData = nullptr;
            }
            _cchStringData = 0;
            _cchStringDataCapacity = 0;
        }

        WCHAR* _pszStringData;
        size_t _cchStringData;
        size_t _cchStringDataCapacity;
    };
}

typedef Windows::Internal::NativeString<Windows::Internal::CoTaskMemPolicy<WCHAR>> CoTaskMemNativeString;
