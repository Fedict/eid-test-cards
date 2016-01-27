//
// Copyright Zetes S.A. Belgium 2008
//
// Created by: Rogier Taal
//

#ifndef SOFTCARD_H
#define SOFTCARD_H

#include "APDURequest.h"
#include "ArrayBuffer.h"
#include "SoftReader.h"
#include "Mutex.h"
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#define IN
#define OUT
#endif

namespace zetes { namespace pcscproxy {
				  class Transaction;
				  class FileStructure;
				  class File;
				  class SoftCard {
public:

				  SoftCard(SoftReader *softReader, SCARDHANDLE hardhandle, SCARDHANDLE softhandle);
				  ~SoftCard();

				  SoftReader *getSoftReader() const;
				  SCARDHANDLE getHardHandle() const;
				  SCARDHANDLE getSoftHandle() const;

				  LONG transmit(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  bool loadFromFile(const std::string &filename, const BYTE* chipnr);

				  bool isDataLoaded() const;

				  void reset();

				  const File* const getSelected() const;

				  LONG beginTransaction();
				  LONG endTransaction(DWORD dwDisposition);
  				  bool firstTimeInTransaction() const;
				  void setLastCmdToCard(bool lastCmdToCard);
				  bool lastCmdWasToCard() const;
private:

				  SoftReader *itsSoftReader;

				  const SCARDHANDLE itsHardHandle, itsSoftHandle;

				  Transaction       * itsTransaction;
				  BYTE              * chipnr;
				  BYTE              * versionnumbers;
				  FileStructure     * fs;
				  ArrayBuffer<BYTE> * responseBuffer;
				  bool              lastCmdToCard;

				  EidInfra::CMutex  scMutex;

				  LONG getCardData(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG readBinary(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG selectFile(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG getResponse(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  SoftCard();                           //do not implement
				  SoftCard(const SoftCard&);            //do not implement
				  SoftCard& operator=(const SoftCard&); //do not implement
				  };                                    // SoftCard
				  } //pcscproxy
} //zetes

#endif
