
#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE fish_androidDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		if (!AfxInitExtensionModule(fish_androidDLL, hInstance)) return 0;

		new CDynLinkLibrary(fish_androidDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		AfxTermExtensionModule(fish_androidDLL);
	}
	return 1;
}
