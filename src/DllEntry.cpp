#include "WarpTexture.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

extern ClassDesc2* GetWarpTextureClassDesc();

HINSTANCE hInstance = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
#if (MAX_RELEASE >= 9000)
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}
#else
	hInstance = hinstDLL;				// Hang on to this DLL's instance handle.

	static BOOL controlsInit = FALSE;
	if (!controlsInit) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);	// Initialize MAX's custom controls
		InitCommonControls();			// Initialize Win95 controls
	}
#endif

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i)
	{
		case 0: return GetWarpTextureClassDesc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}
