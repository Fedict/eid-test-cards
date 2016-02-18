///// Automatically generated by makeproxy, don't modify! /////

#include "proxy_winscard_imp.h"
#include "data_winscard.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

void *hMod = NULL;

extern void __attribute__((constructor)) _init()
{
	int i;
	for (i = 0; REAL_LIBS[i] && !hMod; i++)
		hMod = dlopen(REAL_LIBS[i], RTLD_LAZY);
	if (!hMod)
	{
		printf("ERR _init(): unable to load any of the %d REAL_LIBS\n", i);
		fflush(stdout);
		return;
	}

		pSCardEstablishContext = (tSCardEstablishContext)
			dlsym(hMod, "SCardEstablishContext");
		pSCardReleaseContext = (tSCardReleaseContext)
			dlsym(hMod, "SCardReleaseContext");
		pSCardIsValidContext = (tSCardIsValidContext)
			dlsym(hMod, "SCardIsValidContext");
		pSCardConnect = (tSCardConnect)
			dlsym(hMod, "SCardConnect");
		pSCardReconnect = (tSCardReconnect)
			dlsym(hMod, "SCardReconnect");
		pSCardDisconnect = (tSCardDisconnect)
			dlsym(hMod, "SCardDisconnect");
		pSCardBeginTransaction = (tSCardBeginTransaction)
			dlsym(hMod, "SCardBeginTransaction");
		pSCardEndTransaction = (tSCardEndTransaction)
			dlsym(hMod, "SCardEndTransaction");
		pSCardStatus = (tSCardStatus)
			dlsym(hMod, "SCardStatus");
		pSCardGetStatusChange = (tSCardGetStatusChange)
			dlsym(hMod, "SCardGetStatusChange");
		pSCardControl = (tSCardControl)
			dlsym(hMod, "SCardControl");
		pSCardTransmit = (tSCardTransmit)
			dlsym(hMod, "SCardTransmit");
		pSCardListReaderGroups = (tSCardListReaderGroups)
			dlsym(hMod, "SCardListReaderGroups");
		pSCardListReaders = (tSCardListReaders)
			dlsym(hMod, "SCardListReaders");
		pSCardFreeMemory = (tSCardFreeMemory)
			dlsym(hMod, "SCardFreeMemory");
		pSCardCancel = (tSCardCancel)
			dlsym(hMod, "SCardCancel");
		pSCardGetAttrib = (tSCardGetAttrib)
			dlsym(hMod, "SCardGetAttrib");
		pSCardSetAttrib = (tSCardSetAttrib)
			dlsym(hMod, "SCardSetAttrib");

        Init_g_rgSCardT0Pci(NULL);
        Init_g_rgSCardT1Pci(NULL);
        Init_g_rgSCardRawPci(NULL);

		proxy_init();
}

extern void __attribute__((destructor)) _fini()
{
	if (hMod)
		dlclose(hMod);
	proxy_exit();
}

tSCardEstablishContext pSCardEstablishContext;
tSCardReleaseContext pSCardReleaseContext;
tSCardIsValidContext pSCardIsValidContext;
tSCardConnect pSCardConnect;
tSCardReconnect pSCardReconnect;
tSCardDisconnect pSCardDisconnect;
tSCardBeginTransaction pSCardBeginTransaction;
tSCardEndTransaction pSCardEndTransaction;
tSCardStatus pSCardStatus;
tSCardGetStatusChange pSCardGetStatusChange;
tSCardControl pSCardControl;
tSCardTransmit pSCardTransmit;
tSCardListReaderGroups pSCardListReaderGroups;
tSCardListReaders pSCardListReaders;
tSCardFreeMemory pSCardFreeMemory;
tSCardCancel pSCardCancel;
tSCardGetAttrib pSCardGetAttrib;
tSCardSetAttrib pSCardSetAttrib;

PCSC_API LONG SCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
#ifdef USE_PROXY_LIB_FOR_SCardEstablishContext
	return imp_SCardEstablishContext(dwScope, pvReserved1, pvReserved2, phContext);
#else
	return pSCardEstablishContext(dwScope, pvReserved1, pvReserved2, phContext);
#endif
}

PCSC_API LONG SCardReleaseContext(SCARDCONTEXT hContext)
{
#ifdef USE_PROXY_LIB_FOR_SCardReleaseContext
	return imp_SCardReleaseContext(hContext);
#else
	return pSCardReleaseContext(hContext);
#endif
}

PCSC_API LONG SCardIsValidContext(SCARDCONTEXT hContext)
{
#ifdef USE_PROXY_LIB_FOR_SCardIsValidContext
	return imp_SCardIsValidContext(hContext);
#else
	return pSCardIsValidContext(hContext);
#endif
}

PCSC_API LONG SCardConnect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
#ifdef USE_PROXY_LIB_FOR_SCardConnect
	return imp_SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
#else
	return pSCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
#endif
}

PCSC_API LONG SCardReconnect(SCARDHANDLE hCard, DWORD dwShareMode, DWORD dwPreferredProtocols, DWORD dwInitialization, LPDWORD pdwActiveProtocol)
{
#ifdef USE_PROXY_LIB_FOR_SCardReconnect
	return imp_SCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
#else
	return pSCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
#endif
}

PCSC_API LONG SCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
#ifdef USE_PROXY_LIB_FOR_SCardDisconnect
	return imp_SCardDisconnect(hCard, dwDisposition);
#else
	return pSCardDisconnect(hCard, dwDisposition);
#endif
}

PCSC_API LONG SCardBeginTransaction(SCARDHANDLE hCard)
{
#ifdef USE_PROXY_LIB_FOR_SCardBeginTransaction
	return imp_SCardBeginTransaction(hCard);
#else
	return pSCardBeginTransaction(hCard);
#endif
}

PCSC_API LONG SCardEndTransaction(SCARDHANDLE hCard, DWORD dwDisposition)
{
#ifdef USE_PROXY_LIB_FOR_SCardEndTransaction
	return imp_SCardEndTransaction(hCard, dwDisposition);
#else
	return pSCardEndTransaction(hCard, dwDisposition);
#endif
}

PCSC_API LONG SCardStatus(SCARDHANDLE hCard, LPSTR mszReaderName, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
#ifdef USE_PROXY_LIB_FOR_SCardStatus
	return imp_SCardStatus(hCard, mszReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
#else
	return pSCardStatus(hCard, mszReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
#endif
}

PCSC_API LONG SCardGetStatusChange(SCARDCONTEXT hContext, DWORD dwTimeout, LPSCARD_READERSTATE rgReaderStates, DWORD cReaders)
{
#ifdef USE_PROXY_LIB_FOR_SCardGetStatusChange
	return imp_SCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);
#else
	return pSCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);
#endif
}

PCSC_API LONG SCardControl(SCARDHANDLE hCard, DWORD dwControlCode, LPCVOID pbSendBuffer, DWORD cbSendLength, LPVOID pbRecvBuffer, DWORD cbRecvLength, LPDWORD lpBytesReturned)
{
#ifdef USE_PROXY_LIB_FOR_SCardControl
	return imp_SCardControl(hCard, dwControlCode, pbSendBuffer, cbSendLength, pbRecvBuffer, cbRecvLength, lpBytesReturned);
#else
	return pSCardControl(hCard, dwControlCode, pbSendBuffer, cbSendLength, pbRecvBuffer, cbRecvLength, lpBytesReturned);
#endif
}

PCSC_API LONG SCardTransmit(SCARDHANDLE hCard, const SCARD_IO_REQUEST * pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength, SCARD_IO_REQUEST * pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
#ifdef USE_PROXY_LIB_FOR_SCardTransmit
	return imp_SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
#else
	return pSCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
#endif
}

PCSC_API LONG SCardListReaderGroups(SCARDCONTEXT hContext, LPSTR mszGroups, LPDWORD pcchGroups)
{
#ifdef USE_PROXY_LIB_FOR_SCardListReaderGroups
	return imp_SCardListReaderGroups(hContext, mszGroups, pcchGroups);
#else
	return pSCardListReaderGroups(hContext, mszGroups, pcchGroups);
#endif
}

PCSC_API LONG SCardListReaders(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders)
{
#ifdef USE_PROXY_LIB_FOR_SCardListReaders
	return imp_SCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
#else
	return pSCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
#endif
}

PCSC_API LONG SCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem)
{
#ifdef USE_PROXY_LIB_FOR_SCardFreeMemory
	return imp_SCardFreeMemory(hContext, pvMem);
#else
	return pSCardFreeMemory(hContext, pvMem);
#endif
}

PCSC_API LONG SCardCancel(SCARDCONTEXT hContext)
{
#ifdef USE_PROXY_LIB_FOR_SCardCancel
	return imp_SCardCancel(hContext);
#else
	return pSCardCancel(hContext);
#endif
}

PCSC_API LONG SCardGetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPBYTE pbAttr, LPDWORD pcbAttrLen)
{
#ifdef USE_PROXY_LIB_FOR_SCardGetAttrib
	return imp_SCardGetAttrib(hCard, dwAttrId, pbAttr, pcbAttrLen);
#else
	return pSCardGetAttrib(hCard, dwAttrId, pbAttr, pcbAttrLen);
#endif
}

PCSC_API LONG SCardSetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPCBYTE pbAttr, DWORD cbAttrLen)
{
#ifdef USE_PROXY_LIB_FOR_SCardSetAttrib
	return imp_SCardSetAttrib(hCard, dwAttrId, pbAttr, cbAttrLen);
#else
	return pSCardSetAttrib(hCard, dwAttrId, pbAttr, cbAttrLen);
#endif
}

