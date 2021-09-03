#include <clocale>
#include "Hooks.h"

#define SIGNATURE 0xEAC8FC70
#define SIGNATURE_MASK 0x0000000F
//#define SIGNATURE_ENABLE // Uncomment this to enable signature verification

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

static DWORD WINAPI unload(HMODULE moduleHandle) noexcept
{
	Sleep(100);
	_CRT_INIT(moduleHandle, DLL_PROCESS_DETACH, nullptr);
	FreeLibraryAndExitThread(moduleHandle, 0);
}

BOOL APIENTRY DllEntryPoint(HMODULE moduleHandle, DWORD reason, LPVOID reserved)
{
	if (!_CRT_INIT(moduleHandle, reason & SIGNATURE_MASK, reserved))
		return FALSE;

	if ((reason & SIGNATURE_MASK) == DLL_PROCESS_ATTACH)
	{
		#if defined SIGNATURE_ENABLE && !defined NEPS_DEBUG
		if ((reason & ~SIGNATURE_MASK) == SIGNATURE)
		#endif // NEPS_DEBUG
		{
			std::setlocale(LC_CTYPE, ".utf8");
			hooks = std::make_unique<Hooks>(moduleHandle);
			return TRUE;
		}

		MessageBoxA(nullptr, "Oh no! Somehow DLL got injected outside of loader. I'm sorry about that.", "NEPS.PP", MB_OK | MB_ICONERROR);
		if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), moduleHandle, 0, nullptr))
			CloseHandle(thread);
	}

    return FALSE;
}
