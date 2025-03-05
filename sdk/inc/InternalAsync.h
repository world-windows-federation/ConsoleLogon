#pragma once

#define BUILD_WINDOWS
#include <wrl/async.h>
#include "GitPtr.h"
#include "MarshaledInterface.h"

namespace Windows::Internal
{
	enum class TaskApartment;
	enum class TaskOptions;
	interface IComPoolTask;
}

namespace Windows::Internal
{
	enum class TaskApartment
	{
		MTA,
		STA,
		Calling,
		Any,
		Synchronous,
		STAIfSupported,
		SynchronousUnlessInSTA,
	};

	enum class TaskOptions
	{
		None = 0x0,
		CreateObject = 0x1,
		Ordered = 0x2,
		Marshal = 0x4,
		AffinityHint = 0x8,
		BypassLimits = 0x10,
		Wait = 0x20,
		WaitWithCalls = 0x40,
		SyncNesting = 0x80,
		CancelPending = 0x100,
		NoWait = 0x200,
		PriorityMask = 0xF000,
		BackgroundMode = 0x1000,
		IdlePriority = 0x2000,
		LowestPriority = 0x3000,
		LowerPriority = 0x4000,
		NormalPriority = 0x5000,
		HigherPriority = 0x6000,
		HighestPriority = 0x7000,
		CriticalPriority = 0x8000,
	};

	MIDL_INTERFACE("47cfcc0e-6012-43ca-81a9-ab7bc86ad5d4")
	IComPoolTask : IUnknown
	{
		virtual void STDMETHODCALLTYPE Run() = 0;
	};
}

namespace Windows::Internal
{
	struct IAsyncFireCompletion : IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE InvokeFireCompletion() PURE;
	};

	class ComTaskPoolHandler
	{
		static constexpr long c_lMaxThreadRecursionDepth = 4;

	public:
		ComTaskPoolHandler(TaskApartment apartment, TaskOptions options)
			: _apartment(apartment)
			, _options(options)
			, _dwThreadId(0)
		{
		}

		HRESULT Start(IComPoolTask*);
		HRESULT FireCompletion(IAsyncFireCompletion*);
		HRESULT _FireCompletion(IAsyncFireCompletion*);
		void Cancel();
		void OnComplete();

	private:
		TaskApartment _apartment;
		TaskOptions _options;
		DWORD _dwThreadId;
	};

	MIDL_INTERFACE("5fb52445-1407-4f25-9aa4-ac25bb3a9606")
	IAsyncDeferral : IUnknown
	{
		virtual void STDMETHODCALLTYPE Complete(HRESULT) = 0;
	};

	template <typename TResult>
	class AsyncDeferral
	{
	public:
		AsyncDeferral(TResult& result, IAsyncDeferral* pDeferral)
			: _result(result)
			, _spDeferral(pDeferral)
		{
		}

		AsyncDeferral(const AsyncDeferral&) = default;
		AsyncDeferral(AsyncDeferral&&) noexcept = default;

		void Complete(HRESULT hr)
		{
			_spDeferral->Complete(hr);
			_spDeferral.Reset();
		}

		TResult& GetResult() const
		{
			return _result;
		}

	private:
		TResult& _result;
		Microsoft::WRL::ComPtr<IAsyncDeferral> _spDeferral;
	};

	class CResultBase
	{
	public:
		IAsyncInfo* GetAsyncInfo();
		bool IsCanceled();
		void SetAllowDismiss(bool);
		HRESULT MapErrorIfCanceled(HRESULT);
		IAsyncDeferral* GetDeferral();

		template <typename TResult>
		AsyncDeferral<TResult> GetDeferral(TResult& result)
		{
			Microsoft::WRL::ComPtr<IAsyncDeferral> spDeferral;
			_wpAsyncInfo->QueryInterface(IID_PPV_ARGS(&spDeferral));
			_fDeferred = true;
			return AsyncDeferral<TResult>(result, spDeferral.Get());
		}

		CResultBase();

		void SetAsyncAction(IAsyncInfo*);
		bool AllowDismiss();
		bool IsDeferred();

	private:
		IAsyncInfo* _wpAsyncInfo;
		bool _fAllowDismiss;
		bool _fDeferred;
	};

	template <typename TResult>
	class CMarshaledInterfaceResult : public CResultBase
	{
	public:
		static constexpr Microsoft::WRL::AsyncResultType c_callType = Microsoft::WRL::SingleResult;
		static constexpr bool c_fThreadAffineGetAndClose = false;

		TResult* Initial();

		HRESULT Get(TResult** ppResult)
		{
			*ppResult = 0;
			HRESULT hr = S_OK;
			if (_spLocal.IsInitialized())
			{
				hr = _spLocal.CopyLocal(ppResult);
			}
			else if (_spMarshal.HasStream())
			{
				hr = _spMarshal.Unmarshal(ppResult);
			}
			return hr;
		}

		void Set(CMarshaledInterface&);

		HRESULT Set(TResult* pResult)
		{
			return _spLocal.Initialize(pResult);
		}

		void Close();

	private:
		GitPtrSupportsAgile<TResult> _spLocal;
		CMarshaledInterface _spMarshal;
	};

	class CNoResult : public CResultBase
	{
	public:
		static constexpr Microsoft::WRL::AsyncResultType c_callType = Microsoft::WRL::SingleResult;
		static constexpr bool c_fThreadAffineGetAndClose = false;

		int Initial();
		HRESULT Get(int*);
		HRESULT Set(int);
		void Close();
	};

	enum class AsyncStage
	{
		Start = 0,
		Execute = 1,
		Cancel = 2,
	};

	template <typename TCallback>
	class AsyncCallbackBase
	{
	public:
		using TCallback = TCallback;

		virtual ~AsyncCallbackBase() = default;

		virtual HRESULT Run(AsyncStage stage, HRESULT hr, const TCallback& result) = 0;
	};

	template <
		typename TCall, // Windows::Internal::CCallAsyncStagedLambda<_lambda_6fd1045eef8a4720fd0f4574ed063cd5_ >
		typename TResult // Windows::Internal::CMarshaledInterfaceResult<IRequestCredentialsData>
	>
	class COperationLambda0 final : public AsyncCallbackBase<TResult>
	{
	public:
		COperationLambda0(const typename TCall::TCallback& call)
			: _call(call)
		{
		}

		COperationLambda0(const COperationLambda0&) = default;
		COperationLambda0(COperationLambda0&&) noexcept = default;

		HRESULT Run(AsyncStage stage, HRESULT hr, TResult& result) override
		{
			return _call(stage, hr, result);
		}

	private:
		TCall _call;
	};

	template <typename TLambda>
	class CCallAsyncLambda
	{
	public:
		CCallAsyncLambda(const TLambda& lambda)
			: _lambda(lambda)
		{
		}

		CCallAsyncLambda(const CCallAsyncLambda&) = default;
		CCallAsyncLambda(CCallAsyncLambda&&) noexcept = default;

		template <typename TResult>
		HRESULT operator()(AsyncStage stage, HRESULT hr, TResult& result)
		{
			if (stage == AsyncStage::Execute && SUCCEEDED(hr))
			{
				hr = _lambda(result);
			}
			return hr;
		}

	private:
		TLambda _lambda;
	};

	template <typename TLambda>
	class CCallAsyncStagedLambda
	{
	public:
		CCallAsyncStagedLambda(const TLambda& lambda)
			: _lambda(lambda)
		{
		}

		CCallAsyncStagedLambda(const CCallAsyncStagedLambda&) = default;
		CCallAsyncStagedLambda(CCallAsyncStagedLambda&&) noexcept = default;

		template <typename TResult>
		HRESULT operator()(AsyncStage stage, HRESULT hr, TResult& result)
		{
			return _lambda(stage, hr, result);
		}

	private:
		TLambda _lambda;
	};

	template <typename TResult, typename TLambda>
	COperationLambda0<CCallAsyncLambda<TLambda>, TResult>* MakeOperationLambda(const TLambda& lambda)
	{
		return new(std::nothrow) COperationLambda0<CCallAsyncLambda<TLambda>, TResult>(lambda);
	}

	template <typename TResult, typename TLambda>
	COperationLambda0<CCallAsyncStagedLambda<TLambda>, TResult>* MakeOperationStagedLambda(const TLambda& lambda)
	{
		return new(std::nothrow) COperationLambda0<CCallAsyncStagedLambda<TLambda>, TResult>(lambda);
	}

	struct INilDelegate : IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE Invoke(IUnknown*, int) = 0;
	};

	template <
		typename TOperation, // IAsyncOperation<RequestCredentialsData*>
		typename TCompleteHandler, // IAsyncOperationCompletedHandler<RequestCredentialsData*>
		typename TDelegate, // INilDelegate
		typename TResult, // CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData>
		typename TTaskPoolHandler, // ComTaskPoolHandler&
		typename TAsyncOptions // AsyncOptions<PropagateErrorWithWin8Quirk, nullptr, GUID_CAUSALITY_WINDOWS_PLATFORM_ID, Diagnostics::CausalitySource_System>
	>
	HRESULT MakeAsyncHelper(
		TOperation** ppOperation,
		TTaskPoolHandler handler,
		const WCHAR* const pszRuntimeName,
		TrustLevel trustLevel,
		AsyncCallbackBase<TResult>* pCallback)
	{
		*ppOperation = nullptr;

		HRESULT hr = pCallback ? S_OK : E_OUTOFMEMORY;
		if (hr >= 0)
		{
			auto spOperation = Microsoft::WRL::Make<AsyncOperation<TOperation, TCompleteHandler, TResult, TTaskPoolHandler, TDelegate, TAsyncOptions>>(handler, pszRuntimeName, trustLevel);
			hr = spOperation.Get() ? S_OK : E_OUTOFMEMORY;
			if (hr >= 0)
			{
				spOperation->_pCallback = pCallback;
				hr = spOperation->Start();
				pCallback = nullptr;
				if (hr >= 0)
				{
					*ppOperation = spOperation.Detach();
				}
			}
			if (pCallback)
				delete pCallback;
		}

		return hr;
	}
}
