#pragma once

#include <windows.foundation.h>

#include <wrl/implements.h>

DWORD SHProcessMessagesUntilEventsEx(HWND hwnd, HANDLE* pHandles, DWORD cHandles, DWORD dwTimeout, DWORD dwWakeMask, DWORD dwFlags);

template <typename THandler, typename TOperation>
HRESULT WaitForCompletion(TOperation* pOperation, COWAIT_FLAGS flags = (COWAIT_FLAGS)-1, HANDLE hEventCancelled = nullptr)
{
    class FTMEventDelegate : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>
        , THandler
        , Microsoft::WRL::FtmBase
    >
    {
    public:
        FTMEventDelegate()
            : _status(AsyncStatus::Started)
            , _hEventCompleted(nullptr)
        {
        }

        // ReSharper disable once CppHidingFunction
        HRESULT RuntimeClassInitialize()
        {
            _hEventCompleted = CreateEventExW(nullptr, nullptr, FALSE, EVENT_ALL_ACCESS);
            return _hEventCompleted ? S_OK : HRESULT_FROM_WIN32(GetLastError());
        }

        STDMETHODIMP Invoke(TOperation*, AsyncStatus status) override
        {
            _status = status;
            SetEvent(_hEventCompleted);
            return S_OK;
        }

        HANDLE GetEvent() const { return _hEventCompleted; }
        AsyncStatus GetStatus() const { return _status; }

    private:
        ~FTMEventDelegate() override
        {
            CloseHandle(_hEventCompleted);
        }

        AsyncStatus _status;
        HANDLE _hEventCompleted;
    };

    Microsoft::WRL::ComPtr<TOperation> spOperation;
    Microsoft::WRL::ComPtr<FTMEventDelegate> spCallback;
    HRESULT hr = Microsoft::WRL::MakeAndInitialize<FTMEventDelegate>(&spCallback);
    if (SUCCEEDED(hr))
    {
        hr = pOperation->put_Completed(spCallback.Get());
        if (SUCCEEDED(hr))
        {
            HANDLE rgHandles[] = { spCallback->GetEvent(), nullptr };
            bool fCancelOp = false;
            if (flags == (COWAIT_FLAGS)-1 && SHProcessMessagesUntilEventsEx(nullptr, rgHandles, 1, INFINITE, QS_ALLINPUT, 0) == WAIT_FAILED)
            {
                hr = E_FAIL;
            }
            else if (SUCCEEDED(hr) && flags != (COWAIT_FLAGS)-1)
            {
                DWORD dwHandleIndex;
                hr = CoWaitForMultipleHandles(flags, INFINITE, 1, rgHandles, &dwHandleIndex);
                if (SUCCEEDED(hr) && dwHandleIndex)
                {
                    hr = HRESULT_FROM_WIN32(ERROR_CANCELLED);
                    fCancelOp = true;
                }
            }

            Microsoft::WRL::ComPtr<IAsyncInfo> spAsyncInfo;
            if (fCancelOp && SUCCEEDED(pOperation->QueryInterface(IID_PPV_ARGS(&spAsyncInfo))))
            {
                spAsyncInfo->Cancel();
            }

            if (SUCCEEDED(hr) && spCallback->GetStatus() != AsyncStatus::Completed)
            {
                if (spAsyncInfo.Get() || SUCCEEDED(pOperation->QueryInterface(IID_PPV_ARGS(&spAsyncInfo))))
                {
                    spAsyncInfo->get_ErrorCode(&hr);
                }
            }
        }
    }

    return hr;
}

template <typename TOperationResult>
HRESULT WaitForCompletion(ABI::Windows::Foundation::IAsyncOperation<TOperationResult>* pOperation)
{
    return WaitForCompletion<ABI::Windows::Foundation::IAsyncOperationCompletedHandler<TOperationResult>>(pOperation);
}
