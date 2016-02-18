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
#include "proxy_WinSCard_imp.h"
#include "WinscardImpl.h"
#include <windows.h>
//#include <atlbase.h>

// NULL terminated string array containg the possible locations of the lib to be proxied
const char *REAL_LIBS[] = {
	"%SYSTEM32%\\winscard.dll",
	NULL
};

using namespace fedict::pcscproxy;

WinscardImpl *impl = 0;

/* Called by DllMain(PROCESS_ATTACH) */
void proxy_init()
{
	impl = new WinscardImpl();
}

/* Called by DllMain(PROCESS_DETTACH) */
void proxy_exit()
{
    if (impl)
    	delete impl;
}

//caller is responsible for freeing the returned string
//utf8len is the length of the returned string in bytes, excluding the terminating null
char* Utf16ToUtf8(const wchar_t * utf16string, unsigned long* utf8len)
{
	int utf8bytesize = WideCharToMultiByte(CP_UTF8, 0, utf16string, -1, NULL, 0, NULL, NULL);
	char *utf8string = (char*) malloc(utf8bytesize);
	WideCharToMultiByte(CP_UTF8, 0, utf16string, -1, utf8string, utf8bytesize, NULL, NULL);
	*utf8len = utf8bytesize - 1;

	return utf8string;
}

#ifdef USE_PROXY_LIB_FOR_SCardEstablishContext
WINSCARDAPI LONG WINAPI imp_SCardEstablishContext( DWORD dwScope,  LPCVOID pvReserved1,  LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardReleaseContext
WINSCARDAPI LONG WINAPI imp_SCardReleaseContext(     SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIsValidContext
WINSCARDAPI LONG WINAPI imp_SCardIsValidContext(     SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReaderGroupsA
WINSCARDAPI LONG WINAPI imp_SCardListReaderGroupsA(     SCARDCONTEXT hContext,     LPSTR mszGroups,  LPDWORD pcchGroups)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReaderGroupsW
WINSCARDAPI LONG WINAPI imp_SCardListReaderGroupsW(     SCARDCONTEXT hContext,     LPWSTR mszGroups,  LPDWORD pcchGroups)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReadersA
WINSCARDAPI LONG WINAPI imp_SCardListReadersA(     SCARDCONTEXT hContext,      LPCSTR mszGroups,     LPSTR mszReaders,  LPDWORD pcchReaders)
{
	return impl->SCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListReadersW
WINSCARDAPI LONG WINAPI imp_SCardListReadersW(     SCARDCONTEXT hContext,      LPCWSTR mszGroups,     LPWSTR mszReaders,  LPDWORD pcchReaders)
{
	unsigned long len_a;
	char* a_mszGroups = Utf16ToUtf8(mszGroups, &len_a);

    bool bAutoAlloc = SCARD_AUTOALLOCATE == *pcchReaders;
    char *a_mszReaders = NULL;
    char *autobuf = NULL;
    bool bAllocatedMemory = false;
    if (bAutoAlloc)
    {
        a_mszReaders = (char *) &autobuf;
    }
    else if (NULL != mszReaders)
    {
        a_mszReaders = (char *) malloc(*pcchReaders);
        bAllocatedMemory = true;
    }

    LONG result = impl->SCardListReaders(hContext, a_mszGroups, a_mszReaders, pcchReaders);
    if (SCARD_S_SUCCESS == result)
    {
        if (bAutoAlloc)
        {
            wchar_t *tmp = (wchar_t *) malloc(*pcchReaders * sizeof(wchar_t));
            for (DWORD i = 0; i < *pcchReaders; i++)
                tmp[i] = (wchar_t) autobuf[i];
            impl->m_autoAllocs.push_back(tmp);
            *( (LPWSTR *) mszReaders) = tmp;
        }
        else if (NULL != mszReaders)
        {
            for (DWORD i = 0; i < *pcchReaders; i++)
                mszReaders[i] = (wchar_t) a_mszReaders[i];
        }
    }

    if (bAllocatedMemory)
        free(a_mszReaders);
	if(a_mszGroups != NULL)
		free(a_mszGroups);

    return result;
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListCardsA
WINSCARDAPI LONG WINAPI imp_SCardListCardsA(     SCARDCONTEXT hContext,      LPCBYTE pbAtr,      LPCGUID rgquidInterfaces,      DWORD cguidInterfaceCount,     LPSTR mszCards,  LPDWORD pcchCards)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListCardsW
WINSCARDAPI LONG WINAPI imp_SCardListCardsW(     SCARDCONTEXT hContext,      LPCBYTE pbAtr,      LPCGUID rgquidInterfaces,      DWORD cguidInterfaceCount,     LPWSTR mszCards,  LPDWORD pcchCards)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListInterfacesA
WINSCARDAPI LONG WINAPI imp_SCardListInterfacesA(     SCARDCONTEXT hContext,      LPCSTR szCard,     LPGUID pguidInterfaces,  LPDWORD pcguidInterfaces)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardListInterfacesW
WINSCARDAPI LONG WINAPI imp_SCardListInterfacesW(     SCARDCONTEXT hContext,      LPCWSTR szCard,     LPGUID pguidInterfaces,  LPDWORD pcguidInterfaces)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetProviderIdA
WINSCARDAPI LONG WINAPI imp_SCardGetProviderIdA(     SCARDCONTEXT hContext,      LPCSTR szCard,     LPGUID pguidProviderId)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetProviderIdW
WINSCARDAPI LONG WINAPI imp_SCardGetProviderIdW(     SCARDCONTEXT hContext,      LPCWSTR szCard,     LPGUID pguidProviderId)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetCardTypeProviderNameA
WINSCARDAPI LONG WINAPI imp_SCardGetCardTypeProviderNameA(SCARDCONTEXT hContext, LPCSTR szCardName, DWORD dwProviderId, LPSTR szProvider, LPDWORD pcchProvider)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetCardTypeProviderNameW
WINSCARDAPI LONG WINAPI imp_SCardGetCardTypeProviderNameW(SCARDCONTEXT hContext, LPCWSTR szCardName, DWORD dwProviderId, LPWSTR szProvider, LPDWORD pcchProvider)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceReaderGroupA
WINSCARDAPI LONG WINAPI imp_SCardIntroduceReaderGroupA(SCARDCONTEXT hContext, LPCSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceReaderGroupW
WINSCARDAPI LONG WINAPI imp_SCardIntroduceReaderGroupW(SCARDCONTEXT hContext, LPCWSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetReaderGroupA
WINSCARDAPI LONG WINAPI imp_SCardForgetReaderGroupA(SCARDCONTEXT hContext, LPCSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetReaderGroupW
WINSCARDAPI LONG WINAPI imp_SCardForgetReaderGroupW(SCARDCONTEXT hContext, LPCWSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceReaderA
WINSCARDAPI LONG WINAPI imp_SCardIntroduceReaderA(SCARDCONTEXT hContext, LPCSTR szReaderName, LPCSTR szDeviceName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceReaderW
WINSCARDAPI LONG WINAPI imp_SCardIntroduceReaderW(SCARDCONTEXT hContext, LPCWSTR szReaderName, LPCWSTR szDeviceName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetReaderA
WINSCARDAPI LONG WINAPI imp_SCardForgetReaderA(SCARDCONTEXT hContext, LPCSTR szReaderName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetReaderW
WINSCARDAPI LONG WINAPI imp_SCardForgetReaderW(SCARDCONTEXT hContext, LPCWSTR szReaderName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardAddReaderToGroupA
WINSCARDAPI LONG WINAPI imp_SCardAddReaderToGroupA(SCARDCONTEXT hContext, LPCSTR szReaderName, LPCSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardAddReaderToGroupW
WINSCARDAPI LONG WINAPI imp_SCardAddReaderToGroupW(SCARDCONTEXT hContext, LPCWSTR szReaderName, LPCWSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardRemoveReaderFromGroupA
WINSCARDAPI LONG WINAPI imp_SCardRemoveReaderFromGroupA(SCARDCONTEXT hContext, LPCSTR szReaderName, LPCSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardRemoveReaderFromGroupW
WINSCARDAPI LONG WINAPI imp_SCardRemoveReaderFromGroupW(SCARDCONTEXT hContext, LPCWSTR szReaderName, LPCWSTR szGroupName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceCardTypeA
WINSCARDAPI LONG WINAPI imp_SCardIntroduceCardTypeA(SCARDCONTEXT hContext, LPCSTR szCardName, LPCGUID pguidPrimaryProvider, LPCGUID rgguidInterfaces, DWORD dwInterfaceCount, LPCBYTE pbAtr, LPCBYTE pbAtrMask, DWORD cbAtrLen)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardIntroduceCardTypeW
WINSCARDAPI LONG WINAPI imp_SCardIntroduceCardTypeW(SCARDCONTEXT hContext, LPCWSTR szCardName, LPCGUID pguidPrimaryProvider, LPCGUID rgguidInterfaces, DWORD dwInterfaceCount, LPCBYTE pbAtr, LPCBYTE pbAtrMask, DWORD cbAtrLen)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardSetCardTypeProviderNameA
WINSCARDAPI LONG WINAPI imp_SCardSetCardTypeProviderNameA(SCARDCONTEXT hContext, LPCSTR szCardName, DWORD dwProviderId, LPCSTR szProvider)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardSetCardTypeProviderNameW
WINSCARDAPI LONG WINAPI imp_SCardSetCardTypeProviderNameW(SCARDCONTEXT hContext, LPCWSTR szCardName, DWORD dwProviderId, LPCWSTR szProvider)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetCardTypeA
WINSCARDAPI LONG WINAPI imp_SCardForgetCardTypeA(SCARDCONTEXT hContext, LPCSTR szCardName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardForgetCardTypeW
WINSCARDAPI LONG WINAPI imp_SCardForgetCardTypeW(SCARDCONTEXT hContext, LPCWSTR szCardName)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardFreeMemory
WINSCARDAPI LONG WINAPI imp_SCardFreeMemory(SCARDCONTEXT hContext, LPCVOID pvMem)
{
 	return impl->SCardFreeMemory(hContext, (LPVOID) pvMem);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardAccessStartedEvent
WINSCARDAPI HANDLE WINAPI imp_SCardAccessStartedEvent(void )
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardReleaseStartedEvent
WINSCARDAPI void WINAPI imp_SCardReleaseStartedEvent(void )
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardLocateCardsA
WINSCARDAPI LONG WINAPI imp_SCardLocateCardsA(     SCARDCONTEXT hContext,      LPCSTR mszCards,  LPSCARD_READERSTATEA rgReaderStates,      DWORD cReaders)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardLocateCardsW
WINSCARDAPI LONG WINAPI imp_SCardLocateCardsW(     SCARDCONTEXT hContext,      LPCWSTR mszCards,  LPSCARD_READERSTATEW rgReaderStates,      DWORD cReaders)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardLocateCardsByATRA
WINSCARDAPI LONG WINAPI imp_SCardLocateCardsByATRA(     SCARDCONTEXT hContext,      LPSCARD_ATRMASK rgAtrMasks,      DWORD cAtrs,  LPSCARD_READERSTATEA rgReaderStates,      DWORD cReaders)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardLocateCardsByATRW
WINSCARDAPI LONG WINAPI imp_SCardLocateCardsByATRW(     SCARDCONTEXT hContext,      LPSCARD_ATRMASK rgAtrMasks,      DWORD cAtrs,  LPSCARD_READERSTATEW rgReaderStates,      DWORD cReaders)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetStatusChangeA
WINSCARDAPI LONG WINAPI imp_SCardGetStatusChangeA(     SCARDCONTEXT hContext,      DWORD dwTimeout,  LPSCARD_READERSTATEA rgReaderStates,      DWORD cReaders)
{
	return impl->SCardGetStatusChange(hContext, dwTimeout, rgReaderStates, cReaders);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetStatusChangeW
WINSCARDAPI LONG WINAPI imp_SCardGetStatusChangeW(     SCARDCONTEXT hContext,      DWORD dwTimeout,  LPSCARD_READERSTATEW rgReaderStates,      DWORD cReaders)
{
	//copy to ansi version of readerstates
	LPSCARD_READERSTATE_A a_rgReaderStates = new SCARD_READERSTATE_A[cReaders];
	for (size_t i = 0; i < cReaders; i++)
	{
		//convert szReader to ansi
		//size_t readerNameLen = wcslen(rgReaderStates[i].szReader);
		 unsigned long utf8len;
		//LPSTR  a_szReader    = new CHAR[readerNameLen + 1];

		LPSTR a_szReader = Utf16ToUtf8(rgReaderStates[i].szReader, &utf8len);

		//memcpy(a_szReader, CW2A(rgReaderStates[i].szReader), readerNameLen + 1);
		a_rgReaderStates[i].szReader = a_szReader;
		//copy relevant params
		a_rgReaderStates[i].dwCurrentState = rgReaderStates[i].dwCurrentState;
		a_rgReaderStates[i].dwEventState   = rgReaderStates[i].dwEventState;
	}

	//call ansi version of SCardGetStatusChange
	LONG result = impl->SCardGetStatusChange(hContext, dwTimeout, a_rgReaderStates, cReaders);

	//copy back to existing wide readerstates and
	//!! delete ansi szReader created on the heap above !!
	for (size_t i = 0; i < cReaders; i++)
	{
		//delete szReader
		delete[] a_rgReaderStates[i].szReader;
		//copy relevant params
		rgReaderStates[i].dwEventState = a_rgReaderStates[i].dwEventState;
		rgReaderStates[i].cbAtr        = a_rgReaderStates[i].cbAtr;
		memcpy(rgReaderStates[i].rgbAtr, a_rgReaderStates[i].rgbAtr, 36);
	}

	//delete readerstates
	delete[] a_rgReaderStates;

	return result;
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardCancel
WINSCARDAPI LONG WINAPI imp_SCardCancel(     SCARDCONTEXT hContext)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardConnectA
WINSCARDAPI LONG WINAPI imp_SCardConnectA(     SCARDCONTEXT hContext,      LPCSTR szReader,      DWORD dwShareMode,      DWORD dwPreferredProtocols,     LPSCARDHANDLE phCard,     LPDWORD pdwActiveProtocol)
{
    return impl->SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardConnectW
WINSCARDAPI LONG WINAPI imp_SCardConnectW(     SCARDCONTEXT hContext,      LPCWSTR szReader,      DWORD dwShareMode,      DWORD dwPreferredProtocols,     LPSCARDHANDLE phCard,     LPDWORD pdwActiveProtocol)
{
	unsigned long len_a;
	char* szReader_a = Utf16ToUtf8(szReader, &len_a);
	LONG retVal;
    retVal = impl->SCardConnect(hContext, szReader_a, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
	if(szReader_a != NULL)
		free(szReader_a);
	return retVal;
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardReconnect
WINSCARDAPI LONG WINAPI imp_SCardReconnect(     SCARDHANDLE hCard,      DWORD dwShareMode,      DWORD dwPreferredProtocols,      DWORD dwInitialization,     LPDWORD pdwActiveProtocol)
{
	return impl->SCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardDisconnect
WINSCARDAPI LONG WINAPI imp_SCardDisconnect(     SCARDHANDLE hCard,      DWORD dwDisposition)
{
	return impl->SCardDisconnect(hCard, dwDisposition);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardBeginTransaction
WINSCARDAPI LONG WINAPI imp_SCardBeginTransaction(     SCARDHANDLE hCard)
{
	return impl->SCardBeginTransaction(hCard);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardEndTransaction
WINSCARDAPI LONG WINAPI imp_SCardEndTransaction(     SCARDHANDLE hCard,      DWORD dwDisposition)
{
	return impl->SCardEndTransaction(hCard, dwDisposition);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardCancelTransaction
WINSCARDAPI LONG WINAPI imp_SCardCancelTransaction(     SCARDHANDLE hCard)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardState
WINSCARDAPI LONG WINAPI imp_SCardState(SCARDHANDLE hCard, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	return impl->SCardState(hCard, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardStatusA
WINSCARDAPI LONG WINAPI imp_SCardStatusA(SCARDHANDLE hCard, LPSTR szReaderName, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
	return impl->SCardStatus(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardStatusW
WINSCARDAPI LONG WINAPI imp_SCardStatusW(SCARDHANDLE hCard, LPWSTR szReaderName, LPDWORD pcchReaderLen, LPDWORD pdwState, LPDWORD pdwProtocol, LPBYTE pbAtr, LPDWORD pcbAtrLen)
{
    bool bAutoAlloc = SCARD_AUTOALLOCATE == *pcchReaderLen;
    char *a_szReaderName = NULL;
    char *autobuf = NULL;
    bool bAllocatedMemory = false;
    if (bAutoAlloc)
    {
        a_szReaderName = (char *) &autobuf;
    }
    else if (NULL != szReaderName)
    {
        a_szReaderName = (char *) malloc(*pcchReaderLen);
        bAllocatedMemory = true;
    }

    LONG result = impl->SCardStatus(hCard, a_szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
    if (SCARD_S_SUCCESS == result)
    {
        if (bAutoAlloc)
        {
            wchar_t *tmp = (wchar_t *) malloc(*pcchReaderLen * sizeof(wchar_t));
            for (DWORD i = 0; i < *pcchReaderLen; i++)
                tmp[i] = (wchar_t) autobuf[i];
            impl->m_autoAllocs.push_back(tmp);
            *( (LPWSTR *) szReaderName) = tmp;
        }
        else if (NULL != szReaderName)
        {
            for (DWORD i = 0; i < *pcchReaderLen; i++)
                szReaderName[i] = (wchar_t) a_szReaderName[i];
        }
    }

    if (bAllocatedMemory)
        free(a_szReaderName);

    return result;
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardTransmit
WINSCARDAPI LONG WINAPI imp_SCardTransmit(SCARDHANDLE hCard, LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength, LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	return impl->SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardControl
WINSCARDAPI LONG WINAPI imp_SCardControl(     SCARDHANDLE hCard,      DWORD dwControlCode,      LPCVOID lpInBuffer,      DWORD nInBufferSize,     LPVOID lpOutBuffer,      DWORD nOutBufferSize,     LPDWORD lpBytesReturned)
{
	return impl->SCardControl(hCard, dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardGetAttrib
WINSCARDAPI LONG WINAPI imp_SCardGetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPBYTE pbAttr, LPDWORD pcbAttrLen)
{
	return impl->SCardGetAttrib(hCard, dwAttrId, pbAttr, pcbAttrLen);
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardSetAttrib
WINSCARDAPI LONG WINAPI imp_SCardSetAttrib(SCARDHANDLE hCard, DWORD dwAttrId, LPCBYTE pbAttr, DWORD cbAttrLen)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardUIDlgSelectCardA
WINSCARDAPI LONG WINAPI imp_SCardUIDlgSelectCardA(LPOPENCARDNAMEA_EX p1)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardUIDlgSelectCardW
WINSCARDAPI LONG WINAPI imp_SCardUIDlgSelectCardW(LPOPENCARDNAMEW_EX p1)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_GetOpenCardNameA
WINSCARDAPI LONG WINAPI imp_GetOpenCardNameA(LPOPENCARDNAMEA p1)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_GetOpenCardNameW
WINSCARDAPI LONG WINAPI imp_GetOpenCardNameW(LPOPENCARDNAMEW p1)
{
	// TODO
}
#endif

#ifdef USE_PROXY_LIB_FOR_SCardDlgExtendedError
WINSCARDAPI LONG WINAPI imp_SCardDlgExtendedError(void )
{
	// TODO
}
#endif

