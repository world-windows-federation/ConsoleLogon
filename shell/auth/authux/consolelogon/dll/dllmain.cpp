#include "pch.h"

#include <cinttypes>

using namespace Microsoft::WRL;

#pragma region "Pattern search stuff"

inline void SectionBeginAndSize(HMODULE hhh, const char* pszSectionName, PBYTE* beginSection, DWORD* sizeSection)
{
	*beginSection = nullptr;
	*sizeSection = 0;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hhh;
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS64)((BYTE*)dosHeader + dosHeader->e_lfanew);
		if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
		{
			PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(ntHeader);
			for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
			{
				PIMAGE_SECTION_HEADER section = firstSection + i;
				if (!strcmp((const char*)section->Name, pszSectionName))
				{
					*beginSection = (PBYTE)dosHeader + section->VirtualAddress;
					*sizeSection = section->SizeOfRawData;
					break;
				}
			}
		}
	}
}

inline void TextSectionBeginAndSize(HMODULE hhh, PBYTE* beginSection, DWORD* sizeSection)
{
	SectionBeginAndSize(hhh, ".text", beginSection, sizeSection);
}

inline BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
{
	for (PBYTE value = (PBYTE)pBuffer; *lpMask; ++lpPattern, ++lpMask, ++value)
	{
		if (*lpMask == 'x' && *(LPCBYTE)lpPattern != *value)
			return FALSE;
	}

	return TRUE;
}

inline __declspec(noinline) PVOID FindPatternHelper(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask)
{
	for (SIZE_T index = 0; index < dwSize; ++index)
	{
		PBYTE pAddress = (PBYTE)pBase + index;

		if (MaskCompare(pAddress, lpPattern, lpMask))
			return pAddress;
	}

	return nullptr;
}

inline PVOID FindPattern(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask)
{
	dwSize -= strlen(lpMask);
	return FindPatternHelper(pBase, dwSize, lpPattern, lpMask);
}

#pragma endregion "Pattern search stuff"

namespace Windows::Internal::ComTaskPool
{
	volatile DWORD s_dwUniqueCallingContext = 0x80000000;
	volatile DWORD* p_s_dwThreadIdReuse; // Used in the actual code
}

DWORD WINAPI CUSTOM_SHTaskPoolGetUniqueContext()
{
	return InterlockedIncrement(&Windows::Internal::ComTaskPool::s_dwUniqueCallingContext);
}

void WINAPI CUSTOM_SHTaskPoolAllowThreadReuse()
{
	if (Windows::Internal::ComTaskPool::p_s_dwThreadIdReuse)
		*Windows::Internal::ComTaskPool::p_s_dwThreadIdReuse = GetCurrentThreadId();
}

void TaskPool_InitFunctions()
{
	HMODULE hShcore = LoadLibraryW(L"shcore.dll");
	SHTaskPoolQueueTask = (decltype(SHTaskPoolQueueTask))GetProcAddress(hShcore, "SHTaskPoolQueueTask");
	SHTaskPoolGetUniqueContext = (decltype(SHTaskPoolGetUniqueContext))GetProcAddress(hShcore, "SHTaskPoolGetUniqueContext");
	SHTaskPoolAllowThreadReuse = (decltype(SHTaskPoolAllowThreadReuse))GetProcAddress(hShcore, "SHTaskPoolAllowThreadReuse");

	PBYTE textBegin;
	DWORD textSize;
	TextSectionBeginAndSize(hShcore, &textBegin, &textSize);

	if (!SHTaskPoolQueueTask)
	{
		// HRESULT Windows::Internal::ComTaskPool::RunTask(
		//     Windows::Internal::TaskApartment apartment,
		//     Windows::Internal::TaskOptions options,
		//     DWORD dwCallingContext,
		//     DWORD dwMillisecondsFromNow,
		//     Windows::Internal::IComPoolTask* pTask,
		//     IUnknown** ppDelayedTask)
		// 44 8B C0 8B 0F 48 89 5C 24 ?? E8 ?? ?? ?? ??
		//                                  ^^^^^^^^^^^
		// Ref: Windows::Internal::ComTaskPoolHandler::Start()
		// Arguments dwMillisecondsFromNow and ppDelayedTask are optimized out; always 0 and nullptr respectively
#if defined(_M_X64)
		PBYTE matchRunTask = (PBYTE)FindPattern(
			textBegin,
			textSize,
			"\x44\x8B\xC0\x8B\x0F\x48\x89\x5C\x24\x00\xE8",
			"xxxxxxxxx?x"
		);
		if (matchRunTask)
		{
			matchRunTask += 10;
			matchRunTask += 5 + *(int*)(matchRunTask + 1);
		}
#else
		PBYTE matchRunTask = nullptr;
#endif
		if (matchRunTask)
		{
			SHTaskPoolQueueTask = (decltype(SHTaskPoolQueueTask))matchRunTask;
			printf("Windows::Internal::ComTaskPool::RunTask() = shcore.dll+%" PRIXPTR "\n", (PBYTE)SHTaskPoolQueueTask - (PBYTE)hShcore);
		}
		FAIL_FAST_IF_NULL(SHTaskPoolQueueTask);
	}

	if (!SHTaskPoolGetUniqueContext)
	{
		SHTaskPoolGetUniqueContext = CUSTOM_SHTaskPoolGetUniqueContext;
	}
}

#if !defined(__WRL_CLASSIC_COM__)
STDAPI DllGetActivationFactory(_In_ HSTRING activatibleClassId, _COM_Outptr_ IActivationFactory** factory)
{
	return Module<InProc>::GetModule().GetActivationFactory(activatibleClassId, factory);
}
#endif

#if !defined(__WRL_WINRT_STRICT__)
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, _COM_Outptr_ void** ppv)
{
	return Module<InProc>::GetModule().GetClassObject(rclsid, riid, ppv);
}
#endif

STDAPI DllCanUnloadNow()
{
	return Module<InProc>::GetModule().Terminate() ? S_OK : S_FALSE;
}

STDAPI_(BOOL) DllMain(_In_opt_ HINSTANCE hInstance, DWORD dwReason, _In_opt_ void* __formal)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
	}
	return TRUE;
}
