
#include "stdafx.h"
#include <afxdllx.h>

static AFX_EXTENSION_MODULE fish_serverDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
  UNREFERENCED_PARAMETER(lpReserved);

  if (dwReason == DLL_PROCESS_ATTACH) {
	  if (!AfxInitExtensionModule(fish_serverDLL, hInstance))
		  return 0;

	  new CDynLinkLibrary(fish_serverDLL);
  }
  else if (dwReason == DLL_PROCESS_DETACH) {
	  AfxTermExtensionModule(fish_serverDLL);
  }
  return 1;
}
