#include "pch.h"

#include "consolelockaction.h"

using namespace Microsoft::WRL;

using namespace ABI::Windows::Foundation;
using namespace Windows::Internal::UI::Logon::Controller;
using namespace Windows::Internal::UI::Logon::CredProvData;

extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLockScreen[] = L"Windows.Internal.UI.Logon.Controller.ConsoleLockScreen";

class ConsoleLock final
	: public RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>
		, ILockScreenHost
		, FtmBase
	>
{
	InspectableClass(RuntimeClass_Windows_Internal_UI_Logon_Controller_ConsoleLockScreen, PartialTrust);

public:
	ConsoleLock();
	~ConsoleLock() override;

	STDMETHODIMP LockAsync(LockOptions options, HSTRING domainName, HSTRING userName, HSTRING friendlyName, HSTRING unk, bool* setWin32kForegroundHardening, IUnlockTrigger** ppAction) override;
	STDMETHODIMP Reset() override;
	STDMETHODIMP PreShutdown() override;
};

ConsoleLock::ConsoleLock()
{
}

ConsoleLock::~ConsoleLock()
{
}

HRESULT ConsoleLock::LockAsync(LockOptions options, HSTRING domainName, HSTRING userName, HSTRING friendlyName, HSTRING unk,
	bool* setWin32kForegroundHardening, IUnlockTrigger** ppAction)
{
	*ppAction = nullptr;
	*setWin32kForegroundHardening = false;
	RETURN_HR_IF(E_NOTIMPL, (options & LockOptions_SecureDesktop) == 0);

	RETURN_IF_FAILED(MakeAndInitialize<ConsoleLockAction>(ppAction,domainName,userName,friendlyName)); // 36

	return S_OK;
}

HRESULT ConsoleLock::Reset()
{
	return S_OK;
}

HRESULT ConsoleLock::PreShutdown()
{
	return S_OK;
}

ActivatableClass(ConsoleLock);
