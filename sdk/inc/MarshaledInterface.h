#pragma once

#include <Windows.h>

#include <type_traits>
#include <wrl/implements.h>

#include "AgileGitPtr.h"

enum MARSHAL_KIND
{
	MK_WEAK = 1,
	MK_STRONG = 2
};

class CMarshaledInterface
{
public:
	CMarshaledInterface()
	{
	}

	template <typename I>
	CMarshaledInterface(I* pI, MARSHAL_KIND mk)
	{
		Marshal(__uuidof(I), pI, mk);
	}

	CMarshaledInterface(const CMarshaledInterface& other)
		: m_marshal(other.m_marshal)
	{
	}

	CMarshaledInterface(CMarshaledInterface&& other) noexcept
		: m_marshal(std::move(other.m_marshal))
	{
	}

	CMarshaledInterface& operator=(const CMarshaledInterface& other)
	{
		m_marshal = other.m_marshal;
		return *this;
	}

	CMarshaledInterface& operator=(CMarshaledInterface&& other) noexcept
	{
		m_marshal = std::move(other.m_marshal);
		return *this;
	}

	HRESULT Marshal(REFIID riid, IUnknown* punk, MARSHAL_KIND mk)
	{
		return MakeAndInitialize<CMarshalStream>(&m_marshal, riid, punk, mk);
	}

	HRESULT Unmarshal(REFIID riid, void** ppv)
	{
		return _Unmarshal(riid, ppv, false);
	}

	HRESULT UnmarshalAndRelease(REFIID riid, void** ppv)
	{
		return _Unmarshal(riid, ppv, true);
	}

	void Release()
	{
		m_marshal.Reset();
	}

	bool HasStream() const
	{
		return m_marshal.Get() != nullptr;
	}

	HRESULT AttachStream(IStream*, MSHLFLAGS);

private:
	HRESULT _Unmarshal(REFIID riid, void** ppv, bool fRelease)
	{
		*ppv = nullptr;
		HRESULT hr = E_FAIL;
		if (m_marshal.Get())
		{
			Microsoft::WRL::ComPtr<CMarshalStream> spMarshal;
			if (!fRelease && m_marshal->CanUnmarshalMultipleTimes())
			{
				spMarshal = m_marshal;
			}
			else
			{
				spMarshal.Attach(m_marshal.Detach());
			}
			hr = spMarshal->Unmarshal(riid, ppv);
		}
		return hr;
	}

	class CMarshalStream : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, IUnknown>
	{
	public:
		HRESULT RuntimeClassInitialize(IStream*);

		HRESULT RuntimeClassInitialize(REFIID riid, IUnknown* punk, MARSHAL_KIND mk)
		{
			m_marshalThreadId = GetCurrentThreadId();
			m_marshalingKind = mk;

			if (CanUnmarshalMultipleTimes())
			{
				return m_agileRef.Initialize(AGILEREFERENCE_DEFAULT, riid, punk);
			}
			else
			{
				return CoMarshalInterThreadInterfaceInStream(riid, punk, m_stream.ReleaseAndGetAddressOf());
			}
		}

		~CMarshalStream() override
		{
			if (m_stream.Get())
			{
				if (m_marshalThreadId == GetCurrentThreadId())
				{
					CoReleaseMarshalData(m_stream.Get());
					m_stream.Reset();
				}
				else
				{
					Microsoft::WRL::ComPtr<IUnknown> spUnknown;
					CoGetInterfaceAndReleaseStream(m_stream.Detach(), IID_PPV_ARGS(&spUnknown));
				}
			}
		}

		bool CanUnmarshalMultipleTimes() const
		{
			return m_marshalingKind == MK_STRONG;
		}

		HRESULT Unmarshal(REFIID riid, void** ppv)
		{
			*ppv = nullptr;

			if (CanUnmarshalMultipleTimes())
			{
				return m_agileRef.CopyLocal(riid, ppv);
			}
			else
			{
				return CoGetInterfaceAndReleaseStream(m_stream.Detach(), riid, ppv);
			}
		}

	private:
		Microsoft::WRL::ComPtr<IStream> m_stream;
		MARSHAL_KIND m_marshalingKind = MK_STRONG;
		AgileGitPtr m_agileRef;
		DWORD m_marshalThreadId = 0;
	};

	Microsoft::WRL::ComPtr<CMarshalStream> m_marshal;
};
