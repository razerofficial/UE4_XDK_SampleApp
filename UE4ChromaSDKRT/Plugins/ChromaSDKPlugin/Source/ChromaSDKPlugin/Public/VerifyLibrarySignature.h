#pragma once

#if PLATFORM_WINDOWS

#include "Windows/AllowWindowsPlatformTypes.h" 

#include <tchar.h>
#include <Windows.h>

typedef _TCHAR *PTCHAR;

namespace ChromaSDK
{
	class VerifyLibrarySignature
	{
	public:
		static BOOL VerifyModule(HMODULE hModule);
	private:
		static BOOL IsValidPath(PTCHAR szFileName);
		static BOOL IsFileSignedByRazer(PTCHAR szFileName);
		static BOOL IsFileSigned(PTCHAR szFileName);
	};
}

#include "Windows/HideWindowsPlatformTypes.h"

#endif
