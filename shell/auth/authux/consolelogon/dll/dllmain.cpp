#include "pch.h"

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID __formal)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hInstance);
	return TRUE;
}