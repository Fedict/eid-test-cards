//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef WINSCARDIMPL_H
#define WINSCARDIMPL_H

#ifdef UNICODE
#undef UNICODE
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#include <stddef.h>
#include <pthread.h>
#include "Mutex.h"
#define LPSCARD_READERSTATE_A LPSCARD_READERSTATE
#define IN
#define OUT
#define __stdcall
#endif
#include <string>
#include <vector>

#include "proxy_winscard.h"

namespace zetes { namespace pcscproxy {

///////////////////////////////// Helper class ////////////////////////////////////

#ifndef _WIN32
				  const int CTP_USE_THIS_HANDLE = 1;
				  const int CTP_DO_CONNECT = 2;
				  const int CTP_DO_CONNECT_AND_ADD = 3;
				  const int CTP_DO_DISCONNECT = 11;
				  const int CTP_DONT_DISCONNECT = 12;

				  typedef struct {
					std::string reader;
					 pthread_t threadID;
					SCARDHANDLE handle;
					unsigned long counter;
				  } ReaderThreadHandle;

				  class CardThreadPool {
public:
				  CardThreadPool();
				  ~CardThreadPool();

				  /** Returns CTP_USE_THIS_HANDLE, CTP_DO_CONNECT or CTP_DO_CONNECT_AND_ADD */
				  int GetCardHandle(IN LPCSTR szReader, IN OUT LPSCARDHANDLE phCard);
				  void addCardHandle(IN LPCSTR szReader, IN SCARDHANDLE hCard);
				  /**  Returns CTP_DO_DISCONNECT or CTP_DONT_DISCONNECT */
				  int disconnect(IN SCARDHANDLE hCard);

private:
				  std::vector<ReaderThreadHandle> readerThreadHandles;
				  };
#endif

//////////////////////////////////// WinscardImpl /////////////////////////////////////

class SoftReaderManager;
class SoftCard;

class WinscardImpl {
public:
				  WinscardImpl();
				  ~WinscardImpl();

				  LONG SCardBeginTransaction(IN SCARDHANDLE hCard);

				  LONG SCardConnect(IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					  OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);

				  LONG SCardDisconnect(IN SCARDHANDLE hCard, IN DWORD dwDisposition);

				  LONG SCardEndTransaction(IN SCARDHANDLE hCard, IN DWORD dwDisposition);

				  LONG SCardListReaders(IN SCARDCONTEXT hContext, IN LPCSTR mszGroups, OUT LPSTR mszReaders, IN OUT LPDWORD pcchReaders);

				  LONG SCardReconnect(IN SCARDHANDLE hCard, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					  IN DWORD dwInitialization, OUT LPDWORD pdwActiveProtocol);

				  LONG SCardGetAttrib(IN SCARDHANDLE hCard, IN DWORD dwAttrId, OUT LPBYTE pbAttr, IN OUT LPDWORD pcbAttrLen);

#ifdef _WIN32
                  LONG SCardState(IN SCARDHANDLE hCard, OUT LPDWORD pdwState, OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, IN OUT LPDWORD pcbAtrLen);
#endif

                  LONG SCardTransmit(IN SCARDHANDLE hCard, IN LPCSCARD_IO_REQUEST pioSendPci, IN LPCBYTE pbSendBuffer,
					  IN DWORD cbSendLength, IN OUT LPSCARD_IO_REQUEST pioRecvPci, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength);

				  LONG SCardFreeMemory(SCARDCONTEXT hContext, LPVOID pvMem);

				  LONG SCardStatus(IN SCARDHANDLE hCard, OUT LPSTR szReaderName, IN OUT LPDWORD pcchReaderLen, OUT LPDWORD pdwState,
					  OUT LPDWORD pdwProtocol, OUT LPBYTE pbAtr, OUT LPDWORD pcbAtrLen);

				  LONG SCardGetStatusChange(IN SCARDCONTEXT hContext, IN DWORD dwTimeout, IN OUT LPSCARD_READERSTATE_A rgReaderStates, IN DWORD cReaders);

#ifndef __OLD_PCSC_API__
				  LONG SCardControl(IN SCARDHANDLE hCard, IN DWORD dwControlCode, IN LPCVOID lpInBuffer, IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer,
					  IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
#else
				  LONG SCardControl132(IN SCARDHANDLE hCard, IN DWORD dwControlCode, IN LPCVOID lpInBuffer, IN DWORD nInBufferSize, OUT LPVOID lpOutBuffer,
					  IN DWORD nOutBufferSize, OUT LPDWORD lpBytesReturned);
				  LONG SCardControl(IN SCARDHANDLE IN hCard,
					  IN const unsigned char *pbSendBuffer, IN DWORD cbSendLength,
					  OUT unsigned char *pbRecvBuffer, OUT LPDWORD pcbRecvLength);
#endif

                  std::vector<void *> m_autoAllocs;

private:
				  SoftReaderManager * srmngr;
				  WinscardImpl(const WinscardImpl&);            //do not implement
				  WinscardImpl& operator=(const WinscardImpl&); //do not implement

				  bool setSelectedOnHardCard(const SoftCard*) const;
				  bool loadSoftData(SoftCard*) const;

				  void ForceListReaders(IN SCARDCONTEXT hContext);
				  bool g_listReadersCalled;

				  LONG SCardConnect_Threading(
					IN SCARDCONTEXT hContext, IN LPCSTR szReader, IN DWORD dwShareMode, IN DWORD dwPreferredProtocols,
					OUT LPSCARDHANDLE phCard, OUT LPDWORD pdwActiveProtocol);
				  LONG SCardDisconnect_Threading(IN SCARDHANDLE hCard, IN DWORD dwDisposition);
#ifndef _WIN32
				  CardThreadPool cardThreadPool;
				  EidInfra::CMutex cardThreadPoolMutex;
#endif
};

} //pcscproxy
} //zetes

#endif
