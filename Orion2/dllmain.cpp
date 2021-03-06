/**
* Orion2 - A MapleStory2 Dynamic Link Library Localhost
*
* @author Eric
*
*/
#include "OrionHacks.h"
#include "WinSockHook.h"
#include "NMCOHook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	/* Initial injection from process attach. */
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		/* Update the locale to properly decode foreign StringPools. */
		setlocale(LC_ALL, STRING_LOCALE);

		/* Redirect the client process if necessary. */
		if (!RedirectProcess()) {
			NotifyMessage("Failed to redirect process", Orion::NotifyType::Error);
			return FALSE;
		}

#if DEBUG_MODE
		//InitUnhandledExceptionFilter();		
		InitVectoredExceptionHandler();

		AllocConsole();
		SetConsoleTitleA(CLIENT_NAME);
		AttachConsole(GetCurrentProcessId());
		freopen("CON", "w", stdout);
#endif

		/* Hook CreateMutexA and enable the use of Multi-Client for MS2. */
		if (!Hook_CreateMutexA(true)) {
			NotifyMessage("Failed to hook CreateMutexA", Orion::NotifyType::Error);
			return FALSE;
		}

		/* Initiate the winsock hook for socket spoofing and redirection. */
		if (!Hook_WSPStartup(true)) {
			NotifyMessage("Failed to hook WSPStartup", Orion::NotifyType::Error);
			return FALSE;
		}

		/* Initiate the NMCO hook to fix our login passport handling. */
		if (!NMCOHook_Init()) {
			NotifyMessage("Failed to hook CallNMFunc", Orion::NotifyType::Error);
			return FALSE;
		}

		/* Initiate the CreateWindowExA hook to customize the main window. */
		if (!Hook_CreateWindowExA(true)) {
			NotifyMessage("Failed to hook CreateWindowExA", Orion::NotifyType::Error);
			return FALSE;
		}

		Log("Initializing the Orion2 Dynamic Link Library Localhost...\r\nRuntime: %s", GetCommandLine());
	} else if (dwReason == DLL_PROCESS_DETACH) {
#if DEBUG_MODE
		FreeConsole();
#endif
	}

	return TRUE;
}
