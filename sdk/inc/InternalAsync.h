#pragma once

// #define BUILD_WINDOWS
#include <wrl/async.h>
#include "GitPtr.h"
#include "MarshaledInterface.h"

class RpcOptionsHelper
{
public:
	static HRESULT GetRpcOptions(IUnknown* pInterface, IRpcOptions** ppRpcOptions)
	{
		return GetRpcOptions(pInterface, true, ppRpcOptions);
	}

	static HRESULT GetRpcOptions(IUnknown* pInterface, bool localOnly, IRpcOptions** ppRpcOptions)
	{
		*ppRpcOptions = nullptr;
		if (!pInterface)
			return E_NOINTERFACE;

		Microsoft::WRL::ComPtr<IRpcOptions> spRpcOptions;
		HRESULT hr = pInterface->QueryInterface(IID_PPV_ARGS(&spRpcOptions));

		if (SUCCEEDED(hr) && localOnly)
		{
			ULONG_PTR value;
			hr = spRpcOptions->Query(pInterface, COMBND_SERVER_LOCALITY, &value);
			if (SUCCEEDED(hr))
			{
				hr = value == SERVER_LOCALITY_MACHINE_LOCAL ? S_OK : E_NOINTERFACE;
			}
		}

		if (SUCCEEDED(hr))
		{
			*ppRpcOptions = spRpcOptions.Detach();
		}

		return hr;
	}

	static HRESULT GetRpcOptionsAndSetDoNotDisturb(IUnknown* pInterface, IRpcOptions** ppRpcOptions);
	static HRESULT SetDoNotDisturb(IUnknown*);

private:
	void SetDoNotDisturbInternal(IRpcOptions*, IUnknown*);
};

template <typename TAction, typename TComplete>
class AutoStubBias
{
public:
	AutoStubBias(IRpcOptions* pRpcOptions, TAction* stubInterface, TComplete* delegateInterface)
		: hr_(S_OK)
	{
		if (pRpcOptions && delegateInterface)
		{
			hr_ = CreateStreamOnHGlobal(nullptr, TRUE, &spStream_);
			if (SUCCEEDED(hr_))
			{
				hr_ = CoMarshalInterface(spStream_.Get(), __uuidof(IUnknown), stubInterface, MSHCTX_LOCAL, nullptr, MSHCTX_NOSHAREDMEM);
			}
		}
		else
		{
			hr_ = E_NOINTERFACE;
		}
	}

	AutoStubBias(const AutoStubBias&) = delete;

	~AutoStubBias()
	{
		if (SUCCEEDED(hr_))
		{
			LARGE_INTEGER zero;
			zero.QuadPart = 0;
			spStream_->Seek(zero, 0, nullptr);
			CoReleaseMarshalData(spStream_.Get());
		}
		spStream_.Reset();
	}

private:
	Microsoft::WRL::ComPtr<IStream> spStream_;
	HRESULT hr_;
};

class CThreadRefTaker
{
public:
	CThreadRefTaker()
		: _punkThreadRef(nullptr)
	{
		SHGetThreadRef(&_punkThreadRef);
	}

	CThreadRefTaker(const CThreadRefTaker&) = delete;

	~CThreadRefTaker()
	{
		if (_punkThreadRef)
		{
			_punkThreadRef->Release();
			_punkThreadRef = nullptr;
		}
	}

protected:
	IUnknown* _punkThreadRef;
};

namespace Windows::Internal
{
	enum class TaskApartment;
	enum class TaskOptions;
	interface IComPoolTask;
}

extern "C" inline HRESULT (WINAPI *SHTaskPoolQueueTask)(
	Windows::Internal::TaskApartment apartment,
	Windows::Internal::TaskOptions options,
	DWORD context,
	DWORD startDelayInMs,
	Windows::Internal::IComPoolTask* task,
	IUnknown** delayedTask
) = nullptr;

extern "C" inline DWORD (WINAPI *SHTaskPoolGetUniqueContext)() = nullptr;

extern "C" inline void (WINAPI *SHTaskPoolAllowThreadReuse)() = nullptr;

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

	namespace ComTaskPool
	{
		using namespace Microsoft::WRL;

		template<typename T>
		class CTaskWrapper final
			: public RuntimeClass<RuntimeClassFlags<ClassicCom>, IComPoolTask>
			, wistd::remove_reference_t<T>
		{
			using FunctorType = wistd::remove_reference_t<T>;

		public:
			CTaskWrapper(T&& task)
				: FunctorType(wistd::forward<T>(task))
			{
			}

			void STDMETHODCALLTYPE Run() override
			{
				FunctorType::operator()();
			}
		};

		template<typename T>
		HRESULT QueueTask(TaskApartment apartment, TaskOptions options, DWORD context, T&& task)
		{
			ComPtr<IComPoolTask> wrapper = Make<CTaskWrapper<T>>(wistd::forward<T>(task));
			return SHTaskPoolQueueTask(apartment, options, context, 0, wrapper.Get(), nullptr);
		}

		template<typename T>
		HRESULT QueueTask(TaskApartment apartment, TaskOptions options, T&& task)
		{
			return QueueTask(apartment, options, GetCurrentThreadId(), wistd::forward<T>(task));
		}

		template<typename T>
		HRESULT QueueTask(TaskApartment apartment, T&& task)
		{
			return QueueTask(apartment, TaskOptions::None, wistd::forward<T>(task));
		}
	}
}

namespace Windows::Internal
{
	MIDL_INTERFACE("19874d36-ba31-46b7-986b-121aa1bbcd62")
	IAsyncFireCompletion : IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE InvokeFireCompletion() PURE;
	};

	class ComTaskPoolHandler
	{
		static constexpr long c_lMaxThreadRecursionDepth = 4;

	public:
		ComTaskPoolHandler(TaskApartment apartment, TaskOptions options)
			: m_apartment(apartment)
			, m_options(options)
			, m_threadId(0)
		{
		}

		HRESULT Start(IComPoolTask* task)
		{
			m_threadId = GetCurrentThreadId();
			return SHTaskPoolQueueTask(m_apartment, m_options, m_threadId, 0, task, nullptr);
		}

		HRESULT FireCompletion(IAsyncFireCompletion* pFireCompletion)
		{
			return _FireCompletion(pFireCompletion);
		}

		static HRESULT _FireCompletion(IAsyncFireCompletion* pFireCompletion)
		{
			static long __declspec(thread) lThreadRecursionDepth;

			bool fAsyncCallback = false;
			if (lThreadRecursionDepth > 4)
			{
				Microsoft::WRL::ComPtr<IAsyncFireCompletion> spFireCompletion = pFireCompletion;
				fAsyncCallback = SUCCEEDED(ComTaskPool::QueueTask(TaskApartment::Any, [spFireCompletion]() -> void
				{
					spFireCompletion->InvokeFireCompletion();
				}));
			}

			HRESULT hr;
			if (fAsyncCallback)
			{
				hr = S_OK;
			}
			else
			{
				++lThreadRecursionDepth;
				hr = pFireCompletion->InvokeFireCompletion();
				--lThreadRecursionDepth;
			}

			return hr;
		}

		void Cancel();

		void OnComplete()
		{
			if (GetCurrentThreadId() != m_threadId)
			{
				SHTaskPoolAllowThreadReuse();
			}
		}

	private:
		TaskApartment m_apartment;
		TaskOptions m_options;
		DWORD m_threadId;
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
		IAsyncInfo* GetAsyncInfo() const
		{
			return m_asyncInfoWeak;
		}

		bool IsCanceled() const
		{
			AsyncStatus status;
			return SUCCEEDED(m_asyncInfoWeak->get_Status(&status)) && status == AsyncStatus::Canceled;
		}

		void SetAllowDismiss(bool fAllowDismiss)
		{
			m_allowDismiss = fAllowDismiss;
		}

		HRESULT MapErrorIfCanceled(HRESULT hr) const
		{
			return SUCCEEDED(hr) && IsCanceled() ? HRESULT_FROM_WIN32(ERROR_CANCELLED) : hr;
		}

		IAsyncDeferral* GetDeferral()
		{
			Microsoft::WRL::ComPtr<IAsyncDeferral> spDeferral;
			m_asyncInfoWeak->QueryInterface(IID_PPV_ARGS(&spDeferral));
			m_deferred = true;
			return spDeferral.Get();
		}

		template <typename TResult>
		AsyncDeferral<TResult> GetDeferral(TResult& result)
		{
			Microsoft::WRL::ComPtr<IAsyncDeferral> spDeferral;
			m_asyncInfoWeak->QueryInterface(IID_PPV_ARGS(&spDeferral));
			m_deferred = true;
			return AsyncDeferral<TResult>(result, spDeferral.Get());
		}

		CResultBase()
			: m_asyncInfoWeak(nullptr)
			, m_allowDismiss(true)
			, m_deferred(false)
		{
		}

		void SetAsyncAction(IAsyncInfo* pAsyncInfo)
		{
			m_asyncInfoWeak = pAsyncInfo;
		}

		bool AllowDismiss() const
		{
			return m_allowDismiss;
		}

		bool IsDeferred() const
		{
			return m_deferred;
		}

	private:
		IAsyncInfo* m_asyncInfoWeak;
		bool m_allowDismiss;
		bool m_deferred;
	};

	template <typename TResult>
	class CMarshaledInterfaceResult : public CResultBase
	{
	public:
		using ResultPtr = TResult*;

		static constexpr Microsoft::WRL::AsyncResultType c_callType = Microsoft::WRL::SingleResult;
		static constexpr bool c_fThreadAffineGetAndClose = false;

		static ResultPtr Initial()
		{
			return nullptr;
		}

		HRESULT Get(ResultPtr* ppResult)
		{
			*ppResult = 0;
			HRESULT hr = S_OK;
			if (_spLocal.IsInitialized())
			{
				hr = _spLocal.CopyLocal(ppResult);
			}
			else if (_spMarshal.HasStream())
			{
				hr = _spMarshal.Unmarshal(IID_PPV_ARGS(ppResult));
			}
			return hr;
		}

		void Set(const CMarshaledInterface& marshal)
		{
			_spMarshal = marshal;
		}

		HRESULT Set(ResultPtr pResult)
		{
			return _spLocal.Initialize(pResult);
		}

		void Close()
		{
			_spLocal.Revoke();
			_spMarshal.Release();
		}

	private:
		GitPtrSupportsAgile<TResult> _spLocal;
		CMarshaledInterface _spMarshal;
	};

	class CNoResult : public CResultBase
	{
	public:
		using ResultPtr = int;

		static constexpr Microsoft::WRL::AsyncResultType c_callType = Microsoft::WRL::SingleResult;
		static constexpr bool c_fThreadAffineGetAndClose = false;

		static ResultPtr Initial()
		{
			return 0;
		}

		HRESULT Get(ResultPtr* ppResult)
		{
			*ppResult = 0;
			return S_OK;
		}

		HRESULT Set(ResultPtr pResult)
		{
			return S_OK;
		}

		void Close()
		{
		}
	};

	enum class AsyncStage
	{
		Start = 0,
		Execute = 1,
		Cancel = 2,
	};

	template <typename TResult>
	class AsyncCallbackBase
	{
	public:
		virtual ~AsyncCallbackBase() = default;

		virtual HRESULT Run(AsyncStage stage, HRESULT hr, TResult& result) = 0;
	};

	template <
		typename TCall, // Windows::Internal::CCallAsyncStagedLambda<_lambda_6fd1045eef8a4720fd0f4574ed063cd5_ >
		typename TResult // Windows::Internal::CMarshaledInterfaceResult<IRequestCredentialsData>
	>
	class COperationLambda0 final : public AsyncCallbackBase<TResult>
	{
	public:
		COperationLambda0(const typename TCall::TLambdaT& call)
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
		using TLambdaT = TLambda;

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
		using TLambdaT = TLambda;

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

	MIDL_INTERFACE("2fafaaf9-2986-48ee-919d-98f66edf0a31")
	INilDelegate : IUnknown
	{
		virtual HRESULT STDMETHODCALLTYPE Invoke(IUnknown*, int) = 0;
	};

	template <typename T>
	class DECLSPEC_NOVTABLE AsyncProgress
	{
	public:
		virtual HRESULT SetProgress(const T) = 0;
	};

	inline RTL_RUN_ONCE s_bIsEnvironmentCheckDone;

	template <
		typename TComplete, // ABI::Windows::Foundation::IAsyncActionCompletedHandler
		Microsoft::WRL::AsyncResultType resultType, // Microsoft::WRL::SingleResult
		typename TAsyncBaseOptions // Microsoft::WRL::AsyncCausalityOptions<&AppointmentsManagerShowTimeFrameAsyncOperationName, &GUID_CAUSALITY_WINDOWS_PLATFORM_ID, 2>
	>
	class AsyncBaseFTM
		: public Microsoft::WRL::Implements<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
			, Microsoft::WRL::AsyncBase<TComplete, Microsoft::WRL::Details::Nil, resultType, TAsyncBaseOptions>
			, Microsoft::WRL::FtmBase
		>
	{
		typedef typename Microsoft::WRL::Details::ArgTraitsHelper<TComplete>::Traits CompleteTraits;
		typedef Microsoft::WRL::Details::AsyncOptionsHelper<TComplete, TAsyncBaseOptions> AllOptions;

	public:
		AsyncBaseFTM()
			: completedDelegateBucketAssist_(nullptr)
			, completedDelegateLockCount_(0)
		{
			InitOnceExecuteOnce(&s_bIsEnvironmentCheckDone, CheckExecutionEnvironment, nullptr, nullptr);
		}

		AsyncBaseFTM(const AsyncBaseFTM&) = delete;
		AsyncBaseFTM(AsyncBaseFTM&&) noexcept = delete;

		STDMETHODIMP PutOnComplete(TComplete* completeHandler) override // Microsoft::WRL::AsyncBase
		{
			HRESULT hr = this->CheckValidStateForDelegateCall();
			if (SUCCEEDED(hr))
			{
				if (InterlockedIncrement(&this->cCompleteDelegateAssigned_) == 1)
				{
					hr = completedDelegate_.Initialize(completeHandler);
					if (SUCCEEDED(hr))
					{
						this->TraceDelegateAssigned();

						if (completeHandler)
						{
							this->completedDelegateBucketAssist_ = Microsoft::WRL::Details::GetDelegateBucketAssist(completeHandler);
						}

						MemoryBarrier();

						if (this->IsTerminalState())
						{
							this->FireCompletion();
						}
					}
				}
				else
				{
					hr = E_ILLEGAL_DELEGATE_ASSIGNMENT;
				}
			}
			return hr;
		}

		STDMETHODIMP GetOnComplete(TComplete** completeHandler) override // Microsoft::WRL::AsyncBase
		{
			*completeHandler = 0;
			HRESULT hr = this->CheckValidStateForDelegateCall();
			if (SUCCEEDED(hr) && TryLockCompleteDelegate())
			{
				hr = completedDelegate_.CopyLocal(completeHandler);
				UnlockCompleteDelegate();
			}
			return hr;
		}

		HRESULT FireCompletion() override // Microsoft::WRL::AsyncBase
		{
			HRESULT hr = S_OK;
			this->TryTransitionToCompleted();

			if (completedDelegate_.IsInitialized() && InterlockedIncrement(&this->cCallbackMade_) == 1)
			{
				Microsoft::WRL::ComPtr<IAsyncInfo> asyncInfo = this;
				Microsoft::WRL::ComPtr<typename Microsoft::WRL::Details::DerefHelper<typename CompleteTraits::Arg1Type>::DerefType> operationInterface;

				this->TraceOperationComplete();

				if (SUCCEEDED(asyncInfo.As(&operationInterface)))
				{
					Microsoft::WRL::Details::AsyncStatusInternal current = Microsoft::WRL::Details::AsyncStatusInternal::_Undefined;
					this->CurrentStatus(&current);

					Microsoft::WRL::ComPtr<TComplete> completedDelegateLocal;
					if (SUCCEEDED(completedDelegate_.template CopyLocal<TComplete>(&completedDelegateLocal)))
					{
						this->TraceCompletionNotificationStart();

						Microsoft::WRL::ComPtr<IRpcOptions> spRpcOptions;
						RpcOptionsHelper::GetRpcOptions(completedDelegateLocal.Get(), &spRpcOptions);

						AutoStubBias b1(spRpcOptions.Get(), operationInterface.Get(), completedDelegateLocal.Get());
						hr = completedDelegateLocal->Invoke(operationInterface.Get(), static_cast<AsyncStatus>(current));
						hr = Microsoft::WRL::ErrorPropagationPolicyTraits<AllOptions::PropagationPolicy>::FireProgressErrorPropagationPolicyFilter(hr, completedDelegateLocal.Get(), completedDelegateBucketAssist_);
						UnlockCompleteDelegate();

						this->TraceCompletionNotificationComplete();
					}
				}
			}

			return hr;
		}

	protected:
		virtual bool v_ShouldSetNoWake()
		{
			return true;
		}

		virtual bool v_MustDoNoWake()
		{
			return false;
		}

	private:
		GitPtrSupportsAgile<TComplete> completedDelegate_;
		void* completedDelegateBucketAssist_;
		long completedDelegateLockCount_;

		static BOOL WINAPI CheckExecutionEnvironment(RTL_RUN_ONCE*, void*, void**)
		{
			Microsoft::WRL::ComPtr<IGlobalOptions> globalOptions;
			if (SUCCEEDED(CoCreateInstance(CLSID_GlobalOptions, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&globalOptions))))
			{
				ULONG_PTR value;
				globalOptions->Query(COMGLB_RO_SETTINGS, &value);
			}

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, GetCurrentProcessId());
			if (hProcess)
			{
				HANDLE hToken;
				if (OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
				{
					UINT uIsAppContainer = 0;
					DWORD cbToken;
					GetTokenInformation(hToken, TokenIsAppContainer, &uIsAppContainer, sizeof(uIsAppContainer), &cbToken);
					CloseHandle(hToken);
				}
				CloseHandle(hProcess);
			}

			return TRUE;
		}

		bool TryLockCompleteDelegate()
		{
			while (true)
			{
				long oldValue = completedDelegateLockCount_;
				if (oldValue <= 0)
					return false;
				if (InterlockedCompareExchange(&completedDelegateLockCount_, oldValue + 1, oldValue) == oldValue)
					return true;
			}
		}

		void UnlockCompleteDelegate()
		{
			if (InterlockedDecrement(&completedDelegateLockCount_) == 0)
			{
				MemoryBarrier();
				completedDelegate_.Revoke();
			}
		}
	};

	template <
		typename TComplete, // ABI::Windows::Foundation::IAsyncOperationCompletedHandler<bool>
		typename TProgressDelegate, // INilDelegate
		Microsoft::WRL::AsyncResultType resultType, // Microsoft::WRL::SingleResult
		typename TAsyncBaseOptions // TAsyncBaseOptions
	>
	class AsyncBaseWithProgressFTM
		: public AsyncBaseFTM<TComplete, resultType, TAsyncBaseOptions>
	{
		typedef typename Microsoft::WRL::Details::ArgTraitsHelper<TProgressDelegate>::Traits ProgressedTraits;
		typedef Microsoft::WRL::Details::AsyncOptionsHelper<TComplete, TAsyncBaseOptions> AllOptions;

	public:
		virtual HRESULT PutOnProgress(TProgressDelegate* progressHandler)
		{
			HRESULT hr = this->CheckValidStateForDelegateCall();
			if (SUCCEEDED(hr))
			{
				if (InterlockedIncrement(&cProgressDelegateAssigned_) == 1)
				{
					hr = progressedDelegate_.Initialize(progressHandler);

					if (progressHandler)
					{
						progressedDelegateBucketAssist_ = Microsoft::WRL::Details::GetDelegateBucketAssist(progressHandler);
						InterlockedIncrement(&progressDelegateLockCount_);
					}

					MemoryBarrier();
					this->TraceDelegateAssigned();
				}
				else
				{
					hr = E_ILLEGAL_DELEGATE_ASSIGNMENT;
				}
			}

			return hr;
		}

		virtual HRESULT GetOnProgress(TProgressDelegate** progressHandler)
		{
			*progressHandler = 0;
			HRESULT hr = this->CheckValidStateForDelegateCall();
			if (SUCCEEDED(hr) && TryLockProgressDelegate())
			{
				hr = progressedDelegate_.CopyLocal(progressHandler);
				UnlockProgressDelegate();
			}
			return hr;
		}

		HRESULT FireProgress(const int arg)
		{
			HRESULT hr = S_OK;
			Microsoft::WRL::ComPtr<IAsyncInfo> asyncInfo = this;
			Microsoft::WRL::ComPtr<typename Microsoft::WRL::Details::DerefHelper<typename ProgressedTraits::Arg1Type>::DerefType> operationInterface;
			if (TryLockProgressDelegate() && SUCCEEDED(asyncInfo.As(&operationInterface)))
			{
				Microsoft::WRL::ComPtr<TProgressDelegate> progressedDelegateLocal;
				if (SUCCEEDED(progressedDelegate_.template CopyLocal<TProgressDelegate>(&progressedDelegateLocal)))
				{
					this->TraceProgressNotificationStart();

					Microsoft::WRL::ComPtr<IRpcOptions> spRpcOptions;
					RpcOptionsHelper::GetRpcOptions(progressedDelegateLocal.Get(), &spRpcOptions);

					AutoStubBias b1(spRpcOptions.Get(), operationInterface.Get(), progressedDelegateLocal.Get());

					hr = progressedDelegateLocal->Invoke(operationInterface.Get(), arg);
					hr = Microsoft::WRL::ErrorPropagationPolicyTraits<AllOptions::PropagationPolicy>::FireProgressErrorPropagationPolicyFilter(hr, progressedDelegateLocal.Get(), progressedDelegateBucketAssist_);

					this->TraceProgressNotificationComplete();
				}

				UnlockProgressDelegate();
			}

			return hr;
		}

		HRESULT FireCompletion() override // Microsoft::WRL::AsyncBase
		{
			if (progressDelegateLockCount_ > 0)
			{
				UnlockProgressDelegate();
			}
			return AsyncBaseFTM<TComplete, resultType, TAsyncBaseOptions>::FireCompletion();
		}

	private:
		GitPtrSupportsAgile<TProgressDelegate> progressedDelegate_;
		void* progressedDelegateBucketAssist_ = nullptr;
		long progressDelegateLockCount_ = 0;
		long cProgressDelegateAssigned_ = 0;

		bool TryLockProgressDelegate()
		{
			while (true)
			{
				long oldValue = progressDelegateLockCount_;
				if (oldValue <= 0)
					return false;
				if (InterlockedCompareExchange(&progressDelegateLockCount_, oldValue + 1, oldValue) == oldValue)
					return true;
			}
		}

		void UnlockProgressDelegate()
		{
			if (InterlockedDecrement(&progressDelegateLockCount_) == 0)
			{
				MemoryBarrier();
				progressedDelegate_.Revoke();
			}
		}
	};

	MIDL_INTERFACE("7a900af8-b975-45f7-8c93-3ae17df5c5d0")
	IAsyncOperationLocal : IUnknown
	{
		virtual void STDMETHODCALLTYPE Dismiss() = 0;
	};

	template <
		typename TOperation, // ABI::Windows::Foundation::IAsyncAction,
		typename TComplete, // ABI::Windows::Foundation::IAsyncActionCompletedHandler,
		typename TResult, // CNoResult,
		typename TAsyncHandler, // ComTaskPoolHandler,
		typename TProgressDelegate, // INilDelegate,
		typename TAsyncBaseOptions // Microsoft::WRL::AsyncCausalityOptions<&AppointmentsManagerShowTimeFrameAsyncOperationName, &GUID_CAUSALITY_WINDOWS_PLATFORM_ID, 2>
	>
	class AsyncOperation final
		: public AsyncProgress<int>
		, public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRtClassicComMix>
			, AsyncBaseWithProgressFTM<TComplete, TProgressDelegate, TResult::c_callType, TAsyncBaseOptions>
			, TOperation
			, IAsyncOperationLocal
			, Microsoft::WRL::CloakedIid<IAsyncDeferral>
			, Microsoft::WRL::CloakedIid<IComPoolTask>
			, Microsoft::WRL::CloakedIid<IAsyncFireCompletion>
		>
	{
	public:
		AsyncOperation(auto asyncHandler, const WCHAR* const pszRuntimeName, TrustLevel trustLevel)
			: m_runtimeClassName(pszRuntimeName)
			, m_trustLevel(trustLevel)
			, m_callbackCancelCount(0)
			, m_callbackExecuteCount(0)
			, m_callbackRefs(2)
			, m_callback(nullptr)
			, m_completionsAllowed(0)
			, m_closeCount(0)
			, m_deferralsCompleted(2)
			, m_deferralResult(S_FALSE)
			, m_asyncHandler(asyncHandler)
		{
			m_resultRetriever.SetAsyncAction(this);
		}

		HRESULT AttachCallbackAndStart(AsyncCallbackBase<TResult>* callback)
		{
			m_callback = callback;
			HRESULT hr = S_OK;
			if constexpr (TResult::c_fThreadAffineGetAndClose)
			{
				Microsoft::WRL::ComPtr<TOperation> spOperation;
				hr = Microsoft::WRL::MakeAndInitialize<AsyncOperationStub<TOperation, TResult>>(&spOperation, &m_resultRetriever, (IUnknown*)(IAsyncInfo*)this);
				if (SUCCEEDED(hr))
				{
					hr = m_marshalResultRetriever.Initialize(AGILEREFERENCE_DEFAULT, __uuidof(TOperation), spOperation.Get());
				}
			}
			if (SUCCEEDED(hr))
			{
				hr = this->Start();
			}
			return hr;
		}

		STDMETHODIMP_(ULONG) Release() override // IUnknown
		{
			if constexpr (TResult::c_fThreadAffineGetAndClose)
			{
				if (m_marshalResultRetriever.m_agileRef.Get())
				{
					this->RuntimeClassT::AddRef();
					if (this->RuntimeClassT::Release() == 2)
					{
						m_marshalResultRetriever.Revoke();
					}
				}
			}
			return this->RuntimeClassT::Release();
		}

		HRESULT OnStart() override // Microsoft::WRL::AsyncBase
		{
			HRESULT hr = m_callback->Run(AsyncStage::Start, S_OK, m_resultRetriever);
			if (SUCCEEDED(hr))
			{
				hr = m_asyncHandler.Start(this);
				if (FAILED(hr))
				{
					if (m_callbackExecuteCount)
					{
						this->TryTransitionToError(hr);
						hr = S_OK;
					}
					else
					{
						_Run(AsyncStage::Execute, hr);
					}
				}
			}
			if (FAILED(hr))
			{
				this->TryTransitionToError(hr);
			}
			return hr;
		}

		void OnClose() override // Microsoft::WRL::AsyncBase
		{
			if (InterlockedIncrement(&m_closeCount) == 1)
			{
				if constexpr (TResult::c_fThreadAffineGetAndClose)
				{
					Microsoft::WRL::ComPtr<TOperation> spThreadAffineWrapper;
					if (SUCCEEDED(m_marshalResultRetriever.CopyLocal(&spThreadAffineWrapper)))
					{
						spThreadAffineWrapper->put_Completed(nullptr);
					}
				}
				else
				{
					m_resultRetriever.Close();
				}
			}
		}

		void OnCancel() override // Microsoft::WRL::AsyncBase
		{
			_Run(AsyncStage::Cancel, HRESULT_FROM_WIN32(ERROR_CANCELLED));
		}

			HRESULT FireCompletion() override // Microsoft::WRL::AsyncBase
		{
			HRESULT hr = S_OK;
			if (InterlockedIncrement(&m_completionsAllowed) == 2)
			{
				hr = m_asyncHandler._FireCompletion(this);
			}
			return hr;
		}

		bool v_MustDoNoWake() override // Windows::Internal::AsyncBaseFTM
		{
			return true;
		}

		STDMETHODIMP_(void) Run() override // Windows::Internal::IComPoolTask
		{
			HRESULT hr = S_OK;
			if (!this->ContinueAsyncOperation())
			{
				this->ErrorCode(&hr);
				if (SUCCEEDED(hr))
				{
					hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
				}
			}
			_Run(AsyncStage::Execute, hr);
		}

		STDMETHODIMP InvokeFireCompletion() override // Windows::Internal::IAsyncFireCompletion
		{
			return FireCompletion();
		}

		HRESULT SetProgress(const int arg) override // Windows::Internal::AsyncProgress<int>
		{
			this->FireProgress(arg);
			return S_OK;
		}

		STDMETHODIMP put_Completed(TComplete* pEventHandler) override // TOperation
		{
			Microsoft::WRL::ComPtr<IUnknown> spunk = (IUnknown*)(IAsyncInfo*)this;
			return this->AsyncBaseFTM<TComplete, Microsoft::WRL::AsyncResultType::SingleResult, TAsyncBaseOptions>::PutOnComplete(pEventHandler);
		}

		STDMETHODIMP get_Completed(TComplete** ppEventHandler) override // TOperation
		{
			return this->AsyncBaseFTM<TComplete, Microsoft::WRL::AsyncResultType::SingleResult, TAsyncBaseOptions>::GetOnComplete(ppEventHandler);
		}

		STDMETHODIMP put_Progress(TProgressDelegate*) // Possible override from Windows::Foundation::IAsyncActionWithProgress<...>
		{
			static_assert(false); // Function body unknown
			return E_NOTIMPL;
		}

		STDMETHODIMP get_Progress(TProgressDelegate**) // Possible override from Windows::Foundation::IAsyncActionWithProgress<...>
		{
			static_assert(false); // Function body unknown
			return E_NOTIMPL;
		}

		STDMETHODIMP GetResults() // Possible override from Windows::Foundation::IAsyncAction or Windows::Foundation::IAsyncActionWithProgress<...>
		{
			HRESULT hr = this->CheckValidStateForResultsCall();
			if (SUCCEEDED(hr))
			{
				this->ErrorCode(&hr);
			}
			return hr;
		}

		STDMETHODIMP GetResults(typename TResult::ResultPtr* pResult) // Possible override from Windows::Foundation::IAsyncOperation<TResult::ResultPtr>
		{
			*pResult = TResult::Initial();
			HRESULT hr = this->CheckValidStateForResultsCall();
			if (SUCCEEDED(hr))
			{
				if constexpr (TResult::c_fThreadAffineGetAndClose)
				{
					Microsoft::WRL::ComPtr<TOperation> spThreadAffineWrapper;
					if (SUCCEEDED(m_marshalResultRetriever.CopyLocal(&spThreadAffineWrapper)))
					{
						hr = spThreadAffineWrapper->GetResults(pResult);
					}
				}
				else
				{
					hr = m_resultRetriever.Get(pResult);
				}
			}
			return hr;
		}

		STDMETHODIMP_(void) Dismiss() override // Windows::Internal::IAsyncOperationLocal
		{
			if (m_resultRetriever.AllowDismiss())
			{
				OnCancel();
			}
		}

		STDMETHODIMP_(void) Complete(HRESULT hr) override // Windows::Internal::IAsyncDeferral
		{
			if (InterlockedCompareExchange(&m_deferralResult, hr, S_FALSE) == S_FALSE
				&& InterlockedDecrement(&m_deferralsCompleted) == 0)
			{
				_AfterExecute(m_deferralResult);
			}
		}

		STDMETHODIMP GetRuntimeClassName(HSTRING* phstrRuntimeName) override // IInspectable
		{
			*phstrRuntimeName = nullptr;
			HRESULT hr = S_OK;
			if (m_runtimeClassName)
			{
				hr = WindowsCreateString(m_runtimeClassName, (UINT32)wcslen(m_runtimeClassName), phstrRuntimeName);
			}
			return hr;
		}

		STDMETHODIMP GetTrustLevel(TrustLevel* trustLvl) override // IInspectable
		{
			*trustLvl = m_trustLevel;
			return S_OK;
		}

	private:
		~AsyncOperation() override
		{
			if (m_callback)
				delete m_callback;
		}

		void _AfterComplete()
		{
			if (m_callback)
				delete m_callback;
			m_callback = nullptr;
			m_asyncHandler.OnComplete();
			InterlockedIncrement(&m_completionsAllowed);
			AsyncBaseWithProgressFTM<TComplete, TProgressDelegate, TResult::c_callType, TAsyncBaseOptions>::FireCompletion();
		}

		void _AfterExecute(HRESULT hr)
		{
			bool b = InterlockedDecrement(&m_callbackRefs) == 0;
			if (InterlockedIncrement(&m_callbackCancelCount) == 1)
			{
				b = InterlockedDecrement(&m_callbackRefs) == 0;
			}

			if (SUCCEEDED(hr))
			{
				this->TryTransitionToCompleted(Microsoft::WRL::CancelTransitionPolicy::TransitionFromCanceled);
			}
			else
			{
				Microsoft::WRL::Details::AsyncStatusInternal state = Microsoft::WRL::Details::AsyncStatusInternal::_Undefined;
				this->CurrentStatus(&state);
				if (state != Microsoft::WRL::Details::AsyncStatusInternal::_Canceled)
				{
					this->TryTransitionToError(hr);
				}
			}

			if (b)
			{
				_AfterComplete();
			}
		}

		void _Run(AsyncStage stage, HRESULT hr)
		{
			if (stage == AsyncStage::Cancel)
			{
				if (InterlockedIncrement(&m_callbackCancelCount) == 1)
				{
					m_callback->Run(AsyncStage::Cancel, hr, m_resultRetriever);

					bool b = InterlockedDecrement(&m_callbackRefs) == 0;
					if (InterlockedIncrement(&m_callbackExecuteCount) == 1)
					{
						m_callback->Run(AsyncStage::Execute, HRESULT_FROM_WIN32(ERROR_CANCELLED), m_resultRetriever);
						b = InterlockedDecrement(&m_callbackRefs) == 0;
					}

					if (b)
					{
						_AfterComplete();
					}
				}
			}
			else if (stage == AsyncStage::Execute)
			{
				if (InterlockedIncrement(&m_callbackExecuteCount) == 1)
				{
					hr = m_callback->Run(AsyncStage::Execute, hr, m_resultRetriever);
					if (SUCCEEDED(hr) && m_resultRetriever.IsDeferred())
					{
						if (InterlockedDecrement(&m_deferralsCompleted) == 0)
						{
							_AfterExecute(m_deferralResult);
						}
					}
					else
					{
						_AfterExecute(hr);
					}
				}
			}
		}

		const WCHAR* const m_runtimeClassName;
		TrustLevel m_trustLevel;
		volatile long m_callbackCancelCount;
		volatile long m_callbackExecuteCount;
		volatile long m_callbackRefs;
		AsyncCallbackBase<TResult>* m_callback;
		volatile long m_completionsAllowed;
		volatile long m_closeCount;
		volatile long m_deferralsCompleted;
		HRESULT m_deferralResult;
		TResult m_resultRetriever;
		AgileGitPtr m_marshalResultRetriever;
		TAsyncHandler m_asyncHandler;

		template <typename TOperationI, typename TResultI>
		class AsyncOperationStub
			: public CThreadRefTaker
			, public Microsoft::WRL::RuntimeClass<TOperationI>
		{
		public:
			HRESULT RuntimeClassInitialize(TResultI* resultRetrieverWeak, IUnknown* asyncOperationLifetime)
			{
				m_resultRetrieverWeak = resultRetrieverWeak;
				m_asyncOperationLifetime = asyncOperationLifetime;
				return S_OK;
			}

			STDMETHODIMP GetRuntimeClassName(HSTRING* runtimeClassName) override // IInspectable
			{
				*runtimeClassName = nullptr;
				RoOriginateError(E_NOTIMPL, nullptr);
				return E_NOTIMPL;
			}

			STDMETHODIMP GetTrustLevel(TrustLevel*) override // IInspectable
			{
				RoOriginateError(E_NOTIMPL, nullptr);
				return E_NOTIMPL;
			}

			STDMETHODIMP put_Completed(TComplete*) override // TOperation
			{
				m_resultRetrieverWeak->Close();
				m_resultRetrieverWeak = nullptr;
				m_asyncOperationLifetime.Reset();
				return S_OK;
			}

			STDMETHODIMP get_Completed(TComplete**) override // TOperation
			{
				return E_NOTIMPL;
			}

			STDMETHODIMP put_Progress(INilDelegate*) // Possible override from Windows::Foundation::IAsyncActionWithProgress<...>
			{
				static_assert(false); // Function body unknown
				return E_NOTIMPL;
			}

			STDMETHODIMP get_Progress(INilDelegate**) // Possible override from Windows::Foundation::IAsyncActionWithProgress<...>
			{
				static_assert(false); // Function body unknown
				return E_NOTIMPL;
			}

			STDMETHODIMP GetResults() // Possible override from Windows::Foundation::IAsyncAction or Windows::Foundation::IAsyncActionWithProgress<...>
			{
				static_assert(false); // Function body unknown
				return E_NOTIMPL;
			}

			STDMETHODIMP GetResults(typename TResultI::ResultPtr* pResult) // Possible override from Windows::Foundation::IAsyncOperation<TResult::ResultPtr>
			{
				return m_resultRetrieverWeak ? m_resultRetrieverWeak->Get(pResult) : E_ILLEGAL_METHOD_CALL;
			}

		private:
			~AsyncOperationStub() override
			{
				if (m_resultRetrieverWeak)
				{
					m_resultRetrieverWeak->Close();
				}
			}

			Microsoft::WRL::ComPtr<IUnknown> m_asyncOperationLifetime;
			TResultI* m_resultRetrieverWeak;
		};
	};

	template <
		typename TOperation, // IAsyncOperation<RequestCredentialsData*>
		typename TComplete, // IAsyncOperationCompletedHandler<RequestCredentialsData*>
		typename TProgressDelegate, // INilDelegate
		typename TResult, // CMarshaledInterfaceResult<Windows::Internal::UI::Logon::Controller::IRequestCredentialsData>
		typename TAsyncHandler, // ComTaskPoolHandler&
		typename TAsyncBaseOptions // AsyncOptions<PropagateErrorWithWin8Quirk, nullptr, GUID_CAUSALITY_WINDOWS_PLATFORM_ID, Diagnostics::CausalitySource_System>
	>
	HRESULT MakeAsyncHelper(
		TOperation** ppOperation,
		TAsyncHandler handler,
		const WCHAR* const pszRuntimeName,
		TrustLevel trustLevel,
		AsyncCallbackBase<TResult>* pCallback)
	{
		*ppOperation = nullptr;

		HRESULT hr = pCallback ? S_OK : E_OUTOFMEMORY;
		if (SUCCEEDED(hr))
		{
			auto spOperation = Microsoft::WRL::Make<AsyncOperation<TOperation, TComplete, TResult, TAsyncHandler, TProgressDelegate, TAsyncBaseOptions>>(handler, pszRuntimeName, trustLevel);
			hr = spOperation.Get() ? S_OK : E_OUTOFMEMORY;
			if (SUCCEEDED(hr))
			{
				hr = spOperation->AttachCallbackAndStart(pCallback);
				pCallback = nullptr;
				if (SUCCEEDED(hr))
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
