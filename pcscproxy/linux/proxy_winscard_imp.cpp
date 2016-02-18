#include <stdio.h>
#include <winscard.h>
#include "proxy_winscard_imp.h"
#include "WinscardImpl.h"

// NULL terminated string array containg the possible locations of the lib to be proxied
const char *REAL_LIBS[] = {
	"REALPCSCLIB",  // needed when we proxy dlopen() -- need if libpcsclite is loaded dynamically (see ../dlopenproxy/) 
#ifdef MAC_OS_X
	"/System/Library/Frameworks/PCSC.framework/PCSC",
#elif defined LINUX64BIT
	"/usr/lib64/libpcsclite.so.1",
	"/lib/x86_64-linux-gnu/libpcsclite.so.1",
	"/lib64/libpcsclite.so.1",
#else
	"/usr/lib/libpcsclite.so.1",
	"/lib/i386-linux-gnu/libpcsclite.so.1",
	"/lib/libpcsclite.so.1",
#endif
	NULL
};

using namespace zetes::pcscproxy;

WinscardImpl *impl = 0;

/* Called by _init() */
void proxy_init()
{
    impl = new WinscardImpl();
}

/* Called by _fini() */
void proxy_exit()
{
    if (impl)
        delete impl;
}

#ifdef USE_PROXY_LIB_FOR_SCardEstablishContext
PCSC_API LONG imp_SCardEstablishContext(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	return pSCardEstablishContext(dwScope, pvReserved1, pvReserved2, phContext);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardReleaseContext
PCSC_API LONG imp_SCardReleaseContext(SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIsValidContext
PCSC_API LONG imp_SCardIsValidContext(SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardConnect
PCSC_API LONG imp_SCardConnect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	return impl->SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardReconnect
PCSC_API LONG imp_SCardReconnect(SCARDHANDLE hCard, DWORD dwShareMode, DWORD dwPreferredProtocols, DWORD dwInitialization, LPDWORD pdwActiveProtocol)
{
	return impl->SCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardDisconnect
PCSC_API LONG imp_SCardDisconnect(SCARDHANDLE hCard, DWORD dwDisposition)
{
	return impl->SCardDisconnect(hCard, dwDisposition);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardBeginTransaction
PCSC_API LONG imp_SCardBeginTransaction(SCARDHANDLE hCard)
{
	return impl->SCardBeginTransaction(hCard);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardEndTransaction
PCSC_API LONG imp_SCardEndTransaction(SCARDHANDLE hCard, DWORD dwDisposition)
{
	return impl->SCardEndTransaction(hCard, dwDisposition);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardStatus
PCSC_API LONG imp_SCardStatus(SCARDHANDLE hCard, LPSTR mszReaderName, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	return impl->SCardStatus(hCard, mszReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetStatusChange
PCSC_API LONG imp_SCardGetStatusChange(SCARDCONTEXT hContext, DWORD dwTimeout, LPSCARD_READERSTATE rgReaderStates, DWORD cReaders)
{
	return impl->SCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardControl
PCSC_API LONG imp_SCardControl(SCARDHANDLE hCard, DWORD dwControlCode, LPCVOID pbSendBuffer, DWORD cbSendLength, LPVOID pbRecvBuffer, DWORD cbRecvLength, LPDWORD lpBytesReturned)
{
	return impl->SCardControl(hCard, dwControlCode, pbSendBuffer, cbSendLength, pbRecvBuffer, cbRecvLength, lpBytesReturned);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardTransmit
PCSC_API LONG imp_SCardTransmit(SCARDHANDLE hCard, const SCARD_IO_REQUEST * pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength, SCARD_IO_REQUEST * pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	return impl->SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReaderGroups
PCSC_API LONG imp_SCardListReaderGroups(SCARDCONTEXT hContext, LPSTR mszGroups, LPDWORD pcchGroups)
{
    // TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReaders
PCSC_API LONG imp_SCardListReaders(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders)
{
	return impl->SCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardFreeMemory
PCSC_API LONG imp_SCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardCancel
PCSC_API LONG imp_SCardCancel(SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetAttrib
PCSC_API LONG imp_SCardGetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPBYTE pbAttr, LPDWORD pcbAttrLen)
{
	return impl->SCardGetAttrib(hCard, dwAttrId, pbAttr, pcbAttrLen);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardSetAttrib
PCSC_API LONG imp_SCardSetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPCBYTE pbAttr, DWORD cbAttrLen)
{
	// TODO
}
#endif

