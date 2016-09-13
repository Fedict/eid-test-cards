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
#include "WinscardImpl.h"
#include "SoftReaderManager.h"
#include "SoftReader.h"
#include "SoftCard.h"
#include "Control.h"
#include "Logger.h"
#include "Carddata.h"
#include "FileStructure.h"
#include <string>
#include <algorithm>

#ifdef _WIN32
#define pSCardConnect pSCardConnectA
#define pSCardListReaders pSCardListReadersA
#define pSCardStatus pSCardStatusA
#define pSCardGetStatusChange pSCardGetStatusChangeA
#else
#define SCardListReadersA SCardListReaders
#define SCARD_READERSTATE_A SCARD_READERSTATE
#include <cstdlib>
#include <cstring>
#ifndef SCARD_AUTOALLOCATE
#define SCARD_AUTOALLOCATE    (DWORD) -1
#endif
#endif

using namespace fedict::pcscproxy;

WinscardImpl::WinscardImpl() : srmngr(NULL), g_listReadersCalled(false)
{
	srmngr = new SoftReaderManager();
}

WinscardImpl::~WinscardImpl()
{
	if (srmngr)
		delete srmngr;
}

// If you know the reader's name, you can call SCardConnect() without first calling SCardListReaders()
// But since our SCardListReaders() function creates the virtual readers, it must be called before
// any call to SCardConnect() (at least when giving the name of a virtual reader)
void WinscardImpl::ForceListReaders(IN SCARDCONTEXT hContext)
{
    g_listReadersCalled = true;
    DWORD len = SCARD_AUTOALLOCATE;
    char *buf = NULL;
    if (0 != SCardListReadersA(hContext, NULL, NULL, &len))
        printf("SCardConnect(): SCardListReaders() failed\n");
    if (buf)
        SCardFreeMemory(hContext, buf);
}

LONG WinscardImpl::SCardBeginTransaction(IN SCARDHANDLE hCard)
{
	LONG                    result = 0;
	SoftCard                *sc    = srmngr->getSoftCard(hCard);
	if (sc)
	{
		//call beginTransaction on original dll with hard handle
		result = pSCardBeginTransaction(sc->getHardHandle());
		//call beginTransaction on virtual
		if (SCARD_S_SUCCESS == result)
		{
			result = sc->beginTransaction();
		}
	}
	else
	{
		//it is a hard handle
		result = pSCardBeginTransaction(hCard);
	}

	return result;
}

LONG WinscardImpl::SCardConnect(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol)
{
	LONG           result = 0;
	//lookup SoftReader by soft name
	SoftReader     *sr = srmngr->getSoftReaderByName(szReader, N_SOFT);
	if (sr)
	{
		result = SCardConnect_Threading(hContext, sr->getHardReaderName().c_str(), dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);

		if (SCARD_S_SUCCESS == result)
		{
			//connect to hard card was successful so now create a soft card
			SCARDHANDLE hRealCard = *phCard;
			*phCard = sr->createSoftCard(*phCard)->getSoftHandle();
			logInfo("SCardConnect(%s): res = 0x%0x (%d), soft_hCard=%d, real_hCard=%d\n", szReader, result, result, *phCard, hRealCard);
		}
		else
			logError("SCardConnect(%s) failed: 0x%0x (%d)\n", szReader, result, result);
	}
	else
	{
		//apparently szReader is a hard name so we send the request to the
		//original dll and be done with it
		result = SCardConnect_Threading( hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
		logInfo("SCardConnect(%s): res = 0x%0x (%d), real_hCard=%d\n", szReader, result, result, *phCard);
	}

	return result;
}

LONG WinscardImpl::SCardDisconnect(IN SCARDHANDLE hCard, IN DWORD dwDisposition)
{
	SoftCard          *sc    = srmngr->getSoftCard(hCard);
	LONG              result = 0;
	if (sc)
	{
		//send to hard reader first
		SCARDHANDLE hRealCard = sc->getHardHandle();
		result = SCardDisconnect_Threading(hRealCard, dwDisposition);
		if (SCARD_S_SUCCESS == result)
		{
			//send to soft reader second
			SoftReader *sr = sc->getSoftReader();
			result = sr->disconnect(hCard, dwDisposition);
			logInfo("SCardDisconnect(soft_hCard=%d, real_hCard=%d): res = 0x%0x (%d)\n", hCard, hRealCard, result, result);
		}
	}
	else     //there is no softreader; the card handle is invalid
	{
		//send to hard reader
		result = SCardDisconnect_Threading(hCard, dwDisposition);
		logInfo("SCardDisconnect(real_hCard=%d): res = 0x%0x (%d)\n", hCard, result, result);
	}

	return result;
}

LONG WinscardImpl::SCardEndTransaction(IN SCARDHANDLE hCard, IN DWORD dwDisposition)
{
	LONG                  result = 0;
	SoftCard          *sc    = srmngr->getSoftCard(hCard);
	if (sc)
	{
		//call endTransaction on original dll with hard handle
		result = pSCardEndTransaction(sc->getHardHandle(), dwDisposition);
		//call endTransaction on virtual
		if (SCARD_S_SUCCESS == result)
		{
			result = sc->endTransaction(dwDisposition);
		}
	}
	else
	{
		//it is a hard handle
		result = pSCardEndTransaction(hCard, dwDisposition);
	}

	return result;
}

LONG WinscardImpl::SCardListReaders(IN SCARDCONTEXT hContext, IN LPCSTR mszGroups, OUT LPSTR mszReaders, IN OUT LPDWORD pcchReaders)
{
	LONG               result = 0;

	g_listReadersCalled = true;

	if (mszReaders == NULL)   //ignore buffer
	{
		DWORD buflen = 5000;
		char  *buf   = new char[buflen];
		//get hard readers names with buffer
		result = pSCardListReaders(hContext, mszGroups, buf, &buflen);
		//return available length considering soft names and hard names
		*pcchReaders = srmngr->calcListReadersLength(buf, buflen);
		delete[] buf;
	}
	else if (SCARD_AUTOALLOCATE == *pcchReaders)
	{
		LPTSTR buf    = NULL;
		DWORD  buflen = SCARD_AUTOALLOCATE;
		//send to hard reader
		result = pSCardListReaders(hContext, mszGroups, reinterpret_cast<LPTSTR>(&buf), &buflen);
		//calc len hard + soft names
		const size_t newLen    = srmngr->calcListReadersLength(buf, buflen);
		LPTSTR       * pRemote = reinterpret_cast<LPTSTR*>(mszReaders);

		*pRemote     = (LPTSTR) malloc(newLen * sizeof(char));
		*pcchReaders = (DWORD) newLen;
		memcpy(*pRemote, buf, buflen);
		srmngr->createSoftReaders(*pRemote, pcchReaders);
        m_autoAllocs.push_back(*pRemote); // Needed for SCardFreeMemory()

		result = pSCardFreeMemory(hContext, buf);
		if (SCARD_S_SUCCESS != result)
		{
			return result;
		}
	}
	else
	{
		//send to hard reader
		result = pSCardListReaders(hContext, mszGroups, mszReaders, pcchReaders);
		if (SCARD_S_SUCCESS != result)
		{
			return result;
		}
		//check to see if length is sufficient
		const size_t la = srmngr->calcListReadersLength(mszReaders, *pcchReaders);
		if (la < *pcchReaders)
		{
			return SCARD_E_INSUFFICIENT_BUFFER;
		}
		srmngr->createSoftReaders(mszReaders, pcchReaders);
	}
	return result;
}

LONG WinscardImpl::SCardReconnect(IN SCARDHANDLE hCard, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	IN DWORD dwInitialization, OUT LPDWORD pdwActiveProtocol)
{
	/*
	SoftReader       *sr    = srmngr->getSoftReaderByCardHandle(hCard, H_SOFT);
	LONG             result = 0;

	if (sr)
	{
		//send to hard reader first
		result = pSCardReconnect(sr->getSoftCard()->getHardHandle(), dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
		if (SCARD_S_SUCCESS == result)
		{
			//send to soft reader second
			result = sr->reconnect(dwInitialization);
		}
	}
	else     //there is not soft reader; the card handle is invalid
	{
		//send to hard reader
		result = pSCardReconnect(hCard, dwShareMode, dwPreferredProtocols, dwInitialization, pdwActiveProtocol);
	}

	return result;
	*/
	return SCARD_S_SUCCESS; // TODO
}

LONG WinscardImpl::SCardGetAttrib(IN SCARDHANDLE hCard, IN DWORD dwAttrId, OUT LPBYTE pbAttr, IN OUT LPDWORD pcbAttrLen)
{
	SoftCard *sc = srmngr->getSoftCard(hCard);
    SCARDHANDLE handle = (sc == NULL) ? hCard : sc->getHardHandle();

    return pSCardGetAttrib(handle, dwAttrId, pbAttr, pcbAttrLen);
}

#ifdef _WIN32
LONG WinscardImpl::SCardState(IN SCARDHANDLE hCard, OUT LPDWORD pdwState, OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, IN OUT LPDWORD pcbAtrLen)
{
	SoftCard *sc = srmngr->getSoftCard(hCard);
    SCARDHANDLE handle = (sc == NULL) ? hCard : sc->getHardHandle();

    return pSCardState(handle, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
}
#endif

LONG WinscardImpl::SCardTransmit(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer,
	IN DWORD cbSendLength, IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength)
{
	LONG            result = 0;
	SoftCard        *sc = srmngr->getSoftCard(hCard);
	bool            bSc = (NULL != sc);

	logTransmitSend(bSc , hCard, pbSendBuffer, cbSendLength);

	if (sc)
	{
		const APDURequest apduReq = APDURequest::createAPDURequest(pbSendBuffer, cbSendLength);

		if (apduReq.isGetCardData() || apduReq.isReadBinary() || apduReq.isSelectFile() ||
			(sc->lastCmdWasToCard() && apduReq.isGetResponse()))
		{
			//if no virt data loaded, then load it from file
			if (!(sc->isDataLoaded() || loadSoftData(sc)))
			{
				result = carddata::Carddata::cmdNotAvailable(pbRecvBuffer, pcbRecvLength);
				logTransmitRecv(bSc, hCard, result, pbRecvBuffer, *pcbRecvLength);
				return result;
			}
			//send to the SOFT reader
			result = sc->transmit(apduReq, pbRecvBuffer, pcbRecvLength);
			logTransmitRecv(bSc, hCard, result, pbRecvBuffer, *pcbRecvLength);
			sc->setLastCmdToCard(true);
		}
		else     //an apdu the soft card does not process needs to go to the hard reader
		{
			//if within transaction set the selected EF/DF on the hard reader first
			if (sc->firstTimeInTransaction() && sc->isDataLoaded())
			{
				if (!setSelectedOnHardCard(sc))
				{
					logError("Couldn't do a Select File on the real card");
					return SCARD_F_INTERNAL_ERROR;
				}
			}
			//now send the original request to the hard reader
			SCARDHANDLE hRealCard = sc->getHardHandle();
			result = pSCardTransmit(hRealCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
			logTransmitRecv(false, hRealCard, result, pbRecvBuffer, *pcbRecvLength);
			sc->setLastCmdToCard(false);
		}
	}
	else     //there is no softreader
	{
		//send to the HARD reader
		result = pSCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength, pioRecvPci, pbRecvBuffer, pcbRecvLength);
		logTransmitRecv(bSc, hCard, result, pbRecvBuffer, *pcbRecvLength);
	}

	return result;
}

LONG WinscardImpl::SCardFreeMemory(SCARDCONTEXT hContext, LPVOID pvMem)
{
    std::vector<void *>::iterator it = std::find(m_autoAllocs.begin(), m_autoAllocs.end(), pvMem);
    if (it == m_autoAllocs.end())
    {
        // In case the allocation was done in the real PCSC lib
        return pSCardFreeMemory(hContext, pvMem);
    }
    else
    {
        // In case the allocation was done here
        m_autoAllocs.erase(it);
    	free(pvMem);
    	return SCARD_S_SUCCESS;
    }
}

LONG WinscardImpl::SCardStatus(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
	OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen)
{
	LONG          result = 0;
	SoftCard      *sc = srmngr->getSoftCard(hCard);

	if (sc)
	{
		SoftReader *sr = sc->getSoftReader();

		const size_t softNameLen = sr->getSoftReaderName().size();
		if (SCARD_AUTOALLOCATE == *pcchReaderLen)
		{
			//Make life easy and pass the name of the hardreader instead of SCARD_AUTOALLOCATE
			const size_t hardNameLen    = sr->getHardReaderName().size();
			DWORD        hardNameBufLen = (DWORD) hardNameLen + 2; //2 for double null terminator
			LPTSTR       hardNameBuf    = new char[hardNameBufLen];
			memcpy(hardNameBuf, sr->getHardReaderName().c_str(), hardNameLen);
			hardNameBuf[hardNameLen]     = '\0';  //null terminate single string
			hardNameBuf[hardNameLen + 1] = '\0';  //null terminate multi string
			result                       = pSCardStatus(sc->getHardHandle(), hardNameBuf, &hardNameBufLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
			if (SCARD_S_SUCCESS != result)
			{
				delete[] hardNameBuf;
				return result;
			}
			delete[] hardNameBuf;

			LPTSTR * pRemote = reinterpret_cast<LPTSTR*>(szReaderName);
            *pRemote = (LPTSTR) malloc((softNameLen + 2) * sizeof(char));
			memcpy(*pRemote, sr->getSoftReaderName().c_str(), softNameLen);
			(*pRemote)[softNameLen]     = '\0';   //null terminate single string
			(*pRemote)[softNameLen + 1] = '\0';   //null terminate multi string
            m_autoAllocs.push_back(*pRemote); // Needed for SCardFreeMemory()
		}
		else
		{
			//send to the HARD reader
			result = pSCardStatus(sc->getHardHandle(), szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
			if (SCARD_S_SUCCESS != result)
			{
				return result;
			}
			//check to see if length is sufficient
			const size_t la = softNameLen + 2; //2 for double null terminator
			if (la < *pcchReaderLen)
			{
				return SCARD_E_INSUFFICIENT_BUFFER;
			}
			if (szReaderName != NULL)
			{
				memcpy(szReaderName, sr->getSoftReaderName().c_str(), softNameLen);
				szReaderName[softNameLen]     = '\0'; //null terminate single string
				szReaderName[softNameLen + 1] = '\0'; //null terminate multi string
			}
		}
		*pcchReaderLen = (DWORD) softNameLen + 2; //2 for double null terminator
	}
	else                                          //there is no softreader
	{
		//send to the HARD reader
		result = pSCardStatus(hCard, szReaderName, pcchReaderLen, pdwState, pdwProtocol, pbAtr, pcbAtrLen);
	}
	return result;
}

static LPSCARD_READERSTATE_A GetState(LPSCARD_READERSTATE_A pStates, DWORD len, const char *readerName)
{
	for (DWORD i = 0; i < len; i++)
	{
		if (0 == strcmp(pStates[i].szReader, readerName))
			return &pStates[i];
	}
	return NULL;
}

LONG WinscardImpl::SCardGetStatusChange(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates, IN DWORD cReaders)
{
	LONG result = 0;
	LPSCARD_READERSTATE_A realReaderStates = new SCARD_READERSTATE_A[cReaders];
	DWORD cRealReaders = 0;

	// Populate realReaderStates with only the hard readers
	for (size_t i = 0; i < cReaders; i++)
	{
		SoftReader *sr = srmngr->getSoftReaderByName(rgReaderStates[i].szReader);
		const char *hardReaderName = (sr == NULL) ? rgReaderStates[i].szReader : sr->getHardReaderName().c_str();
		// If the hard reader state was already added, don't add it again
		if (GetState(realReaderStates, cRealReaders, hardReaderName) == NULL)
		{
			realReaderStates[cRealReaders] = rgReaderStates[i];
			realReaderStates[cRealReaders++].szReader = hardReaderName;
		}
	}

	//Send to hard reader
	result = pSCardGetStatusChange(hContext, dwTimeout, realReaderStates, cRealReaders);

	//Update the rgReaderStates
	for (size_t i = 0; i < cReaders; i++)
	{
		LPSCARD_READERSTATE_A pHardReaderState = NULL;
		SoftReader *sr = srmngr->getSoftReaderByName(rgReaderStates[i].szReader);
		if (sr)
			pHardReaderState = GetState(realReaderStates, cRealReaders, sr->getHardReaderName().c_str());
		else
			pHardReaderState = GetState(realReaderStates, cRealReaders, rgReaderStates[i].szReader);

		if (pHardReaderState)
		{
			rgReaderStates[i].dwEventState   = pHardReaderState->dwEventState;
			rgReaderStates[i].dwCurrentState = pHardReaderState->dwCurrentState;
			rgReaderStates[i].cbAtr          = pHardReaderState->cbAtr;
			memcpy(rgReaderStates[i].rgbAtr, pHardReaderState->rgbAtr, 36);
		}
	}

	delete[] realReaderStates;

	return result;
}

#ifndef __OLD_PCSC_API__
LONG WinscardImpl::SCardControl(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
	IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
	OUT LPVOID lpOutBuffer,
	IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned)
{
	LONG           result;
	SoftCard       *sc = srmngr->getSoftCard(hCard);
	if (sc)   //send to hard reader with hard handle
		result = pSCardControl(sc->getHardHandle(), dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
	else     //must be a hard handle already
		result = pSCardControl(hCard, dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);

	return result;
}
#else  // On Mac OS X

#ifdef USE_SCARD_CONTROL_112  // not for 10.4
LONG WinscardImpl::SCardControl132(IN SCARDHANDLE hCard, IN DWORD dwControlCode,
	IN LPCVOID lpInBuffer, IN DWORD nInBufferSize,
	OUT LPVOID lpOutBuffer,
	IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned)
{
	LONG           result;
	SoftCard       *sc = srmngr->getSoftCard(hCard);
	if (sc)   //send to hard reader with hard handle
		result = pSCardControl132(sc->getHardHandle(), dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);
	else     //must be a hard handle already
		result = pSCardControl132(hCard, dwControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned);

	return result;
}
#endif

LONG WinscardImpl::SCardControl(IN SCARDHANDLE IN hCard,
	IN const unsigned char *lpInBuffer, IN DWORD nInBufferSize,
	OUT unsigned char * lpOutBuffer, OUT LPDWORD lpBytesReturned)
{
	LONG           result;
	SoftCard       *sc = srmngr->getSoftCard(hCard);
	if (sc)   //send to hard reader with hard handle
		result = pSCardControl(sr->getHardHandle(), lpInBuffer, nInBufferSize, lpOutBuffer, lpBytesReturned);
	else     //must be a hard handle already
		result = pSCardControl(hCard, lpInBuffer, nInBufferSize, lpOutBuffer, lpBytesReturned);

	return result;
}
#endif

bool WinscardImpl::setSelectedOnHardCard(const SoftCard* softCard) const
{
	SCARD_IO_REQUEST ioSendPci;
	ioSendPci.dwProtocol = SCARD_PROTOCOL_T0;
	ioSendPci.cbPciLength = sizeof(ioSendPci);
	LPCSCARD_IO_REQUEST pioSendPci = &ioSendPci;
	BYTE                pbRecvBuffer[2];
	DWORD               pcbRecvLength = 2;

	bool bAppletSelected = false;

try_again:

	const File *f = softCard->getSelected();
	BYTE       *pbSendBuffer;
	DWORD      cbSendLength;
	if (f->isRoot())
	{
		cbSendLength = 5 + 2;
		pbSendBuffer = new BYTE[cbSendLength];
		BYTE SelectFileById[5] = { 0x00, 0xA4, 0x02, 0x0C, 0x02 };
		memcpy(pbSendBuffer, SelectFileById, 5);
		memcpy(pbSendBuffer + 5, ROOT_ID, 2);
	}
	else
	{
		BYTE   path[MAX_PATH_LEN];
		size_t pathLen = MAX_PATH_LEN;
		f->getPath(path, &pathLen);
		cbSendLength = (DWORD) (5 + pathLen);
		pbSendBuffer = new BYTE[cbSendLength];
		BYTE SelectFileByPath[4] = { 0x00, 0xA4, 0x08, 0x0C };
		memcpy(pbSendBuffer, SelectFileByPath, 4);
		pbSendBuffer[4] = (BYTE) pathLen;
		memcpy(pbSendBuffer + 5, path, pathLen);
	}

	pcbRecvLength = 2;
	LONG result = pSCardTransmit(softCard->getHardHandle(), pioSendPci, pbSendBuffer, cbSendLength,
		NULL, pbRecvBuffer, &pcbRecvLength);
	delete[] pbSendBuffer;

	if (SCARD_S_SUCCESS != result)
		return false;

	bool bRet = memcmp(pbRecvBuffer, carddata::NORMAL_ENDING_OF_CMD, 2) == 0;

	if (!bRet && !bAppletSelected)
	{
		// Select applet
		pcbRecvLength = 2;
		result = pSCardTransmit(softCard->getHardHandle(), pioSendPci, carddata::APDU_SELECT_APPLET, carddata::SIZEOF_SEL_APPL,
			NULL, pbRecvBuffer, &pcbRecvLength);
		if (SCARD_S_SUCCESS == result && (pbRecvBuffer[0] == 0x90 || pbRecvBuffer[0] == 0x61))
		{
			bAppletSelected = true;
			goto try_again;
		}
	}

	return bRet;
}

bool WinscardImpl::loadSoftData(SoftCard* softcard) const
{
	SCARD_IO_REQUEST ioSendPci;
	ioSendPci.dwProtocol = SCARD_PROTOCOL_T0;
	ioSendPci.cbPciLength = sizeof(ioSendPci);
	LPCSCARD_IO_REQUEST pioSendPci = &ioSendPci;

	BYTE                pbRecvBuffer[30];
	DWORD               pcbRecvLength = 30;
	SCARDHANDLE         hHardCard = softcard->getHardHandle();

	// Before reading out the Card Data, do an SCardBeginTransaction and try to select the BE eID applet

	LONG                result        = pSCardBeginTransaction(hHardCard);  // SCardBeginTransaction()
	if (result != SCARD_S_SUCCESS)
		logError("Can't do an SCardBeginTransaction(): 0x%0x (%d)\n", result, result);

	result        = pSCardTransmit(hHardCard, pioSendPci, carddata::APDU_SELECT_APPLET, carddata::SIZEOF_SEL_APPL, NULL, pbRecvBuffer, &pcbRecvLength);

	pcbRecvLength = 30;
	result        = pSCardTransmit(hHardCard, pioSendPci, carddata::APDU_GET_CARD_DATA, carddata::SIZEOF_APDU, NULL, pbRecvBuffer, &pcbRecvLength);

	pSCardEndTransaction(hHardCard, SCARD_LEAVE_CARD);  // SCardEndTransaction()

	const BYTE          *status       = pbRecvBuffer + pcbRecvLength - 2;
	bool                ret           = (SCARD_S_SUCCESS == result && memcmp(status, carddata::NORMAL_ENDING_OF_CMD, 2) == 0);
	if (!ret)
	{
		if (result != SCARD_S_SUCCESS)
			logError("Can't get Card Data from physical card: 0x%0x (%d)\n", result, result);
		else
			logInfo("Can't get Card Data from physical card: SW12 = %02X %02X\n",
				pbRecvBuffer[pcbRecvLength - 2], pbRecvBuffer[pcbRecvLength - 1]);
		return false;
	}
	std::string file = srmngr->getControl()->getSoftCardFileLocation(pbRecvBuffer);
	ret = softcard->loadFromFile(file, pbRecvBuffer);
	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// Problem on Linux and some Mac OS-es: if you have multiple threads that do an SCardConnect()
// to the same card and then do an SCardBeginTransaction(), then this function hangs PCSC as
// soon as it's called by the 2nd thread. (Checked on Kubuntu and Mac OS 10.5 PPC, unable to
// reproduce on Mac OS 10.4).
// This problem occurs if you use the BE eID middleware GUI and set the reader visibility in
// the control file to REAL_FIRST and REAL_LAST. In this case there's a thread for the real
// and one for the virtual reader that both do an SCardConnect() to the same real card and
// and SCardBeginTransaction() afterwards.
// Our solution is to allow only 1 thread to do an SCardConnect() for each reader. If another
// tries to do the same, the card handle from the 1st thread is returned. The 1st thread can
// do other SCardConnect() calls, if other thread try to do this, they are all provided with
// the first card handle for the 1st thread.

LONG WinscardImpl::SCardConnect_Threading(
	IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
	OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol)
{
    if (!g_listReadersCalled)
        ForceListReaders(hContext);

#ifdef _WIN32
	return pSCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
#else
	EidInfra::CAutoMutex autoMutex(&cardThreadPoolMutex);

	int stat = cardThreadPool.GetCardHandle(szReader, phCard);
	if (stat == CTP_USE_THIS_HANDLE)
	{
		// There's already a card handle for this reader and it was done by another thread.
		// So we use the same card handle here.
		return SCARD_S_SUCCESS;
	}
	else
	{
		// There are 2 possibilities:
		//  1. there's no card handle yet for this reader -> do a SCardConnect() and add the card handle
		//  2. there's already a card handle for this reader and it was done by the same thread -> only do an SCardConnect()
		LONG ret = pSCardConnect (hContext, szReader, dwShareMode, dwPreferredProtocols, phCard, pdwActiveProtocol);
		if (ret == SCARD_S_SUCCESS && stat == CTP_DO_CONNECT_AND_ADD)
			cardThreadPool.addCardHandle(szReader, *phCard); // case 1
		return ret;
	}
#endif
}

LONG WinscardImpl::SCardDisconnect_Threading(IN SCARDHANDLE hCard, IN DWORD dwDisposition)
{
#ifdef _WIN32
	return pSCardDisconnect(hCard, dwDisposition);
#else
	EidInfra::CAutoMutex autoMutex(&cardThreadPoolMutex);

	int stat = cardThreadPool.disconnect(hCard);
	if (stat == CTP_DO_DISCONNECT)
	{
		// Either the card handle wasn't known, or it was known and we can disconnect
		return pSCardDisconnect(hCard, dwDisposition);
	}
	else
	{
		// The card handle was known but we can't disconnect yet
		return SCARD_S_SUCCESS;
	}
#endif
}

#ifndef _WIN32

CardThreadPool::CardThreadPool()
{
}

CardThreadPool::~CardThreadPool()
{
}

int CardThreadPool::GetCardHandle(IN LPCSTR szReader, IN OUT LPSCARDHANDLE phCard)
{
	std::string reader = szReader;

	std::vector<ReaderThreadHandle>::iterator it;
	for (it = readerThreadHandles.begin(); it != readerThreadHandles.end(); it++)
	{
		if (it->reader == reader)
		{
			if (pthread_equal(it->threadID, pthread_self()))
			{
				return CTP_DO_CONNECT;
			}
			else
			{
				*phCard = it->handle;
				it->counter++;
				return CTP_USE_THIS_HANDLE;
			}
		}
	}

	return CTP_DO_CONNECT_AND_ADD;

}

void CardThreadPool::addCardHandle(IN LPCSTR szReader, IN SCARDHANDLE hCard)
{
	ReaderThreadHandle readerThreadHandle;

	readerThreadHandle.reader = szReader;
	readerThreadHandle.threadID = pthread_self();
	readerThreadHandle.handle = hCard;
	readerThreadHandle.counter = 1;

	readerThreadHandles.push_back(readerThreadHandle);
}

int CardThreadPool::disconnect(IN SCARDHANDLE hCard)
{
	std::vector<ReaderThreadHandle>::iterator it;
	for (it = readerThreadHandles.begin(); it != readerThreadHandles.end(); it++)
	{
		if (it->handle == hCard)
		{
			if (it->counter == 1)
			{
				readerThreadHandles.erase(it);
				return CTP_DO_DISCONNECT;
			}
			else
			{
				it->counter--;
				return CTP_DONT_DISCONNECT;
			}
		}
	}

	return CTP_DO_DISCONNECT;
}

#endif

