/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2016 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */
#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Windows /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// To build: cl.exe pcsc.c winscard.lib

#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winscard.h>

typedef LONG (WINAPI *t_SCardEstablishContext)(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext);
typedef LONG (WINAPI *t_SCardReleaseContext)(SCARDCONTEXT hContext);
typedef LONG (WINAPI * t_SCardListReaders)(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders);

int main()
{
	HMODULE mod = LoadLibrary("C:\\WINDOWS\\System32\\winscard.dll");
	if (mod == NULL)
		printf("LoadLibrary(winscard.dll) failed: %d\n", GetLastError());
	else
	{
		t_SCardEstablishContext pEstablischContext = (t_SCardEstablishContext) GetProcAddress(mod, "SCardEstablishContext");
		t_SCardReleaseContext pReleaseContext = (t_SCardReleaseContext) GetProcAddress(mod, "SCardReleaseContext");
		t_SCardListReaders pListReaders = (t_SCardListReaders) GetProcAddress(mod, "SCardListReadersA");

		if (!(pEstablischContext && pReleaseContext && pListReaders))
			printf("Couldn't find one or more proc addresses\n");
		else
		{
			SCARDCONTEXT hCtx;
			long ret = pEstablischContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);
			if (ret != 0)
				printf("pEstablischContext(): 0x%0x (%d)\n", ret, ret);
			else
			{
				char rdrs[1000];
				DWORD readersLen = sizeof(rdrs);
				rdrs[0] = '\0';
				ret = pListReaders(hCtx, NULL, rdrs, &readersLen);
				if (ret != 0)
					printf("pListReaders(): 0x%0x (%d)\n", ret, ret);
				else
					printf("\nFirst reader: %s (%d)\n\n", rdrs, readersLen);

				pReleaseContext(hCtx);
			}
		}

		FreeLibrary(mod);
	}

	return 0;
}

#else

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Linux/Mac ////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wintypes.h>
#include <winscard.h>
#include <dlfcn.h>

typedef LONG (*t_SCardEstablishContext)(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext);
typedef LONG (*t_SCardReleaseContext)(SCARDCONTEXT hContext);
typedef LONG (* t_SCardListReaders)(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders);

int main()
{
	void *mod = NULL;
	int is32bit = 4 == sizeof(int);

	printf("This program uses dlopen() to dynamically load the libpcsclite.so and call some functions on it\n");

	mod = dlopen("/System/Library/Frameworks/PCSC.framework/PCSC", RTLD_LAZY);
	if (!mod && is32bit)
		mod = dlopen("/lib/i386-linux-gnu/libpcte.so.1", RTLD_LAZY);
	if (!mod && is32bit)
		mod = dlopen("/usr/lib/libpcsclite.so", RTLD_LAZY);
	if (!mod && is32bit)
		mod = dlopen("/lib/libpcsclite.so", RTLD_LAZY);
	if (!mod && !is32bit)
		mod = dlopen("/lib64/libpcsclite.so", RTLD_LAZY);
	if (!mod && !is32bit)
		mod = dlopen("/usr/lib64/libpcsclite.so", RTLD_LAZY);
	if (!mod && !is32bit)
		mod = dlopen("/lib/x86_64-linux-gnu/libpcsclite.so.1", RTLD_LAZY);
	if (!mod)
		printf("dlopen(libpcsclite) failed\n");
	else
	{
		t_SCardEstablishContext pEstablischContext = NULL;
		t_SCardReleaseContext pReleaseContext = NULL;
		t_SCardListReaders pListReaders = NULL;

		printf("libpcsclite.so loaded, now calling dlsym() to obtain the adresses of some functions\n");

		pEstablischContext = (t_SCardEstablishContext) dlsym(mod, "SCardEstablishContext");
		pReleaseContext = (t_SCardReleaseContext) dlsym(mod, "SCardReleaseContext");
		pListReaders = (t_SCardListReaders) dlsym(mod, "SCardListReaders");

		if (!(pEstablischContext && pReleaseContext && pListReaders))
			printf("Couldn't find one or more proc addresses\n");
		else
		{
			SCARDCONTEXT hCtx;
			long ret = 0;

			printf("Addresses obtained, now calling the functions (starting with pEstablischContext = %p)\n", pEstablischContext);

			ret = pEstablischContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);
			if (ret != 0)
				printf("pEstablischContext(): 0x%0x (%d)\n", (unsigned) ret, (int) ret);
			else
			{
				char rdrs[1000];
				DWORD readersLen = sizeof(rdrs);
				rdrs[0] = '\0';
				ret = pListReaders(hCtx, NULL, rdrs, &readersLen);
				if (ret != 0)
					printf("pListReaders(): 0x%0x (%d)\n", (unsigned) ret, (int) ret);
				else
				{
					printf("First reader: %s\n", rdrs);
					printf("\nOK, all seems to work fine\n");
				}

				pReleaseContext(hCtx);
			}
		}

		dlclose(mod);
	}

	return 0;
}

#endif
