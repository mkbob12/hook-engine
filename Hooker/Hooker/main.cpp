#include "pch.h"
VOID(WINAPI* True_Sleep)(DWORD dwMilliseconds) = Sleep;
VOID(WINAPI* True_GetLocalTime)(_Out_ LPSYSTEMTIME lpSystemTime) = GetLocalTime;
VOID WINAPI Hooked_Sleep(DWORD dwMilliseconds)
{
	printf("Let's say sleep %u\n", dwMilliseconds);
}
VOID WINAPI Hooked_GetLocalTime(_Out_ LPSYSTEMTIME lpSystemTime)
{
	memset(lpSystemTime, 0, sizeof(SYSTEMTIME));
}
__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	if (DetourIsHelperProcess()) {
		return TRUE;
	}
	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)True_Sleep, Hooked_Sleep);
		DetourAttach(&(PVOID&)True_GetLocalTime, Hooked_GetLocalTime);
		DetourTransactionCommit();
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)True_Sleep, Hooked_Sleep);
		DetourDetach(&(PVOID&)True_GetLocalTime, Hooked_GetLocalTime);
		DetourTransactionCommit();
	}
	return TRUE;
}